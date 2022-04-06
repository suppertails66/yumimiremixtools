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
  if (argc < 5) {
    cout << "Mega Drive-format raw graphics dumper" << endl;
    cout << "Usage: " << argv[0] << " <infile> <height> <patterncount>"
      << " <outfile> [options]" << endl;
    
    cout << "Options: " << endl;
    cout << "  -s   " << "Set starting offset" << endl;
    cout << "  -p   " << "Set palette line" << endl;
    
    return 0;
  }
  
  char* infile = argv[1];
  int tileH = TStringConversion::stringToInt(string(argv[2]));
  int patternCount = TStringConversion::stringToInt(string(argv[3]));
  char* outfile = argv[4];
  
  int startOffset = 0;
  TOpt::readNumericOpt(argc, argv, "-s", &startOffset);
  
  MdPaletteLine palLine;
  bool hasPalLine = false;
  char* palOpt = TOpt::getOpt(argc, argv, "-p");
  if (palOpt != NULL) {
    TArray<TByte> rawpal;
    TFileManip::readEntireFile(rawpal, palOpt);
    palLine = MdPaletteLine(rawpal.data());
    hasPalLine = true;
  }
  
  TBufStream ifs;
  ifs.open(infile);
  ifs.seek(startOffset);
  
  if (patternCount == 0)
   patternCount = (ifs.size() - startOffset) / MdPattern::uncompressedSize;
  
  int outputW = patternCount * MdPattern::w;
  int outputH = tileH * MdPattern::h;
  
  TGraphic dst;
  dst.resize(outputW, outputH);
  dst.clearTransparent();
  
  int x = 0;
  for (int i = 0; i < patternCount; i++) {
    int y = 0;
    for (int j = 0; j < tileH; j++) {
      MdPattern pattern;
      pattern.read(ifs);
      
      if (hasPalLine) {
        pattern.toColorGraphic(dst, palLine, x, y);
      }
      else {
        pattern.toGrayscaleGraphic(dst, x, y);
      }
      
      y += MdPattern::h;
    }
    
    x += MdPattern::w;
  }
  
  TPngConversion::graphicToRGBAPng(string(outfile), dst);
  
  return 0;
}
