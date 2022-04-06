#include "ymm/YmmAblkOpNotFoundExp.h"

using namespace BlackT;

namespace Md {


YmmAblkOpNotFoundExp::YmmAblkOpNotFoundExp(
                 const char* nameOfSourceFile__,
                 int lineNum__,
                 const std::string& source__)
  : TException(nameOfSourceFile__, lineNum__, source__) { }


} 
