#include "ymm/YmmScene.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include <string>
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yumimi Mix scene ripper" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outprefix> [engine]" << endl;
    cout << "Engine may be 'mcd' or 'sat' (default: mcd)" << endl;
    
    return 0;
  }
  
  char* infile = argv[1];
  char* outprefix = argv[2];
  
  YmmEngineVersions::YmmEngineVersion engineVersion
    = YmmEngineVersions::mcd;
  if (argc >= 4) {
    std::string versionStr(argv[3]);
    if (versionStr.compare("sat") == 0)
      engineVersion = YmmEngineVersions::sat;
    else if (versionStr.compare("mcd") == 0)
      engineVersion = YmmEngineVersions::mcd;
  }
  
//  TIfstream ifs(infile, ios_base::binary);
  TBufStream ifs;
  ifs.open(infile);
  
  YmmScene scene;
  scene.read(ifs, engineVersion);
  
  // save
  scene.save(outprefix);
  
  // save and write
/*  scene.save(outprefix);
  TBufStream test(0x400000);
  scene.write(test);
  test.save((string(outprefix) + "build_test.bin").c_str()); */
  
  // save, write, read, then save and write again
/*  scene.save(outprefix);
//  scene.load(outprefix);
//  scene.save(outprefix);
  TBufStream test(0x400000);
  scene.write(test);
  test.save((string(outprefix) + "build_test_1.bin").c_str()); 
  test.seek(0);
  YmmScene scene2;
  scene2.read(test);
  scene2.save(outprefix);
  test.seek(0);
  scene2.write(test);
  test.save((string(outprefix) + "build_test_2.bin").c_str()); */
  
  return 0;
}
