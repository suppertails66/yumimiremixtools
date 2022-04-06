#include "sm/SmFont.h"
#include "util/TIniFile.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"

using namespace BlackT;

namespace Md {


SmFont::SmFont() { }
  
void SmFont::addCharacter(const SmFontEntry& entry) {
  entries.push_back(entry);
}

void SmFont::save(const std::string& prefix) const {
  TIniFile ini;
  for (unsigned int i = 0; i < entries.size(); i++) {
    std::string numstr = TStringConversion::intToString(i);
    while (numstr.size() < 3) numstr = "0" + numstr;
    std::string filenamestr = numstr + ".png";
    
//    ini.setValue(numstr,
//                 "filename",
//                 filenamestr);
    ini.setValue(numstr,
                 "width",
                 TStringConversion::intToString(entries[i].width));
/*    ini.setValue(numstr,
                 "advanceWidth",
                 TStringConversion::intToString(entries[i].advanceWidth)); */
    
    TPngConversion::graphicToRGBAPng(prefix + filenamestr, entries[i].graphic);
  }
  
  ini.writeFile(prefix + "index.txt");
}

void SmFont::load(const std::string& prefix) {
  entries.clear();

  TIniFile ini;
  ini.readFile(prefix + "index.txt");
  
  for (SectionKeysMap::const_iterator it = ini.cbegin();
       it != ini.cend();
       ++it) {
    int width = TStringConversion::stringToInt(
      (it->second.find("width"))->second);
/*    int advanceWidth = TStringConversion::stringToInt(
      (it->second.find("advanceWidth"))->second); */
//    std::string filenamestr = (it->second.find("filename"))->second;
    std::string filenamestr = it->first + ".png";
    
    TGraphic graphic;
    TPngConversion::RGBAPngToGraphic(prefix + filenamestr, graphic);
    
//    entries.push_back(SmFontEntry { graphic, width, advanceWidth });
    SmFontEntry entry { graphic, width, width };
    entries.push_back(entry);
  }
}


}
