#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TSoundFile.h"
#include <iostream>

using namespace std;
using namespace BlackT;

const static int dataBaseOffset = 0x800;
const static int dataBlockSize = 0x8000;
const static int dataBlockSeparation = 0x25800;
const static int dataSubBlockSize = 0x800;
//const static int numSubBlocksPerBlock = dataBlockSize / dataSubBlockSize;

int fromMcdPcm(int rawSamp) {
  unsigned char next = rawSamp;
  if ((next & 0x80) != 0) {
    char mag = next & 0x7F;
    return -mag;
  }
  else {
    return rawSamp;
  }
}

void readSubBlock(TStream& ifs, TStream& ofs) {
  for (int i = 0; i < dataSubBlockSize; i++) {
    int in = ifs.readu8();
    int out = fromMcdPcm(in);
//  cout << hex << in << " " << out << endl;
//  char c;
//  cin >> c;
    ofs.writes8(out);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yumimi Mix MKP audio file to WAV converter" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
    return 0;
  }
  
  TBufStream ifs;
  ifs.open(argv[1]);
  
  int numBlocks = (ifs.size() - dataBaseOffset) / dataBlockSeparation;
  
  TSoundFile sound;
  sound.setChannels(2);
  sound.setRate(16000);
  
  for (int i = 0; i < numBlocks; i++) {
    ifs.seek(dataBaseOffset + (i * dataBlockSeparation));
    
//    std::cout << std::hex << ifs.tell() << std::endl;
    
    int remaining = dataBlockSize;
    while (remaining > 0) {
      TBufStream ofsA;
      TBufStream ofsB;
      
      readSubBlock(ifs, ofsA);
      readSubBlock(ifs, ofsB);
      remaining -= (dataSubBlockSize * 2);
      
      ofsA.seek(0);
      ofsB.seek(0);
      for (int i = 0; i < dataSubBlockSize; i++) {
//        cout << hex << (int)((unsigned char)ofsA.peek()) << " ";
//        cout << hex << (int)((unsigned char)ofsB.peek()) << " " << endl;
//        char c;
//        cin >> c;
      
        sound.addSampleS8(ofsA.reads8());
        sound.addSampleS8(ofsB.reads8());
      }
    }
  }
  
  TBufStream ofs;
  sound.exportWav(ofs);
  ofs.save(argv[2]);
  
  return 0;
}
