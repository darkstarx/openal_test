#ifndef SOUNDBUFFER_HPP
#define SOUNDBUFFER_HPP

#include <AL/al.h>
#include <vorbis/vorbisfile.h>
#include <fstream>
#include <string>


class SoundBuffer
{
public:
	SoundBuffer(const std::string &filename, const std::string &name);

	~SoundBuffer();

	inline bool valid() { return m_id; }

	inline ALuint id() { return m_id; }

	inline bool stereo() { return m_stereo; }

	inline std::string filename() { return m_filename; }

	inline std::string name() { return m_name; }

private:
	ALuint m_id;
	bool m_stereo;
	std::string m_filename;
	std::string m_name;

	OggVorbis_File *m_vfile;	///< Переменные библиотеки vorbisfile, главная структура описания файла
	vorbis_comment *m_vcomment;	///< Структура комментариев к файлу
	vorbis_info *m_vinfo;		///< Информация о файле
	std::ifstream m_ogg_file;	///< Файловый поток, содержащий ogg файл

	/**
	 * @brief load
	 *   Загрузить данные из файла в аудиобуфер.
	 * @return
	 */
	bool load();

	/**
	 * @brief read_ogg_block
	 *   Прочитать из файла определенное кол-во байт сырых данных и поместить их в аудиобуфер.
	 * @param size Сколько байт сырых данных следует прочитать и поместить в аудиобуфер.
	 * @param format Формат аудиоданных.
	 * @param rate Частота аудио.
	 * @return true если файл не прочитан до конца, false если достигнут конец файла.
	 */
	bool read_ogg_block(size_t size, int format, int rate);
};

size_t read_ogg(void *ptr, size_t size, size_t nmemb, void *datasource);
int seek_ogg(void *datasource, ogg_int64_t offset, int whence);
long tell_ogg(void *datasource);
int close_ogg(void *);

#endif // SOUNDBUFFER_HPP
