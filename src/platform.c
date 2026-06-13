/*
 * This file is part of Lunarglow.
 *
 * Copyright (C) 2015-2017 Iwan Timmer
 *
 * Moonlight is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Moonlight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Moonlight; if not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE

#include "platform.h"

#include "audio/audio.h"
#include "video/video.h"

#include <string.h>

enum platform platform_check(char* name) {
  bool std = strcmp(name, "auto") == 0;
  #ifdef HAVE_X11
  bool x11 = strcmp(name, "x11") == 0;
  bool vdpau = strcmp(name, "x11_vdpau") == 0;
  bool vaapi = strcmp(name, "x11_vaapi") == 0;
  if (std || x11 || vdpau || vaapi) {
    int init = x11_init(std || vdpau, std || vaapi);
    #ifdef HAVE_VAAPI
    if (init == INIT_VAAPI)
      return X11_VAAPI;
    #endif
    #ifdef HAVE_VDPAU
    if (init == INIT_VDPAU)
      return X11_VDPAU;
    #endif
    #ifdef HAVE_SDL
    return SDL;
    #else
    return X11;
    #endif
  }
  #endif
  #ifdef HAVE_SDL
  if (std || strcmp(name, "sdl") == 0)
    return SDL;
  #endif

  if (strcmp(name, "fake") == 0)
    return FAKE;

  return 0;
}

void platform_start(enum platform system) {
}

void platform_stop(enum platform system) {
}

DECODER_RENDERER_CALLBACKS* platform_get_video(enum platform system) {
  switch (system) {
  #ifdef HAVE_X11
  case X11:
    return &decoder_callbacks_x11;
  #ifdef HAVE_VAAPI
  case X11_VAAPI:
    return &decoder_callbacks_x11_vaapi;
  #endif
  #ifdef HAVE_VDPAU
  case X11_VDPAU:
    return &decoder_callbacks_x11_vdpau;
  #endif
  #endif
  #ifdef HAVE_SDL
  case SDL:
    return &decoder_callbacks_sdl;
  #endif
  }
  return NULL;
}

AUDIO_RENDERER_CALLBACKS* platform_get_audio(enum platform system, char* audio_device) {
  switch (system) {
  case FAKE:
      return NULL;
  #ifdef HAVE_SDL
  case SDL:
    return &audio_callbacks_sdl;
  #endif
  default:
    #ifdef HAVE_PULSE
    if (audio_pulse_init(audio_device))
      return &audio_callbacks_pulse;
    #endif
    #ifdef HAVE_ALSA
    return &audio_callbacks_alsa;
    #endif
    #ifdef __FreeBSD__
    return &audio_callbacks_oss;
    #endif
  }
  return NULL;
}

bool platform_prefers_codec(enum platform system, enum codecs codec) {
  switch (codec) {
  case CODEC_H264:
    // H.264 is always supported
    return true;
  case CODEC_HEVC:
    switch (system) {
    case X11_VAAPI:
    case X11_VDPAU:
      return true;
    }
    return false;
  case CODEC_AV1:
    return false;
  }
  return false;
}

char* platform_name(enum platform system) {
  switch(system) {
  case X11:
    return "X Window System (software decoding)";
  case X11_VAAPI:
    return "X Window System (VAAPI)";
  case X11_VDPAU:
    return "X Window System (VDPAU)";
  case SDL:
    return "SDL2 (software decoding)";
  case FAKE:
    return "Fake (no a/v output)";
  default:
    return "Unknown";
  }
}
