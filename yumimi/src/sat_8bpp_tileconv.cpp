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
  
  void setRealColor(int index, BlackT::TColor realColor) {
    colors.at(index) = realColorToRaw(realColor);
  }
  
  BlackT::TColor getRealColor(int index) const {
    int rawColor = colors.at(index);
    TColor result = rawColorToReal(rawColor);
    
    // index 0 = fully transparent
    if (index == 0)
      result.setA(TColor::fullAlphaTransparency);
    
    return result;
  }
  
  int findIndexOfRealColor(TColor realColor) const {
    // fully transparent = index 0
    if (realColor.a() == TColor::fullAlphaTransparency) return 0;
  
    int rawColor = realColorToRaw(realColor);
    for (int i = 0; i < colors.size(); i++) {
      if (colors[i] == rawColor) return i;
    }
    
    // failure
    return -1;
  }
  
  static int realColorToRaw(BlackT::TColor color) {
    int result = 0;
    result |= ((color.r() >> 3) << 0);
    result |= ((color.g() >> 3) << 5);
    result |= ((color.b() >> 3) << 10);
    
    if (color.a() != TColor::fullAlphaOpacity) result |= 0x8000;
    
    return result;
  }
  
  static BlackT::TColor rawColorToReal(int rawColor) {
    TColor result;
    
    double scale = (double)0xFF / (double)((1 << 5) - 1);
    
    result.setR(scale * ((rawColor & (0x1F << 0)) >> 0));
    result.setG(scale * ((rawColor & (0x1F << 5)) >> 5));
    result.setB(scale * ((rawColor & (0x1F << 10)) >> 10));
    
    if ((rawColor & 0x8000) != 0)
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
  
  bool operator==(const SatPattern& other) const {
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        if (this->data.data(i, j) != other.data.data(i, j)) return false;
      }
    }
    return true;
  }
  
  bool operator!=(const SatPattern& other) const {
    return !(*this == other);
  }
  
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
  
  void fromGraphic(const TGraphic& src,
                 int baseX, int baseY,
                 SatPalette* palette = NULL) {
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        int x = baseX + i;
        int y = baseY + j;
        
        TColor color = src.getPixel(x, y);
        
        // default = grayscale
        int value = color.r();
        if (palette == NULL) {
          
        }
        else {
          value = palette->findIndexOfRealColor(color);
        }
        
        if (value == -1) {
          throw TGenericException(T_SRCANDLINE,
             "SatPattern::fromGraphic()",
             std::string("Failed color conversion at (")
             + TStringConversion::intToString(x)
             + ","
             + TStringConversion::intToString(y)
             + ")");
        }
        
        data.data(i, j) = value;
      }
    }
  }
  
protected:
  TTwoDArray<TByte> data;
};

typedef std::vector<SatPattern> SatPatternCollection;

class SatTileSet {
public:
  SatTileSet() { }
  
  int addPattern(const SatPattern& pattern) {
    patterns.push_back(pattern);
    return patterns.size() - 1;
  }
  
  int numPatterns() const {
    return patterns.size();
  }
  
  int findPatternIndex(const SatPattern& pattern) const {
    for (int i = 0; i < patterns.size(); i++) {
      if (patterns[i] == pattern) return i;
    }
    // failure
    return -1;
  }
  
  SatPattern& getPattern(int index) {
    return patterns.at(index);
  }
  
  const SatPattern& getPattern(int index) const {
    return patterns.at(index);
  }
protected:
  SatPatternCollection patterns;
};

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "8bpp image to Saturn tiles+tilemap converter" << endl;
    cout << "Usage: " << argv[0]
      << " <infile> <outtiles> <outtilemap>"
      << endl;
    cout << "Options: " << endl
      << "   -p   Set palette file (default: grayscale)"
      << endl;
    
    return 0;
  }
  
  std::string infileName(argv[1]);
  std::string outTilesName(argv[2]);
  std::string outTilemapName(argv[3]);
  
  try {
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
    
    TGraphic grp;
    TPngConversion::RGBAPngToGraphic(infileName, grp);
    
    int tileW = grp.w() / SatPattern::w;
    int tileH = grp.h() / SatPattern::h;
    
    TBufStream tilemapOfs;
    SatTileSet tileSet;
    for (int j = 0; j < tileH; j++) {
      for (int i = 0; i < tileW; i++) {
        int x = (i * SatPattern::w);
        int y = (j * SatPattern::h);
        SatPattern pattern;
        pattern.fromGraphic(grp, x, y, palette);
        
        int patternIndex = tileSet.findPatternIndex(pattern);
        if (patternIndex == -1) {
          patternIndex = tileSet.addPattern(pattern);
        }
        
        tilemapOfs.writeu16be(patternIndex);
      }
    }
    
    tilemapOfs.save(outTilemapName.c_str());
    
    TBufStream tileOfs;
    for (int i = 0; i < tileSet.numPatterns(); i++) {
      const SatPattern& pattern = tileSet.getPattern(i);
      pattern.write(tileOfs);
    }
    tileOfs.save(outTilesName.c_str());
  }
  catch (TGenericException& e) {
    std::cerr << "Exception: " << e.problem() << std::endl;
    return 1;
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  catch (...) {
    std::cerr << "error" << std::endl;
    return 1;
  }
  
/*  SatPatternCollection patterns;
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
  
  TPngConversion::graphicToRGBAPng(outfileName, grp);*/

  return 0;
}
