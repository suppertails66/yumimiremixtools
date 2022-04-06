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

using namespace std;
using namespace BlackT;
//using namespace Md;

const static int numColors = 16;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "16-color grayscale to color image converter" << endl;
    cout << "Usage: " << argv[0]
      << " <inpng> <palette> <outpng>"
      << endl;
    
    return 0;
  }
  
  std::string infileName(argv[1]);
  std::string paletteName(argv[2]);
  std::string outfileName(argv[3]);
  
//  MdPaletteLine paletteLine;
  std::vector<TColor> palette;
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
      
      TColor color(r, g, b);
    
//      if (index == 0)
//        result.setA(TColor::fullAlphaTransparency);
//      else
      if ((rawColor & 0x8000) != 0)
        color.setA(128);
      else
        color.setA(TColor::fullAlphaOpacity);
      
      palette.push_back(color);
    }
  }
  
  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(infileName, grp);
  
  for (int j = 0; j < grp.h(); j++) {
    for (int i = 0; i < grp.w(); i++) {
      TColor color = grp.getPixel(i, j);
      if (color.a() == TColor::fullAlphaTransparency) continue;
      
      int value = ((color.r() & 0xF0) >> 4) & 0x0F;
      grp.setPixel(i, j, palette.at(value));
    }
  }
  
  TPngConversion::graphicToRGBAPng(outfileName, grp);

  return 0;
}
