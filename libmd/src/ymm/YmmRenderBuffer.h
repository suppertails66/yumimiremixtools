#ifndef YMMRENDERBUFFER_H
#define YMMRENDERBUFFER_H


#include "ymm/YmmAblkOp.h"
#include "util/TStream.h"
#include <vector>

namespace Md {


class YmmAblkEnv;

class YmmRenderBuffer {
public:
  typedef std::vector<YmmAblkOp> BufferItemCollection;

  YmmRenderBuffer();
  
  bool dirty;
  BufferItemCollection items;
  
  void addItem(const YmmAblkOp& op);
  void write(BlackT::TStream& dst,
             YmmAblkEnv& env) const;
protected:
  
};


}


#endif
