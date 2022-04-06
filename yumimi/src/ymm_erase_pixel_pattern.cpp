#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TSort.h"
#include "util/TGraphic.h"
#include "util/TColor.h"
#include "util/TPngConversion.h"
#include "util/TBitmapFont.h"
#include "util/TThingyTable.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include "ymm/YmmCompress.h"
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Diagonal pixel pattern eraser" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
    
    return 0;
  }
  
  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(string(argv[1]), grp);
  
  for (int j = 0; j < grp.h(); j++) {
    bool yIsEven = ((j % 2) == 0);
    for (int i = (yIsEven ? 0 : 1); i < grp.w(); i += 2) {
      grp.setPixel(i, j, TColor(0xB4, 0xFF, 0xFF));
    }
  }
  
  TPngConversion::graphicToRGBAPng(string(argv[2]), grp);
  
  return 0;
}
