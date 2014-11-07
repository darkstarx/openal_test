#include "SoundBuffer.hpp"
#include "alcheck.hpp"
#include "log.hpp"
#include <vorbis/codec.h>


SoundBuffer::SoundBuffer(const std::string &filename, const std::string &name)
: m_id(0)
, m_stereo(false)
, m_filename(filename)
, m_name(name)
{
	// Создаем буфер
	alGenBuffers(1, &m_id);
	if (!CheckALError()) return;

	// Пытаемся загрузить файл в буфер
	if (!load()) {
		alDeleteBuffers(1, &m_id);
		CheckALError();
		m_id = 0;
	}
}


SoundBuffer::~SoundBuffer()
{
	if (!m_id) return;
	alDeleteBuffers(1, &m_id);
	CheckALError();
	m_ogg_file.close();
	delete m_vfile;
}


bool SoundBuffer::load()
{
	// Открываем OGG файл как бинарный
	m_ogg_file.open(m_filename.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!m_ogg_file.is_open()) return false;

	// Структура с функциями обратного вызова
	ov_callbacks cb;
	// Заполняем структуру cb
	cb.read_func = read_ogg;
	cb.seek_func = seek_ogg;
	cb.tell_func = tell_ogg;
	cb.close_func = close_ogg;

	// Создаем структуру OggVorbis_File
	m_vfile = new OggVorbis_File;

	// Инициализируем файл средствами vorbisfile
	if (ov_open_callbacks(&m_ogg_file, m_vfile, NULL, -1, cb) < 0) {
		// Если ошибка, то открываемый файл не является OGG
		m_ogg_file.close();
		delete m_vfile;
		m_vfile = 0;
		return false;
	}

	// Получаем комментарии и информацию о файле
	m_vcomment = ov_comment(m_vfile, -1);
	m_vinfo = ov_info(m_vfile, -1);

	// Размер блока – весь файл
	int block_size = ov_pcm_total(m_vfile, -1) * 4;

	// Формат аудио
	if (m_vinfo->channels > 2) {
		LOG("Loading of audio containing more than 2 channels. Not supported.");
		return false;
	}
	int format = (m_vinfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	m_stereo = m_vinfo->channels > 1;
	// Считываем блок данных
	read_ogg_block(block_size, format, m_vinfo->rate);

	return true;
}


bool SoundBuffer::read_ogg_block(size_t size, int format, int rate)
{
	if (size < 1) return false;

	// Текущая секция файла
	int current_section;
	// Общий объем прочтенных данных
	size_t total_ret = 0;
	// Объем прочтенных данных на текущей итерации
	long ret;
	// Буфер данных
	char *pcm = new char[size];

	// Цикл чтения
	while (total_ret < size)
	{
		ret = ov_read(m_vfile, pcm + total_ret, size - total_ret, 0, m_vinfo->channels, 1, &current_section);
		// Если достигнут конец файла
		if (ret == 0) {
			break;
		} else if (ret < 0) {
			// Ошибка в потоке, хз может ли быть такое
			// TODO: Обрабатывать ошибку
		} else {
			total_ret += ret;
		}
	}
	if (total_ret > 0) {
		alBufferData(m_id, format, (void *)pcm, total_ret, rate);
		CheckALError();
	}
	delete[] pcm;
	return ret > 0;
}


size_t read_ogg(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	std::istream *file = reinterpret_cast<std::istream*>(datasource);
	file->read((char *)ptr, size * nmemb);
	return file->gcount();
}


int seek_ogg(void *datasource, ogg_int64_t offset, int whence)
{
	std::istream *file = reinterpret_cast<std::istream*>(datasource);
	std::ios_base::seekdir dir;
	file->clear();
	switch (whence)
	{
		case SEEK_SET: dir = std::ios::beg;  break;
		case SEEK_CUR: dir = std::ios::cur;  break;
		case SEEK_END: dir = std::ios::end;  break;
		default: return -1;
	}
	file->seekg((std::istream::off_type)offset, dir);
	return (file->fail() ? -1 : 0);
}


long tell_ogg(void *datasource)
{
	std::istream *file = reinterpret_cast<std::istream*>(datasource);
	return file->tellg();
}


int close_ogg(void *)
{
	return 0;
}
