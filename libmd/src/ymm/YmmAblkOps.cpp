#include "ymm/YmmAblkOps.h"
#include "ymm/YmmAblkOpNotFoundExp.h"
#include "exception/TGenericException.h"
#include "util/TStringConversion.h"
#include <iostream>

using namespace BlackT;

namespace Md {


YmmAblkOp makeYmmAblkOp(int id) {
  for (int i = 0; i < numYmmAblkOps; i++) {
    if (ymmAblkOps[i].id == id) return ymmAblkOps[i];
  }
  
  throw YmmAblkOpNotFoundExp(T_SRCANDLINE,
                          "makeYmmAblkOp()");
}

YmmAblkOp makeYmmAblkOp02Subop(int id) {
  for (int i = 0; i < numYmmAblkOp02Subops; i++) {
    if (ymmAblkOp02Subops[i].id == id) return ymmAblkOp02Subops[i];
  }
  
  throw YmmAblkOpNotFoundExp(T_SRCANDLINE,
                          "makeYmmAblkOp02Subop()");
}


}
