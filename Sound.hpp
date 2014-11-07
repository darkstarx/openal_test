#ifndef SOUND_HPP
#define SOUND_HPP

#include "types.hpp"
#include "forwards.hpp"
#include <AL/al.h>
#include <vorbis/vorbisfile.h>
#include <string>
#include <memory>
#include <fstream>


class Sound
{
public:
	Sound();

	~Sound();

	inline bool valid() { return m_id; }

	inline ALuint id() { return m_id; }

	inline SoundBufferSPtr buffer() { return m_buf; }

	void set_position(const position_t &value);

	void set_velocity(const position_t &value);

	void attach_buffer(const SoundBufferSPtr &buf);

	void detach_buffer();

	int state();

	void play();

	void pause();

	void stop();

private:
	ALuint m_id;
	SoundBufferSPtr m_buf;
	position_t m_pos;
	position_t m_vel;
};


#endif // SOUND_HPP
