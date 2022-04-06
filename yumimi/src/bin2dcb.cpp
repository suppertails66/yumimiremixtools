#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TByte.h"
#include <iostream>

using namespace std;
using namespace BlackT;
//using namespace Md;

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}

int constsPerLine = 16;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Binary file to dc.b assembly directive converter"
      << std::endl;
    std::cout << "Usage: " << argv[0] << " [infile]" << std::endl;
    
    return 0;
  }
  
  TBufStream ifs;
  ifs.open(argv[1]);
  
  while (true) {
    if (ifs.eof()) break;
    
    std::cout << "  dc.b ";
    
    for (int i = 0; i < constsPerLine; i++) {
      if (ifs.eof()) break;
      
      TByte next = ifs.get();
      std::cout << as2bHexPrefix(next);
      if (!ifs.eof() && (i != constsPerLine - 1)) std::cout << ",";
    }
    
    std::cout << std::endl;
  }
  
  return 0;
}
