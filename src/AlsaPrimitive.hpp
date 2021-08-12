/*
  Copyright (C) 2021 hidenorly

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef __ALSA_PRIMITIVE_HPP__
#define __ALSA_PRIMITIVE_HPP__

#include "AudioFormat.hpp"
#include "Buffer.hpp"
#if __linux__
#include <alsa/asoundlib.h>
#endif

class AlsaPrimitive
{
protected:
  static inline AudioFormat mLastFormat = AudioFormat(AudioFormat::ENCODING::PCM_UNKNOWN);
#if __linux__
  static inline snd_pcm_t* mHandle = nullptr;
#endif
public:
  static void initialize(void);
  static void terminate(void);
  static bool config(AudioFormat& format);
  static void write(IAudioBuffer& buf);
};

#endif /* __ALSA_PRIMITIVE_HPP__ */
