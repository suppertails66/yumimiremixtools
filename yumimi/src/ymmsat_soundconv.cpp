#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TSoundFile.h"
#include <iostream>

using namespace std;
using namespace BlackT;

const int headerSize = 0x20;
const unsigned int initialDecryptionKey = 0x90BC;
const unsigned int decryptionKeyIncrement = 0x7654;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yumimi Mix Remix (Saturn) raw audio file to WAV converter" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
    return 0;
  }
  
  TBufStream ifs;
  ifs.open(argv[1]);
  
  ifs.seek(0x10);
  int sampleCount = ifs.readu32be();
  // ???
  // this seems to be garbage data, sometimes bits of other sounds...?
  int extraAreaStart = headerSize + sampleCount;
  // i don't even know what this is...
  int extraArea2Start = headerSize + ifs.readu32be();
  
  // TEST
//  sampleCount = (ifs.size() - headerSize) / 2;
//  sampleCount = (ifs.size() - extraArea2Start) / 2;
  
  TSoundFile sound;
  sound.setChannels(1);
  sound.setRate(16000);
  
//  TBufStream test;
  
  ifs.seek(headerSize);
//  ifs.seek(extraArea2Start);
  unsigned int decryptionKey = initialDecryptionKey;
  for (int i = 0; i < sampleCount; i++) {
    int nextSample = ifs.reads16be();
    nextSample ^= decryptionKey;
    decryptionKey = (decryptionKey + decryptionKeyIncrement) & 0xFFFF;
    
    sound.addSampleS16(nextSample);
//    test.writes16be(nextSample);
  }
  
//  test.save("test_decrypt.bin");
  
  TBufStream ofs;
  sound.exportWav(ofs);
  ofs.save(argv[2]);
  
  return 0;
}
