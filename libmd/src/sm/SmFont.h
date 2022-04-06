#ifndef SMFONT_H
#define SMFONT_H


#include "util/TGraphic.h"
#include "util/TStream.h"
#include <vector>
#include <string>

namespace Md {


struct SmFontEntry {
  BlackT::TGraphic graphic;
  int width;
  int advanceWidth;
};

class SmFont {
public:
  SmFont();
  
  void addCharacter(const SmFontEntry& entry);
  
  void save(const std::string& prefix) const;
  void load(const std::string& prefix);
  
  std::vector<SmFontEntry> entries;
  
protected:
  
};


}


#endif
