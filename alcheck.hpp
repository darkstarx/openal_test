#ifndef ALCHECK_HPP
#define ALCHECK_HPP

#include <AL/al.h>
#include <AL/alc.h>


ALboolean CheckALCError(ALCdevice *device);

ALboolean CheckALError();

#endif // ALCHECK_HPP
