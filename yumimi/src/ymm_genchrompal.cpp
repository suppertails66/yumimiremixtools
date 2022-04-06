//#include "psx/PsxPalette.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TIniFile.h"
#include "util/TBufStream.h"
#include "util/TOfstream.h"
#include "util/TIfstream.h"
#include "util/TStringConversion.h"
#include "util/TBitmapFont.h"
#include "util/TOpt.h"
#include "util/MiscMath.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace BlackT;
//using namespace Psx;

const int numColors = 16;
const int previewBlockW = 16;
const int previewBlockH = 16;

int main(int argc, char* argv[]) {
  if (argc < 9) {
    cout << "4-bit chromatic palette generator" << endl;
    cout << "Usage: " << argv[0]
      << " <minR> <minG> <minB> <maxR> <maxG> <maxB> <outpal> <outimg>"
      << endl;
    cout << "Options:" << endl;
    cout << "   --trans     Force slot 0 to transparent" << endl;
    
    return 0;
  }
  
  int minR = TStringConversion::stringToInt(std::string(argv[1]));
  int minG = TStringConversion::stringToInt(std::string(argv[2]));
  int minB = TStringConversion::stringToInt(std::string(argv[3]));
  int maxR = TStringConversion::stringToInt(std::string(argv[4]));
  int maxG = TStringConversion::stringToInt(std::string(argv[5]));
  int maxB = TStringConversion::stringToInt(std::string(argv[6]));
  std::string outPalName(argv[7]);
  std::string outImgName(argv[8]);
  
//  TOpt::readNumericOpt(argc, argv, "-paloffset", &palOffset);

  bool transparency = false;
  if (TOpt::hasFlag(argc, argv, "--trans")) {
    transparency = true;
  }
  
  int trueNumColors = numColors;
  if (transparency) --trueNumColors;
  
  int rRange = maxR - minR;
  int gRange = maxG - minG;
  int bRange = maxB - minB;
  double rFactor = (double)rRange / (double)(trueNumColors - 1);
  double gFactor = (double)gRange / (double)(trueNumColors - 1);
  double bFactor = (double)bRange / (double)(trueNumColors - 1);
  
  TBufStream ofs;
  
//  PsxPalette pal;
  TGraphic grp(previewBlockW * numColors, previewBlockH);
  grp.clearTransparent();
  for (int i = 0; i < numColors; i++) {
    int factor = i;
    if (transparency && (i != 0)) --factor;
  
    int r = minR + (rFactor * factor);
    int g = minG + (gFactor * factor);
    int b = minB + (bFactor * factor);
    
    for (int j = 0; j <= argc - 5; j++) {
      if (std::string(argv[j]).compare("--force") == 0) {
        int index = TStringConversion::stringToInt(std::string(argv[j + 1]));
        if (index != i) continue;
        
        r = TStringConversion::stringToInt(std::string(argv[j + 2]));
        g = TStringConversion::stringToInt(std::string(argv[j + 3]));
        b = TStringConversion::stringToInt(std::string(argv[j + 4]));
      }
    }
    
    MiscMath::clamp(r, 0, 255);
    MiscMath::clamp(g, 0, 255);
    MiscMath::clamp(b, 0, 255);
    
    int trueR = (r >> 3);
    int trueG = (g >> 3);
    int trueB = (b >> 3);
    
    int color = 0;
    color |= (trueB << 10);
    color |= (trueG << 5);
    color |= (trueR);
    
    if (transparency && (i == 0)) color = 0;
    
    if (!(transparency && (i == 0))) {
      grp.fillRect(i * previewBlockW, 0, previewBlockW, previewBlockH,
                   TColor(r, g, b));
    }
    
    ofs.writeu16be(color);
    
/*    TColor color(r, g, b);
    PsxColor psxColor;
    psxColor.fromTColor(color);
    
    if (transparency && (i == 0)) psxColor.fromNativeColor(0);
    
    pal.setColor(i, psxColor);
    
    if (!(transparency && (i == 0))) {
      grp.fillRect(i * previewBlockW, 0, previewBlockW, previewBlockH,
                   psxColor.asTColor());
    } */
  }
  
//  pal.write(ofs, numColors);
  ofs.save(outPalName.c_str());
  
  TPngConversion::graphicToRGBAPng(outImgName, grp);
  
  return 0;
}
