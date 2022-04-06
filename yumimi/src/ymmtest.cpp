#include "ymm/YmmCompress.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Usage: " << argv[0] << " <input> <output>" << endl;
    
    return 0;
  }
  
  TIfstream ifs(argv[1], ios_base::binary);
//  TOfstream ofs(argv[2], ios_base::binary);
  TBufStream bufstream(0x400000);
  
  ifs.seek(4);
  int headerSize = ifs.readu32be();
  ifs.seekoff(headerSize - 4);
  
  ifs.seekoff(0x80);
  
  int value1 = ifs.readu16be();
  int value1Pointer = 0x3CB00 - value1;
  int tilemapCount = ifs.readu16be();
  int block3Count = ifs.readu16be();
  
  cout << "value1Pointer: " << hex << value1Pointer << endl;
  cout << "tilemapCount: " << hex << tilemapCount << endl;
  cout << "block3Count: " << hex << block3Count << endl;
  
//  cout << ifs.tell() << endl;
//  YmmCompress::decmpYmmRleTypes(ifs, bufstream);
  {
    string outname = string(argv[2]) + "_tiles.bin";
    TOfstream ofs(outname.c_str(), ios_base::binary);
    YmmCompress::decmpYmmRleTypes(ifs, ofs);
  }
  
  
  if (tilemapCount != 0xFFFF) {
    for (int i = 0; i < tilemapCount + 1; i++) {
      cout << "tilemap " << i << " start: " << hex << ifs.tell() << endl;
      string outname = string(argv[2]) + "_map"
        + TStringConversion::toString(i) + ".bin";
      TOfstream ofs(outname.c_str(), ios_base::binary);
      YmmCompress::decmpYmmTilemap(ifs, ofs);
      cout << "end: " << hex << ifs.tell() << endl;
    }
  }
  
  if (block3Count != 0) {
    for (int i = 0; i < block3Count; i++) {
      cout << "block3 " << i << " start: " << hex << ifs.tell() << endl;
  
//  char c; cin >> c;
      
      int unk1 = ifs.readu8be();
      int unk2 = ifs.readu8be();
      int unk3 = ifs.readu8be();
      int unk4 = ifs.readu8be();
      
      int unk5 = ifs.readu16be();
      int unk6 = ifs.readu16be();
      
      int unk5shift = unk5 >> 3;
      int unk6shift = unk6 >> 3;
      int entries = unk5shift * unk6shift;
      
      cout << "  unk1: " << unk1 << endl;
      cout << "  unk2: " << unk2 << endl;
      cout << "  unk3: " << unk3 << endl;
      cout << "  unk4: " << unk4 << endl;
      cout << "  unk5: " << unk5 << endl;
      cout << "  unk6: " << unk6 << endl;
      
      for (int i = 0; i < entries; i++) {
        int entry = ifs.readu16be();
        cout << "  entry " << i << ": " << entry << endl;
      }
//      string outname = string(argv[2]) + "_map"
//        + TStringConversion::toString(i) + ".bin";
//      TOfstream ofs(outname.c_str(), ios_base::binary);
      
      
      cout << "end: " << hex << ifs.tell() << endl;
    }
  }
  
  ifs.seekoff(2);
  cout << "endpos: " << ifs.tell() << endl;

  return 0;
}
