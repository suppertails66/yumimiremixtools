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

using namespace std;
using namespace BlackT;
//using namespace Md;

inline int getColComponent(TColor col) {
  return (int)((unsigned char)col.r()) / 16;
}

void exportGrpBitmap(const TGraphic& src, TStream& ofs) {
  if ((src.w() % 8) != 0) {
    throw TGenericException(T_SRCANDLINE,
                            "exportGrpBitmap()",
                            "Graphic width not a multiple of 8");
  }

  int w = src.w();
  int h = src.h();
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i += 4) {
      int output = 0;
      output |= (getColComponent(src.getPixel(i + 0, j)) << 12);
      output |= (getColComponent(src.getPixel(i + 1, j)) << 8);
      output |= (getColComponent(src.getPixel(i + 2, j)) << 4);
      output |= (getColComponent(src.getPixel(i + 3, j)));
      ofs.writeu16be(output);
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Grayscale PNG to Saturn 4bpp graphic converter" << endl;
    cout << "Usage: " << argv[0]
      << " <infile> <outfile>"
      << endl;
    
    return 0;
  }
  
  string infileName(argv[1]);
  string outfileName(argv[2]);
  
  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(infileName, grp);
  
  TBufStream ofs;
  exportGrpBitmap(grp, ofs);
  ofs.save(outfileName.c_str());
  
  return 0;
}
