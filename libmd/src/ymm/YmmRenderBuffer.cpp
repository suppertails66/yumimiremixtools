#include "ymm/YmmRenderBuffer.h"
#include "ymm/YmmAblkEnv.h"

namespace Md {


YmmRenderBuffer::YmmRenderBuffer()
  : dirty(false) { }

void YmmRenderBuffer::addItem(const YmmAblkOp& op) {
  if (dirty) {
    items.clear();
    dirty = false;
  }
  
  items.push_back(op);
}

void YmmRenderBuffer::write(BlackT::TStream& dst,
                            YmmAblkEnv& env) const {
  for (unsigned int i = 0; i < items.size(); i++) {
    items[i].write(dst, env);
  }
}


}
