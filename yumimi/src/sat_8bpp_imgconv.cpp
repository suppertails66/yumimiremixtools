//#include "ymm/YmmCompress.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TByte.h"
#include "util/TTwoDArray.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TOpt.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace BlackT;
//using namespace Md;

class SatPalette {
public:
  const static int maxColors = 256;
  
  SatPalette() {
    colors.resize(maxColors);
  }
  
  void setRawColor(int index, int rawColor) {
    colors.at(index) = rawColor;
  }
  
  TColor getRealColor(int index) const {
    int rawColor = colors.at(index);
    TColor result;
    
    double scale = (double)0xFF / (double)((1 << 5) - 1);
    
    // TODO: color modes...
    result.setR(scale * ((rawColor & (0x1F << 0)) >> 0));
    result.setG(scale * ((rawColor & (0x1F << 5)) >> 5));
    result.setB(scale * ((rawColor & (0x1F << 10)) >> 10));
    
    if (index == 0)
      result.setA(TColor::fullAlphaTransparency);
    else if ((rawColor & 0x8000) != 0)
      result.setA(128);
    else
      result.setA(TColor::fullAlphaOpacity);
    
    return result;
  }
  
protected:
  std::vector<int> colors;
  
};

class SatPattern {
public:
  const static int w = 8;
  const static int h = 8;

  SatPattern()
    : data(w, h) { }
  
  void read(TStream& ifs) {
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        data.data(i, j) = ifs.get();
      }
    }
  }
  
  void write(TStream& ofs) const {
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        ofs.put(data.data(i, j));
      }
    }
  }
  
  void toGraphic(TGraphic& dst,
                 int baseX, int baseY,
                 SatPalette* palette = NULL) const {
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        TByte value = data.data(i, j);
        int x = baseX + i;
        int y = baseY + j;
        
        TColor color(value, value, value);
        if (palette == NULL) {
          
        }
        else {
          color = palette->getRealColor(value);
        }
        
        dst.setPixel(x, y, color);
      }
    }
  }
  
protected:
  TTwoDArray<TByte> data;
};

typedef std::vector<SatPattern> SatPatternCollection;

int main(int argc, char* argv[]) {
  if (argc < 6) {
    cout << "8bpp Saturn tiles+tilemap to image converter" << endl;
    cout << "Usage: " << argv[0]
      << " <tiles> <tilemap> <tilew> <tileh> <outfile>"
      << endl;
    cout << "Options: " << endl
      << "   -p   Set palette file (default: grayscale)"
      << endl;
    
    return 0;
  }
  
  std::string tilesName(argv[1]);
  std::string tilemapName(argv[2]);
  int outputTileW = TStringConversion::stringToInt(std::string(argv[3]));
  int outputTileH = TStringConversion::stringToInt(std::string(argv[4]));
  std::string outfileName(argv[5]);
  
  SatPalette paletteValue;
  char* paletteName = TOpt::getOpt(argc, argv, "-p");
  SatPalette* palette = NULL;
  if (paletteName != NULL) {
    palette = &paletteValue;
    
    TBufStream ifs;
    ifs.open(paletteName);
    for (int i = 0; i < SatPalette::maxColors; i++) {
      int rawColor = ifs.readu16be();
      palette->setRawColor(i, rawColor);
    }
  }
  
  SatPatternCollection patterns;
  {
    TBufStream ifs;
    ifs.open(tilesName.c_str());
    while (!ifs.eof()) {
      SatPattern pattern;
      pattern.read(ifs);
      patterns.push_back(pattern);
    }
  }
  
  TGraphic grp(outputTileW * SatPattern::w,
               outputTileH * SatPattern::h);
  
  {
    TBufStream ifs;
    ifs.open(tilemapName.c_str());
    for (int j = 0; j < outputTileH; j++) {
      for (int i = 0; i < outputTileW; i++) {
        int x = (i * SatPattern::w);
        int y = (j * SatPattern::h);
        
        int tileNum = ifs.readu16be();
        const SatPattern& pattern = patterns.at(tileNum);
        pattern.toGraphic(grp, x, y, palette);
      }
    }
  }
  
  TPngConversion::graphicToRGBAPng(outfileName, grp);

  return 0;
}
