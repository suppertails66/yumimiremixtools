#include "ymm/YmmScene.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "exception/TGenericException.h"
#include <string>
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yumimi Mix scene builder" << endl;
    cout << "Usage: " << argv[0] << " <inprefix> <outfile>" << endl;
    
    return 0;
  }
  
  try {
  
    char* inprefix = argv[1];
    char* outfile = argv[2];
    
    YmmScene scene;
    scene.load(inprefix);
    
  //  TOfstream ofs(outfile, ios_base::binary);
  //  scene.write(ofs);
    TBufStream ofs;
    scene.write(ofs);
    ofs.save(outfile);
  
  }
  catch (TGenericException& e) {
    std::cerr << "Exception: " << e.problem() << std::endl;
    return 1;
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
