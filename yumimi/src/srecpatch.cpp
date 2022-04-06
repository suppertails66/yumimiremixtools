#include "util/TStringConversion.h"
#include "util/TIfstream.h"
#include "util/TBufStream.h"
#include "util/TOpt.h"
#include <string>
#include <iostream>

using namespace std;
using namespace BlackT;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "SREC binary converter" << endl;
    cout << "Usage: " << argv[0] << " <srcfile> <dstfile> [options]" << endl;
    cout << "Options:" << endl;
    cout << "  -o    Set load offset of data (subtracted from address)" << endl;
    cout << "Input SREC is read from standard input." << endl;
    
    return 0;
  }
  
  char* srcfile = argv[1];
  char* dstfile = argv[2];
//  int dstoffset = TStringConversion::stringToInt(string(argv[3]));
  
  TBufStream ofs;
  TIfstream srcifs(srcfile, ios_base::binary);
  ofs.writeFrom(srcifs, srcifs.size());
  srcifs.close();
  
  int loadOffset = 0;
  TOpt::readNumericOpt(argc, argv, "-o", &loadOffset);
  
  int ofssize = ofs.tell();
//  ofs.seek(dstoffset);
  
  istream& ifs = cin;
  
  string next;
  while (ifs.good()) {
    
    ifs >> next;
    
    ifs.get();
    if (!ifs.good()) break;
    ifs.unget();
    
//    cout << next << endl;
    
    int recordType = TStringConversion::stringToInt(string() + next[1]);
    int size = TStringConversion::stringToInt("0x" + next.substr(2, 2));
    bool hasData = false;
    int address, dataOffset, dataSize;
    
//    cout << recordType << endl;
  
    switch (recordType) {
    // header
    case 0:
      
      break;
    // 16-bit
    case 1:
      address = TStringConversion::stringToInt("0x" + next.substr(4, 4));
      dataOffset = 8;
      dataSize = size - 3;
      hasData = true;
      break;
    // 24-bit
    case 2:
      address = TStringConversion::stringToInt("0x" + next.substr(4, 6));
      dataOffset = 10;
      dataSize = size - 4;
      hasData = true;
      break;
    // 32-bit
    case 3:
      address = TStringConversion::stringToInt("0x" + next.substr(4, 8));
      dataOffset = 12;
      dataSize = size - 5;
      hasData = true;
      break;
    // footer
    case 9:
      
      break;
    default:
      
      break;
    }
    
    if (!hasData) continue;
    
    ofs.seek(address - loadOffset);
    
    string data = next.substr(dataOffset, dataSize * 2);
    
//    cout << data.size() << endl;

//    cout << data << endl;

//    cout << hex << address << " " << dataSize << " " << data << endl;
    
    for (int i = 0; i < dataSize; i++) {
      string digitstr = "0x" + data.substr(i * 2, 2);
      int value = TStringConversion::stringToInt(digitstr);
      ofs.writeu8be(value);
    }
  }
  
  ofs.seek(ofssize);
  ofs.save(dstfile);
  
  return 0;
}
