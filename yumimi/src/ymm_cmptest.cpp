#include "ymm/YmmCompress.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yumimi Mix compression tester" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
    
    return 0;
  }
  
  TIfstream ifs(argv[1], ios_base::binary);
  TBufStream ofs(0x400000);
  TBufStream ofs2(0x400000);
  
  YmmCompress::cmpYmmTilemap(ifs, ofs);
  ofs.seek(0);
  YmmCompress::decmpYmmTilemap(ofs, ofs2);
  
  ofs2.save(argv[2]);
  
  return 0;
}
