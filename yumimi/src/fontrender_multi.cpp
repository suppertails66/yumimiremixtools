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
#include "util/TBitmapFont.h"
#include "util/TThingyTable.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace BlackT;
//using namespace Md;

enum AlignMode {
  alignMode_left,
  alignMode_center,
  alignMode_right
};

enum OutlineMode {
  outlineMode_none,
  outlineMode_square,
  outlineMode_cross
};

void setPixelIfExists(TGraphic& grp, int x, int y, TColor color) {
  if ((x >= grp.w()) || (x < 0) || (y >= grp.h()) || y < 0) return;
  grp.setPixel(x, y, color);
}

void outlinePixelCross(TGraphic& grp, int i, int j,
                       TColor emptyColor,
                       TColor color,
                       int radius) {
  bool fullTrans = false;
  if (emptyColor.a() == TColor::fullAlphaTransparency) fullTrans = true;
  for (int b = -radius; b <= radius; b++) {
    for (int a = -radius; a <= radius; a++) {
      if ((std::abs(a) == radius) && (std::abs(b) == radius)) continue;
      if ((a == 0) && (b == 0)) continue;
      
      if (fullTrans) {
        if (grp.getPixel(i + a, j + b).a() == TColor::fullAlphaTransparency) {
          setPixelIfExists(grp, i + a, j + b, color);
        }
      }
      else {
        if (grp.getPixel(i + a, j + b) == emptyColor) {
          setPixelIfExists(grp, i + a, j + b, color);
        }
      }
    }
  }
}

void outlinePixelSquare(TGraphic& grp, int i, int j,
                       TColor emptyColor,
                       TColor color,
                       int radius) {
  bool fullTrans = false;
  if (emptyColor.a() == TColor::fullAlphaTransparency) fullTrans = true;
  for (int b = -radius; b <= radius; b++) {
    for (int a = -radius; a <= radius; a++) {
      if ((a == 0) && (b == 0)) continue;
      
      if (fullTrans) {
        if (grp.getPixel(i + a, j + b).a() == TColor::fullAlphaTransparency) {
          setPixelIfExists(grp, i + a, j + b, color);
        }
      }
      else {
        if (grp.getPixel(i + a, j + b) == emptyColor) {
          setPixelIfExists(grp, i + a, j + b, color);
        }
      }
    }
  }
}

void outlineGrpSquare(TGraphic& grp, TColor solidColor, TColor outlineColor) {
  for (int j = 0; j < grp.h(); j++) {
    for (int i = 0; i < grp.w(); i++) {
      TColor color = grp.getPixel(i, j);
      if (color == solidColor) {
        outlinePixelSquare(grp, i, j,
                          TColor(255, 255, 255, TColor::fullAlphaTransparency),
                          outlineColor,
                          1);
      }
      else {
//        cout << " ";
      }
    }
  }
}

