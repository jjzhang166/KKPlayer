/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is SDL_free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#ifndef _SDL_sysaudio_h
#define _SDL_sysaudio_h

#include "SDL_mutex.h"
#include "SDL_thread.h"

/* The SDL audio driver */
typedef struct SDL_AudioDevice SDL_AudioDevice;

/* Define the SDL audio driver structure */
#define _THIS	SDL_AudioDevice *_this
#ifndef _STATUS
#define _STATUS	SDL_status *status
#endif
struct SDL_AudioDevice {
	/* * * */
	/* The name of this audio driver */
	const char *name;

	/* * * */
	/* The description of this audio driver */
	const char *desc;

	/* * * */
	/* Public driver functions */
	int  (*OpenAudio)(_THIS, SDL_AudioSpec *spec);
	void (*ThreadInit)(_THIS);	/* Called by audio thread at start */
	void (*WaitAudio)(_THIS);
	void (*PlayAudio)(_THIS);
	Uint8 *(*GetAudioBuf)(_THIS);
	void (*WaitDone)(_THIS);
	void (*CloseAudio)(_THIS);

	/* * * */
	/* Lock / Unlock functions added for the Mac port */
	void (*LockAudio)(_THIS);
	void (*UnlockAudio)(_THIS);

	void (*SetCaption)(_THIS, const char *caption);

	/* * * */
	/* Data common to all devices */

	/* The current audio specification (shared with audio thread) */
	SDL_AudioSpec spec;

	/* An audio conversion block for audio format emulation */
	SDL_AudioCVT convert;

	/* Current state flags */
	int enabled;
	int paused;
	int opened;

	/* Fake audio buffer for when the audio hardware is busy */
	Uint8 *fake_stream;

	/* A semaphore for locking the mixing buffers */
	SDL_mutex *mixer_lock;

	/* A thread to feed the audio device */
	SDL_Thread *thread;
	Uint32 threadid;

	/* * * */
	/* Data private to this driver */
	struct SDL_PrivateAudioData *hidden;

	/* * * */
	/* The function used to dispose of this structure */
	void (*free)(_THIS);
};
#undef _THIS

typedef struct AudioBootStrap {
	const char *name;
	const char *desc;
	int (*available)(void);
	SDL_AudioDevice *(*create)(int devindex);
} AudioBootStrap;


#if SDL_AUDIO_DRIVER_DSOUND
extern AudioBootStrap DSOUND_bootstrap;
#endif
#if SDL_AUDIO_DRIVER_WAVEOUT
extern AudioBootStrap WAVEOUT_bootstrap;
#endif

#if SDL_AUDIO_DRIVER_DISK
extern AudioBootStrap DISKAUD_bootstrap;
#endif
#if SDL_AUDIO_DRIVER_DUMMY
extern AudioBootStrap DUMMYAUD_bootstrap;
#endif


#endif /* _SDL_sysaudio_h */
