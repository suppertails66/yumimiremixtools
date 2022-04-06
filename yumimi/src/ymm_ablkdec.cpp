#include "ymm/YmmAblkDecompiler.h"
#include "ymm/YmmCompress.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yumimi Mix ABLK decompiler" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outprefix>" << endl;
    
    return 0;
  }
  
  TIfstream ifs(argv[1], ios_base::binary);
  
  // skip over header stuff
  
  ifs.seek(4);
  int headerSize = ifs.readu32be();
  ifs.seekoff(headerSize - 4);
  
  ifs.seekoff(0x80);
  
  int value1 = ifs.readu16be();
  int value1Pointer = 0x3CB00 - value1;
  int tilemapCount = ifs.readu16be();
  int block3Count = ifs.readu16be();
  
  {
    TBufStream ofs(0x400000);
    YmmCompress::decmpYmmRleTypes(ifs, ofs);
  }
  
  if (tilemapCount != 0xFFFF) {
    for (int i = 0; i < tilemapCount + 1; i++) {
      TBufStream ofs(0x400000);
      YmmCompress::decmpYmmTilemap(ifs, ofs);
    }
  }
  
  if (block3Count != 0) {
  
//  cerr << hex << ifs.tell() << endl;
    for (int i = 0; i < block3Count; i++) {
      int unk1 = ifs.readu8be();
      int unk2 = ifs.readu8be();
      int unk3 = ifs.readu8be();
      int unk4 = ifs.readu8be();
      
      int unk5 = ifs.readu16be();
      int unk6 = ifs.readu16be();
      
      int unk5shift = unk5 >> 3;
      int unk6shift = unk6 >> 3;
      int entries = (unk5shift * unk6shift) & 0xFFFF;
//      cerr << hex << entries << endl;
      for (int i = 0; i < entries; i++) {
        int entry = ifs.readu16be();
      }
    }
  }
  
  ifs.seekoff(2);
  ifs.seekoff(1);
  
  // decompile
  
  YmmAblkDecompiler(string(argv[2]),
                    YmmEngineVersions::sat)
    .decompile(ifs, cout);
  
  return 0;
} 