void outlineGrpCross(TGraphic& grp, TColor solidColor, TColor outlineColor) {
  for (int j = 0; j < grp.h(); j++) {
    for (int i = 0; i < grp.w(); i++) {
      TColor color = grp.getPixel(i, j);
      if (color == solidColor) {
        outlinePixelSquare(grp, i, j,
                          TColor(255, 255, 255, TColor::fullAlphaTransparency),
                          outlineColor,
                          1);
      }
      else {
//        cout << " ";
      }
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Font renderer" << endl;
    cout << "Usage: " << argv[0]
      << " <fontprefix> <stringfile> <table> <outfile>"
      << endl;
    cout << "Options:" << endl;
    cout << "  --left     Left-align output (default)" << endl;
    cout << "  --center   Center output" << endl;
    cout << "  --left     Right-align output" << endl;
    cout << "  --lineh    Override font's line spacing" << endl;
    cout << "  --recolor  <r> <g> <b>" << endl;
    cout << "             Changes non-transparent pixels of font to specified color" << endl;
    cout << "  --outline  <r> <g> <b>" << endl;
    cout << "             Outlines output with specified color" << endl;
    cout << "  --xoff     <x>" << endl;
    cout << "             Shifts output's x-position by X pixels" << endl;
    cout << "  --yoff     <y>" << endl;
    cout << "             Shifts output's y-position by Y pixels" << endl;
    cout << "  --KERNHACK Activates dumb game-specific kerning hacks" << endl;
    
    return 0;
  }
  
  char* fontPrefix = argv[1];
  char* stringfile = argv[2];
  char* tablefile = argv[3];
  char* outfile = argv[4];
  
  AlignMode alignMode = alignMode_left;
  if (TOpt::hasFlag(argc, argv, "--left")) {
    alignMode = alignMode_left;
  }
  if (TOpt::hasFlag(argc, argv, "--center")) {
    alignMode = alignMode_center;
  }
  if (TOpt::hasFlag(argc, argv, "--right")) {
    alignMode = alignMode_right;
  }
  
  OutlineMode outlineMode = outlineMode_none;
  TColor outlineColor(0, 0, 0);
  int outlineRadius = 0;
  {
    int outlineIndex = TOpt::findOpt(argc, argv, "--outline");
    if (outlineIndex != -1) {
      outlineMode = outlineMode_square;
      outlineRadius = 1;
      
      int r = TStringConversion::stringToInt(
        std::string(argv[outlineIndex + 1]));
      int g = TStringConversion::stringToInt(
        std::string(argv[outlineIndex + 2]));
      int b = TStringConversion::stringToInt(
        std::string(argv[outlineIndex + 3]));
      
      // HACK
      if ((r == -1) || (g == -1) || (b == -1)) {
        outlineMode = outlineMode_none;
      }
      else {
        outlineColor.setR(r);
        outlineColor.setG(g);
        outlineColor.setB(b);
      }
    }
  }
  
  int extraXOffset = 0;
  TOpt::readNumericOpt(argc, argv, "--xoff", &extraXOffset);
  int extraYOffset = 0;
  TOpt::readNumericOpt(argc, argv, "--yoff", &extraYOffset);
  
  
  bool recolorOn = false;
  TColor recolorColor(0xFF, 0xFF, 0xFF);
  {
    int recolorIndex = TOpt::findOpt(argc, argv, "--recolor");
    if (recolorIndex != -1) {
      recolorOn = true;
      
      recolorColor.setR(TStringConversion::stringToInt(
        std::string(argv[recolorIndex + 1])));
      recolorColor.setG(TStringConversion::stringToInt(
        std::string(argv[recolorIndex + 2])));
      recolorColor.setB(TStringConversion::stringToInt(
        std::string(argv[recolorIndex + 3])));
    }
  }
  
  
  TBitmapFont font;
  font.load(std::string(fontPrefix));
  
  // HACK
  // for large font in yumimi credits sequence;
  // only a few specific letter pairs need to be modified
  if (TOpt::hasFlag(argc, argv, "--KERNHACK")) {
    // "Ta"
    font.offsetKerning(0x1E, 0x25, 1);
    // "Te"
    font.offsetKerning(0x1E, 0x29, 1);
    // "To"
    font.offsetKerning(0x1E, 0x33, 1);
    // "Yo"
    font.offsetKerning(0x23, 0x33, 1);
  }
  
  TThingyTable table;
  table.readSjis(tablefile);
  
  TBufStream filterIfs;
  filterIfs.open(stringfile);
  
  std::vector<TGraphic> lineGrps;
  
  int lineNum = 0;
  int longestLineW = 0;
  int lineH = 0;
  while (!filterIfs.eof()) {
    TBufStream ifs;
    {
      while (!filterIfs.eof()) {
        if (filterIfs.peek() == '\r') {
          filterIfs.get();
          continue;
        }
        
        if (filterIfs.peek() == '\n') {
          filterIfs.get();
          break;
        }
        
        ifs.put(filterIfs.get());
      }
    }
    
    ifs.seek(0);
    
    TGraphic grp;
    font.render(grp, ifs, table);
    lineGrps.push_back(grp);
    
    if (grp.w() > longestLineW) longestLineW = grp.w();
    lineH = grp.h();
    ++lineNum;
  }
  
  int lineHOverride = -1;
  TOpt::readNumericOpt(argc, argv, "--lineh", &lineHOverride);
  if (lineHOverride != -1) lineH = lineHOverride;
  
//  TGraphic outputGrp(longestLineW + (outlineRadius * 2),
//                     (lineGrps.size() * lineH) + (outlineRadius * 2));
  TGraphic outputGrp;
  if (alignMode == alignMode_right) {
    outputGrp = TGraphic(longestLineW + (outlineRadius * 2),
                     (lineGrps.size() * lineH));
  }
  else {
    outputGrp = TGraphic(longestLineW + (outlineRadius * 1),
                     (lineGrps.size() * lineH));
  }
  outputGrp.clearTransparent();
  int baseX = outlineRadius + extraXOffset;
  int baseY = outlineRadius + extraYOffset;
  for (int i = 0; i < lineGrps.size(); i++) {
    const TGraphic& src = lineGrps[i];
    int x = baseX + 0;
    int y = baseY + (i * lineH);
    
    switch (alignMode) {
    case alignMode_left:
      break;
    case alignMode_center:
      x = baseX + ((outputGrp.w() - src.w()) / 2);
      break;
    case alignMode_right:
      x = baseX + (outputGrp.w() - src.w()) - outlineRadius;
      break;
    default:
      break;
    }
    
    outputGrp.copy(src,
                   TRect(x, y, 0, 0));
  }
  
  if (recolorOn) {
    for (int j = 0; j < outputGrp.h(); j++) {
      for (int i = 0; i < outputGrp.w(); i++) {
        int x = i;
        int y = j;
        if (outputGrp.getPixel(x, y).a() != TColor::fullAlphaTransparency) {
          outputGrp.setPixel(x, y, recolorColor);
        }
      }
    }
  }
  
  if (outlineMode == outlineMode_square) {
    outlineGrpSquare(outputGrp, recolorColor, outlineColor);
  }
  else if (outlineMode == outlineMode_cross) {
    outlineGrpCross(outputGrp, recolorColor, outlineColor);
  }
  
  TPngConversion::graphicToRGBAPng(std::string(outfile), outputGrp);
  
  return 0;
}
