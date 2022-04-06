#include "ymm/YmmAblkLexeme.h"

namespace Md {


YmmAblkLexeme::YmmAblkLexeme()
  : intVal(0),
    doubleVal(0) { }

bool YmmAblkLexeme::isType(const std::string& type__) const {
  if (type.compare(type__) == 0) return true;
  return false;
}


}
