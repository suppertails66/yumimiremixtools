#include "ymm/YmmAblkEnv.h"
#include "exception/TGenericException.h"

using namespace BlackT;

namespace Md {


YmmAblkEnv::YmmAblkEnv()
  : translationData(NULL) { }

void YmmAblkEnv::addLabel(std::string name, int pos) {
  // FIXME: this SHOULD be an error.
  // but unfortunately i already copy-pasted a bunch of the
  // unnecessary auto-generated labels around in the WIP
  // scripts, and i'm not fixing them all now.
  // so we'll just have to deal with this not being checked.
/*  if (labels.find(name) != labels.end()) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkEnv::addLabel()",
                            std::string("Error: multiply defined label '")
                              + name
                              + "'");
  } */
  
  labels[name] = pos;
}

void YmmAblkEnv::addLabelTarget(std::string name, int offset, int width) {
  LabelTarget target;
  target.name = name;
  target.offset = offset;
  target.width = width;
  labelTargets.push_back(target);
}


}
