//#include "md/MdPaletteLine.h"
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
#include <map>

using namespace std;
using namespace BlackT;
//using namespace Md;

const static int numColors = 16;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "16-color color to grayscale image converter" << endl;
    cout << "Usage: " << argv[0]
      << " <inpng> <palette> <outpng> [options]"
      << endl;
    cout << "Options:" << endl;
    cout << "  -t   Enable transparency on index 0" << endl;
    
    return 0;
  }
  
  std::string infileName(argv[1]);
  std::string paletteName(argv[2]);
  std::string outfileName(argv[3]);
  
  bool transparency = TOpt::hasFlag(argc, argv, "-t");
  
//  MdPaletteLine paletteLine;
  std::vector<TColor> palette;
  std::map<TColor, int> realToRawColorMap;
  {
    TBufStream ifs;
    ifs.open(paletteName.c_str());
//    paletteLine.read(ifs);
    double scale = (double)0xFF / (double)((1 << 5) - 1);
    for (int i = 0; i < numColors; i++) {
      int rawColor = ifs.readu16be();
      
      int r = scale * ((rawColor & (0x1F << 0)) >> 0);
      int g = scale * ((rawColor & (0x1F << 5)) >> 5);
      int b = scale * ((rawColor & (0x1F << 10)) >> 10);
      
      // throw away precision the hardware doesn't have
      r >>= 3;
      g >>= 3;
      b >>= 3;
      
      TColor color(r, g, b);
    
      if (transparency && (i == 0))
        color.setA(TColor::fullAlphaTransparency);
      else if ((rawColor & 0x8000) != 0)
        color.setA(128);
      else
        color.setA(TColor::fullAlphaOpacity);
      
//      std::cerr << hex << "i:" << i << " " << r << " " << g << " " << b << endl;
      
      palette.push_back(color);
      if (realToRawColorMap.find(color) == realToRawColorMap.end())
        realToRawColorMap[color] = i;
    }
  }
  
  if (realToRawColorMap.size() != numColors) {
    std::cerr << "WARNING: palette has conflicting colors!" << std::endl;
  }
  
  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(infileName, grp);
  
  double scale = (double)0xFF / (double)((1 << 4) - 1);
  for (int j = 0; j < grp.h(); j++) {
    for (int i = 0; i < grp.w(); i++) {
      TColor color = grp.getPixel(i, j);
      if (color.a() == TColor::fullAlphaTransparency) continue;
      
      int r = (unsigned char)color.r();
      int g = (unsigned char)color.g();
      int b = (unsigned char)color.b();
      r >>= 3;
      g >>= 3;
      b >>= 3;
      color = TColor(r, g, b, color.a());
      
      int index = realToRawColorMap.at(color);
      TColor outColor(index * scale, index * scale, index * scale);
      grp.setPixel(i, j, outColor);
    }
  }
  
  TPngConversion::graphicToRGBAPng(outfileName, grp);

  return 0;
}
