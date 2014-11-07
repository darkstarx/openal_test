#include "SoundManager.hpp"
#include "SoundBuffer.hpp"
#include "Sound.hpp"
#include "alcheck.hpp"
#include "log.hpp"
#include <AL/alut.h>
#include <sstream>
#include <vector>


/** \class SoundManagerDestroyer */

SoundManagerDestroyer::~SoundManagerDestroyer()
{
	delete m_instance;
}



/** \class SoundManager */

SoundManager* SoundManager::m_instance = 0;


SoundManagerDestroyer SoundManager::m_destroyer;


SoundManager::SoundManager()
{
	// Позиция слушателя.
	ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
	// Скорость слушателя.
	ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
	// Ориентация слушателя. (Первые 3 элемента – направление «на», последние 3 – «вверх»)
	ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

	// Открываем заданное по умолчанию устройство
	m_device = alcOpenDevice(NULL);
	CheckALCError(m_device);
	// Проверка на ошибки
	if (!m_device) {
		throw("Default sound device not present");
	}
	// Создаем контекст рендеринга
	m_context = alcCreateContext(m_device, NULL);
	if (!CheckALCError(m_device)) throw("Can't create AL context");

	// Делаем контекст текущим
	alcMakeContextCurrent(m_context);
	CheckALCError(m_device);

	// Устанавливаем параметры слушателя
	// Позиция
	alListenerfv(AL_POSITION, ListenerPos);
	CheckALError();
	// Скорость
	alListenerfv(AL_VELOCITY, ListenerVel);
	CheckALError();
	// Ориентация
	alListenerfv(AL_ORIENTATION, ListenerOri);
	CheckALError();

	// Получаем атрибуты устройства
	ALCint size = 0;
	alcGetIntegerv(m_device, ALC_ATTRIBUTES_SIZE, 1, &size);
	CheckALCError(m_device);
	std::vector<ALCint> attrs(size);
	alcGetIntegerv(m_device, ALC_ALL_ATTRIBUTES, size, &attrs[0]);
	CheckALCError(m_device);
	for (ALCint i = 0; i < size; ++i) {
		if (attrs[i] == ALC_MONO_SOURCES) {
			m_max_mono_sources = attrs.at(i + 1);
			std::stringstream sbuf;
			sbuf << "Max mono sources: " << m_max_mono_sources;
			LOG(sbuf.str());
		} else if (attrs[i] == ALC_STEREO_SOURCES) {
			m_max_stereo_sources = attrs.at(i + 1);
			std::stringstream sbuf;
			sbuf << "Max stereo sources: " << m_max_stereo_sources;
			LOG(sbuf.str());
		}
	}
}


SoundManager::~SoundManager()
{
	// Разрушаем все созданные звуки
	m_mono_sounds.clear();
	m_stereo_sounds.clear();
	// Удаляем все созданные буферы
	m_buffers.clear();
	// Выключаем текущий контекст
	alcMakeContextCurrent(NULL);
	// Уничтожаем контекст
	alcDestroyContext(m_context);
	// Закрываем звуковое устройство
	alcCloseDevice(m_device);
}


SoundManager& SoundManager::instance()
{
	if (!m_instance) {
		m_instance = new SoundManager();
		m_destroyer.init(m_instance);
	}
	return *m_instance;
}


void SoundManager::preload_sound(const std::string &filename, const std::string &name)
{
	// Возможно аудиобуфер уже существует, ищем его по имени
	SoundBufferSPtr &buffer = m_buffers[name];
	// Если буфер еще не создан, создаем его; если в буфер загружены другие данные, создаем новый буфер
	if (!buffer || buffer->filename() != filename) {
		buffer.reset(new SoundBuffer(filename, name));
	}
}


void SoundManager::play_sound(const std::string &name)
{
	// Ищем приостановленный источник звука с таким именем
	SoundSPtr sound = paused_sound(name);
	if (sound) {
		sound->play();
		return;
	}
	// Приостановленных нет, нужно создать новый. Ищем аудиобуфер по имени
	SoundBufferSPtr &buffer = m_buffers[name];
	// Если буфер еще не создан, воспроизводить еще нечего
	if (!buffer) {
		LOG("Audio `" + name + "` is not preloaded");
		return;
	}
	// Запрашиваем подходящий источник звука
	sound = request_idle_sound(buffer->stereo());
	// Если не удалось получить звуковой источник, воспроизвести звук мы не сможем
	if (!sound) return;
	// Подключаем буфер к источнику звука и воспроизводим аудиобуфер
	sound->attach_buffer(buffer);
	sound->play();
}


void SoundManager::pause_sound(const std::string &name)
{
	// Находим источник звука с именем буфера, который воспроизводится
	SoundSPtr sound = paused_sound(name);
	if (sound) sound->pause();
}


void SoundManager::stop_sound(const std::string &name)
{
	// Находим источник звука с именем буфера, который воспроизводится
	SoundSPtr sound = playing_sound(name);
	if (sound) sound->stop();
}


SoundSPtr SoundManager::request_idle_sound(bool stereo)
{
	if (stereo) {
		// Ищем свободный стерео-источник
		for (sounds_t::iterator it = m_stereo_sounds.begin(), end = m_stereo_sounds.end(); it != end; ++it) {
			SoundSPtr &sound = *it;
			int state = sound->state();
			if (state != AL_PAUSED && state != AL_PLAYING) return sound;
		}
		// Свободный источник не найден, создаем новый
		if (m_stereo_sounds.size() >= static_cast<size_t>(m_max_stereo_sources)) {
			return SoundSPtr();
		} else {
			SoundSPtr sound(new Sound());
			m_stereo_sounds.push_back(sound);
			return sound;
		}
	} else {
		// Ищем свободный 3D источник
		for (sounds_t::iterator it = m_mono_sounds.begin(), end = m_mono_sounds.end(); it != end; ++it) {
			SoundSPtr &sound = *it;
			int state = sound->state();
			if (state != AL_PAUSED && state != AL_PLAYING) return sound;
		}
		// Свободный источник не найден, создаем новый
		if (m_mono_sounds.size() >= static_cast<size_t>(m_max_mono_sources)) {
			return SoundSPtr();
		} else {
			SoundSPtr sound(new Sound());
			m_mono_sounds.push_back(sound);
			return sound;
		}
	}
}


SoundSPtr SoundManager::playing_sound(const std::string &name)
{
	// Ищем воспроизводимый источник звука среди стерео
	SoundSPtr ret = sound_in_state(true, name, AL_PLAYING);
	if (ret) return ret;
	// Ищем воспроизводимый источник звука среди моно
	ret = sound_in_state(false, name, AL_PLAYING);
	return ret;
}


SoundSPtr SoundManager::paused_sound(const std::string &name)
{
	// Ищем приостановленный источник звука среди стерео
	SoundSPtr ret = sound_in_state(true, name, AL_PAUSED);
	if (ret) return ret;
	// Ищем приостановленный источник звука среди моно
	ret = sound_in_state(false, name, AL_PAUSED);
	return ret;
}


SoundSPtr SoundManager::sound_in_state(bool stereo, const std::string &name, int state)
{
	sounds_t::iterator it, end;
	if (stereo) {
		it = m_stereo_sounds.begin();
		end = m_stereo_sounds.end();
	} else {
		it = m_mono_sounds.begin();
		end = m_mono_sounds.end();
	}
	for (; it != end; ++it) {
		SoundSPtr &sound = *it;
		if (sound->buffer() && sound->buffer()->name() == name) {
			if (sound->state() == state) return sound;
		}
	}
	return SoundSPtr();
}
