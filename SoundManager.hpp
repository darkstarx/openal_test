#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include "forwards.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <list>
#include <map>


class SoundManager;


class SoundManagerDestroyer
{
public:
	~SoundManagerDestroyer();
	inline void init(SoundManager *instance) { m_instance = instance; }
private:
	SoundManager *m_instance;
};


class SoundManager
{
public:
	typedef std::list<SoundSPtr> sounds_t;
	typedef std::map<std::string, SoundBufferSPtr> buffers_t;

private:
	friend class SoundManagerDestroyer;

	/**
	 * Запрещается создавать
	 */
	SoundManager();

	/**
	 * Запрещается разрушать
	 */
	~SoundManager();

public:
	/**
	 * Запрещается копировать
	 */
	SoundManager(const SoundManager &) = delete;

	/**
	 * Запрещается присваивать
	 */
	const SoundManager& operator =(const SoundManager&) = delete;

	static SoundManager& instance();

	inline ALCdevice* device() { return m_device; }

	void preload_sound(const std::string &filename, const std::string &name);

	void play_sound(const std::string &name);

	void pause_sound(const std::string &name);

	void stop_sound(const std::string &name);

private:
	static SoundManager *m_instance;
	static SoundManagerDestroyer m_destroyer;

	ALCint m_max_mono_sources;
	ALCint m_max_stereo_sources;
	ALCdevice *m_device;
	ALCcontext *m_context;
	buffers_t m_buffers;
	sounds_t m_mono_sounds;
	sounds_t m_stereo_sounds;

	/**
	 * @brief request_idle_sound
	 *   Возвращает объект свободного на данный момент источника звука, который можно использовать для воспроизведения
	 *   другого аудиобуфера. Если свободный источник звука не найден, пытается создать новый.
	 * @param stereo Для какого типа аудиобуфера требуется источник звука (mono/stereo)
	 * @return Возвращает указатель на объект звука, либо пустой указатель, если превышен лимит кол-ва источников звука
	 *   данного типа, поддерживаемых устройством.
	 */
	SoundSPtr request_idle_sound(bool stereo);

	SoundSPtr playing_sound(const std::string &name);

	SoundSPtr paused_sound(const std::string &name);

	SoundSPtr sound_in_state(bool stereo, const std::string &name, int state);

};

inline SoundManager& sound_mgr() { return SoundManager::instance(); }

#endif // SOUNDMANAGER_HPP
