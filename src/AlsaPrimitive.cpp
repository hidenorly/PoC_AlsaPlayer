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

#include "AlsaPrimitive.hpp"
#include <iostream>

#define ALSA_DEVICE_NAME "default"

void AlsaPrimitive::initialize(void)
{
#if __linux__
  mHandle = nullptr;
  int err;
  if( (err = snd_pcm_open(&mHandle, ALSA_DEVICE_NAME, SND_PCM_STREAM_PLAYBACK, 0)) < 0 ){
    std::cout << "AlsaPrimitive::failed to open:" << err << std::endl;
    mHandle = nullptr;
  }
#endif
}

void AlsaPrimitive::terminate(void)
{
#if __linux__
  if( mHandle ){
    snd_pcm_drain(mHandle);
    snd_pcm_close(mHandle);
    mHandle = nullptr;
  }
#endif
}

bool AlsaPrimitive::config(AudioFormat& format)
{
#if __linux__
  if( mHandle ){
    snd_pcm_format_t alsaFormat = SND_PCM_FORMAT_S16_LE;
    switch( format.getEncoding() ){
      case AudioFormat::ENCODING::PCM_8BIT:
        alsaFormat = SND_PCM_FORMAT_S8;
        break;
      case AudioFormat::ENCODING::PCM_16BIT:
        alsaFormat = SND_PCM_FORMAT_S16_LE;
        break;
      case AudioFormat::ENCODING::PCM_24BIT_PACKED:
        alsaFormat = SND_PCM_FORMAT_S24_LE;
        break;
      case AudioFormat::ENCODING::PCM_32BIT:
        alsaFormat = SND_PCM_FORMAT_S32_LE;
        break;
      case AudioFormat::ENCODING::PCM_FLOAT:
        alsaFormat = SND_PCM_FORMAT_FLOAT_LE;
        break;
      default:
        break;
    }
    int err = snd_pcm_set_params(mHandle,
      alsaFormat,
      SND_PCM_ACCESS_RW_INTERLEAVED,
      format.getNumberOfChannels(),
      format.getSamplingRate(),
      1,
      500000
    );
    std::cout << "AlsaPrimitive::config as " << format.toString() << " (ret:" << err << ")" << std::endl;
  }
  return ( mHandle != nullptr );
#else
  return true;
#endif
}


void AlsaPrimitive::write(IAudioBuffer& buf)
{
  AudioBuffer* pBuf = dynamic_cast<AudioBuffer*>(&buf);
  if( pBuf ){
    AudioFormat format = pBuf->getAudioFormat();
    if( !mLastFormat.equal( format ) ){
      if( config( format ) ){
        mLastFormat = format;
      }
    }
    ByteBuffer rawBuffer = pBuf->getRawBuffer();
#if __linux__
    if( mHandle ){
      snd_pcm_sframes_t frames = snd_pcm_writei( mHandle, rawBuffer.data(), rawBuffer.size() );
      std::cout << "AlsaPrimitive::pcm_write() " << frames << " frames" << std::endl;
      if( frames < 0) {
        frames = snd_pcm_recover(mHandle, frames, 0);
      }
    }
#endif
  }
}

