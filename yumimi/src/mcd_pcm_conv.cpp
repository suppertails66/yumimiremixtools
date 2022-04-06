#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include <iostream>

using namespace std;
using namespace BlackT;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "8-bit sign-and-magnitude to two's complement converter" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
    return 0;
  }
  
  TBufStream ifs;
  ifs.open(argv[1]);
  
  TBufStream ofs;
  
  while (!ifs.eof()) {
    unsigned char next = ifs.get();
    if ((next & 0x80) != 0) {
      char mag = next & 0x7F;
      int val = -mag;
      next = (unsigned char)val;
    }
    else {
      
    }
    
    ofs.put(next);
  }
  
  ofs.save(argv[2]);
  
  return 0;
}
