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
#include <filesystem>
#include <fstream>
#include "OptParse.hpp"

AudioFormat getAudioFormatFromOpts( std::string encoding, std::string samplingRate, std::string channels )
{
  AudioFormat::ENCODING _encoding = AudioFormat::getEncodingFromString(encoding);
  int _samplingRate = std::stoi(samplingRate);
  AudioFormat::CHANNEL _channel = AudioFormat::getChannelsFromString(channels);

  return AudioFormat(_encoding, _samplingRate, _channel);
}

int main(int argc, char **argv)
{
  std::vector<OptParse::OptParseItem> options;
  options.push_back( OptParse::OptParseItem("-r", "--sampleRate", true, "48000", "Set Sampling Rate"));
  options.push_back( OptParse::OptParseItem("-e", "--encoding", true, "PCM_16BIT", "Set Encoding PCM_8BIT, PCM_16BIT, PCM_24BIT, PCM_32BIT, PCM_FLOAT"));
  options.push_back( OptParse::OptParseItem("-c", "--channel", true, "2", "Set channel 2, 2.1, 4, 4.1, 5, 5.1, 5.1.2, 7.1"));
  options.push_back( OptParse::OptParseItem("-i", "--input", true, "", "Specify input(source) file"));

  std::filesystem::path myPath = argv[0];
  OptParse optParser( argc, argv, options, std::string("Filter executor e.g. ")+std::string(myPath.filename())+std::string(" -s source.pcm") );

  // setup audio format
  AudioFormat format = getAudioFormatFromOpts( optParser.values["-e"], optParser.values["-r"], optParser.values["-c"] );
  std::cout << "Specified audio format : " << format.toString() << std::endl;

  AlsaPrimitive::initialize();    

  if( std::filesystem::exists( optParser.values["-i"] ) ){
#if 0
    std::fstream stream( optParser.values["-i"], std::ios::in | std::ios::binary );
    AudioBuffer buf( format, 256 );
    while( !stream.eof() ){
      stream.read( reinterpret_cast<char*>( buf.getRawBufferPointer() ), buf.getRawBufferSize() );
        AlsaPrimitive::write( buf );
    }
#else
    FILE* fd = fopen(optParser.values["-i"].c_str(), "rb" );
    AudioBuffer buf( format, 256 );
    while( !feof(fd) ){
      fread(reinterpret_cast<char*>( buf.getRawBufferPointer() ), buf.getRawBufferSize(), 1, fd );
      AlsaPrimitive::write( buf );
    }
#endif
  }

  AlsaPrimitive::terminate();
}

