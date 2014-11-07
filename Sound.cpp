#include "Sound.hpp"
#include "SoundBuffer.hpp"
#include "alcheck.hpp"
#include <vorbis/codec.h>


Sound::Sound()
: m_id(0)
, m_buf()
{
	// Создаем источник соответствующий нашему звуку
	alGenSources(1, &m_id);
	if (!CheckALError()) return;

	alSourcef (m_id, AL_PITCH, 1.0f);
	CheckALError();
	alSourcef (m_id, AL_GAIN, 1.0f);
	CheckALError();
	alSourcefv(m_id, AL_POSITION, m_pos.array());
	CheckALError();
	alSourcefv(m_id, AL_VELOCITY, m_vel.array());
	CheckALError();
	alSourcei (m_id, AL_LOOPING, AL_FALSE);
	CheckALError();
}


Sound::~Sound()
{
	if (!m_id) return;
	alSourceStop(m_id);
	CheckALError();
	alDeleteSources(1, &m_id);
	CheckALError();
}


void Sound::set_position(const position_t &value)
{
	if (m_pos == value || !m_id) return;
	m_pos = value;
	alSourcefv(m_id, AL_POSITION,  m_pos.array());
	CheckALError();
}


void Sound::set_velocity(const position_t &value)
{
	if (m_vel == value || !m_id) return;
	m_vel = value;
	alSourcefv(m_id, AL_VELOCITY,  m_vel.array());
	CheckALError();
}


void Sound::attach_buffer(const SoundBufferSPtr &buf)
{
	alSourcei(m_id, AL_BUFFER, buf->id());
	m_buf = buf;
	CheckALError();
}


void Sound::detach_buffer()
{
	alSourcei(m_id, AL_BUFFER, 0);
	m_buf.reset();
	CheckALError();
}


int Sound::state()
{
	if (!m_id) return 0;
	int ret;
	alGetSourcei(m_id, AL_SOURCE_STATE, &ret);
	if (!CheckALError()) return 0;
	return ret;
}


void Sound::play()
{
	if (!m_id || !m_buf) return;
	alSourcePlay(m_id);
	CheckALError();
}


void Sound::pause()
{
	if (!m_id || !m_buf) return;
	alSourcePause(m_id);
	CheckALError();
}


void Sound::stop()
{
	if (!m_id || !m_buf) return;
	alSourceStop(m_id);
	CheckALError();
}
