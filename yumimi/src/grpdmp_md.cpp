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

const static int defaultPatternsPerRow = 16;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Mega Drive-format raw graphics dumper" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile> [options]" << endl;
    
    cout << "Options: " << endl;
    cout << "  -s   " << "Set starting offset" << endl;
    cout << "  -p   " << "Set palette line" << endl;
    cout << "  -r   " << "Set patterns per row (default: "
      << defaultPatternsPerRow << ")" << endl;
    
    return 0;
  }
  
  char* infile = argv[1];
  char* outfile = argv[2];
  
  int startOffset = 0;
  TOpt::readNumericOpt(argc, argv, "-s", &startOffset);

  int patternsPerRow = defaultPatternsPerRow;
  TOpt::readNumericOpt(argc, argv, "-r", &patternsPerRow);
  
  MdPaletteLine palLine;
  bool hasPalLine = false;
  char* palOpt = TOpt::getOpt(argc, argv, "-p");
  if (palOpt != NULL) {
    TArray<TByte> rawpal;
    TFileManip::readEntireFile(rawpal, palOpt);
    palLine = MdPaletteLine(rawpal.data());
    hasPalLine = true;
  }
  
  TIfstream ifs(infile, ios_base::binary);
  TArray<TByte> input;
  input.resize(ifs.size());
  ifs.read((char*)input.data(), input.size());
  
  int numPatterns = (input.size() - startOffset)
    / MdPattern::uncompressedSize;
  int outputW = patternsPerRow * MdPattern::w;
  int outputH = (numPatterns / patternsPerRow) * MdPattern::h;
  // deal with edge case
  if ((numPatterns % patternsPerRow) != 0) outputH += MdPattern::h;
  
  TGraphic dst;
  dst.resize(outputW, outputH);
  dst.clearTransparent();
  
  int pos = startOffset;
  int x = 0;
  int y = 0;
  for (int i = 0; i < numPatterns; i++) {
//    int x = (i % patternsPerRow) * MdPattern::w;
//    int y = (i / patternsPerRow) * MdPattern::h;
    
    MdPattern pattern;
    pattern.read((char*)(input.data() + pos));
    
    if (hasPalLine) {
      pattern.toColorGraphic(dst, palLine, x, y);
    }
    else {
      pattern.toGrayscaleGraphic(dst, x, y);
    }
    
    pos += MdPattern::uncompressedSize;
    x += MdPattern::w;
    if (((x / MdPattern::w) % patternsPerRow) == 0) {
      x = 0;
      y += MdPattern::h;
    }
  }
  
  TPngConversion::graphicToRGBAPng(string(outfile), dst);
  
  return 0;
}
