#include "alcheck.hpp"
#include "log.hpp"


ALboolean CheckALCError(ALCdevice *device)
{
	ALenum err_code;
	if ((err_code = alcGetError(device)) != ALC_NO_ERROR)
	{
		std::string err = "ALC error: " + std::string(alcGetString(device, err_code));
		LOG(err);
		return AL_FALSE;
	}
	return AL_TRUE;
}

ALboolean CheckALError()
{
	ALenum err_code;
	if ((err_code = alGetError()) != AL_NO_ERROR)
	{
		std::string err = "OpenAL error: " + std::string(alGetString(err_code));
		LOG(err);
		return AL_FALSE;
	}
	return AL_TRUE;
}
