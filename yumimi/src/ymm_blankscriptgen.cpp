#include "ymm/YmmScene.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include <string>
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Yumimi Mix blank scene script generator" << endl;
    cout << "Usage: " << argv[0] << " <infile>" << endl;
    
    return 0;
  }
  
  char* infile = argv[1];
  
//  TIfstream ifs(infile, ios_base::binary);
  TBufStream ifs;
  ifs.open(infile);
  
  YmmScene scene;
  scene.read(ifs);
  
  std::string basename = TFileManip::getFilename(std::string(infile));
  
  std::cout << "//============================================" << endl;
//  std::cout << "// " << basename << endl;
  std::cout << "#scene \"" << basename << "\"" << endl;
  std::cout << "//============================================" << endl;
  cout << endl;
  
  for (int i = 0; i < scene.numSubscenes(); i++) {
    YmmSubscene& subscene = scene.subscene(i);
    if (i != 0) {
      cout << "//==============" << endl;
      cout << "#subscene " << i << endl;
      cout << "//==============" << endl;
      cout << endl;
    }
    
    for (int j = 0; j < subscene.pcmData_.size(); j++) {
      cout << "#sub " << j << endl;
      cout << endl;
      cout << endl;
    }
  }
  
  return 0;
}
