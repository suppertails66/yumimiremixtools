#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/TStringConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TOpt.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TFileManip.h"
#include "md/MdPattern.h"
#include "md/MdPalette.h"
#include "md/MdPaletteLine.h"
#include <iostream>
#include <string>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Mega Drive-format raw graphics undumper" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile> [options]" << endl;
    
    cout << "Options: " << endl;
//    cout << "  -s   " << "Set starting offset" << endl;
    cout << "  -p   " << "Set palette line" << endl;
    
    return 0;
  }
  
  char* infile = argv[1];
  char* outfile = argv[2];
  
//  int startOffset = 0;
//  TOpt::readNumericOpt(argc, argv, "-s", &startOffset);
  
  MdPaletteLine palLine;
  bool hasPalLine = false;
  char* palOpt = TOpt::getOpt(argc, argv, "-p");
  if (palOpt != NULL) {
    TArray<TByte> rawpal;
    TFileManip::readEntireFile(rawpal, palOpt);
    palLine = MdPaletteLine(rawpal.data());
    hasPalLine = true;
  }
  
  TGraphic src;
  TPngConversion::RGBAPngToGraphic(string(infile), src);
  
  int patternCount = src.w() / MdPattern::w;
  int tileH = src.h() / MdPattern::h;
  
  TBufStream ofs;
  int x = 0;
  for (int i = 0; i < patternCount; i++) {
    int y = 0;
    for (int j = 0; j < tileH; j++) {
      MdPattern pattern;
      
      if (hasPalLine) {
        pattern.fromColorGraphic(src, palLine, x, y);
      }
      else {
        pattern.fromGrayscaleGraphic(src, x, y);
      }
      
      pattern.write(ofs);
      y += MdPattern::h;
    }
    
    x += MdPattern::w;
  }
  
  ofs.save(outfile);
  
  return 0;
}
