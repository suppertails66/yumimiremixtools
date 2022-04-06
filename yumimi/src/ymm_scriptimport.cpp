#include "ymm/YmmTranslationSheet.h"
//#include "psx/PsxPalette.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TFileManip.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TCsv.h"
#include "util/TParse.h"
#include "util/TThingyTable.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;
using namespace BlackT;
using namespace Md;

YmmTranslationSheet scriptSheet;
//TThingyTable tableStd;

std::string getNumStr(int num) {
  std::string str = TStringConversion::intToString(num);
  while (str.size() < 2) str = string("0") + str;
  return str;
}

std::string getHexWordNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 4) str = string("0") + str;
  return string("$") + str;
}

void importScript(string basename, bool substitutionsOn = false) {
  TBufStream ifs;
  ifs.open((string("script/") + basename).c_str());
  
  TBufStream ofs;
  while (!ifs.eof()) {
    std::string line;
    ifs.getLine(line);
    
    TBufStream lineIfs;
    lineIfs.writeString(line);
    lineIfs.seek(0);
    
    bool success = false;
    
    TParse::skipSpace(lineIfs);
    if (TParse::checkChar(lineIfs, '#')) {
      TParse::matchChar(lineIfs, '#');
      
      std::string name = TParse::matchName(lineIfs);
      TParse::matchChar(lineIfs, '(');
      
      for (unsigned int i = 0; i < name.size(); i++) {
        name[i] = toupper(name[i]);
      }
      
      bool isDirectImport = (name.compare("IMPORT") == 0);
      if (isDirectImport
          || (name.compare("IMPORTIFEXISTS") == 0)) {
        std::string id = TParse::matchString(lineIfs);
//        cerr << id << endl;
        
        if (!isDirectImport && !scriptSheet.hasStringEntryForId(id)) {
          // return as a success if target file does not exist
          // so the import directive will be removed
          success = true;
        }
        else {
          YmmTranslationSheetEntry str = scriptSheet.getStringEntryById(id);
          
  //        if (substitutionsOn) {
  //          str.stringContent = doSubstitutions(str.stringContent);
  //        }
          
          // HACK: ignore "empty" cells for now
          if (!(str.stringContent.empty() && str.stringPrefix.empty())) {
            ofs.writeString(str.stringPrefix);
            ofs.put('\n');
            ofs.writeString(str.stringContent);
            ofs.put('\n');
  //          ofs.writeString(str.stringSuffix);
  //          ofs.put('\n');
          }
          
          TParse::matchChar(lineIfs, ')');
          success = true;
        }
      }
    }
    
    if (!success) {
      ofs.writeString(line);
      ofs.put('\n');
    }
  }
  
  std::string outname = (string("out/scripttxt/") + basename).c_str();
  TFileManip::createDirectoryForFile(outname.c_str());
  ofs.save(outname.c_str());
}

int main(int argc, char* argv[]) {
  if (argc < 1) {
    cout << "Yumimi Mix script importer" << endl;
    cout << "Usage: " << argv[0]
      << endl;
    return 0;
  }
  
  scriptSheet = YmmTranslationSheet();
  scriptSheet.importCsv("script/script_scene.csv");
  importScript("spec_scene.txt");
  
  scriptSheet = YmmTranslationSheet();
  scriptSheet.importCsv("script/script_choice.csv");
  importScript("spec_choice.txt");
  
  return 0;
}
