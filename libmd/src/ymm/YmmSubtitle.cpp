#include "ymm/YmmSubtitle.h"
#include "ymm/YmmAblkOps.h"
#include "ymm/YmmAblkOp.h"
#include "ymm/YmmAblkEnv.h"
#include "util/TBufStream.h"

using namespace BlackT;

namespace Md {


YmmSubtitle::YmmSubtitle() { }

void YmmSubtitle::write(BlackT::TStream& dst,
                        YmmAblkEnv& env) const {
  /* 18  renderSpriteOverride? -> renderObj
  Parameters:
    1b <spriteObjIndex>
    1b <nametableMask>
      - if zero, treated as 0x2000 (palette 1, priority 0)
      - if 1, treated as 0xA000 (palette 1, priority 1)
      - otherwise, shifted left 8 bits
    WORDPARAM <xpos = spriteObjWord1Override>
    WORDPARAM <ypos = spriteObjWord2Override>
  Effect:
    Renders(?) a sprite object using the specified parameters in place of
    any defaults or listed values?
    <nametableMask> is ORed onto the nametable entry for each generated
    sprite. */
  
  // FIXME
  for (SubObjEntryCollection::const_iterator it = subObjEntries.cbegin();
       it != subObjEntries.cend();
       ++it) {
    // make renderObj op
    YmmAblkOp op = makeYmmAblkOp(0x18);
    
    const SubObjEntry& item = *it;
    
    TBufStream paramStr;
    // object graphic index
    paramStr.writeu8(item.graphicIndex);
    // sprite table OR mask
    // setting low bit now indicates object should be positioned relative
    // to camera, regardless of scene settings
    paramStr.writeu8(0xE0 | 0x01);
    // x
  //    paramStr.writeu16be(32);
//    paramStr.writeu16be(232);
    paramStr.writeu16be(item.x);
    // y
  //    paramStr.writeu16be(32);
//    paramStr.writeu16be(200);
    paramStr.writeu16be(item.y);
    
    paramStr.seek(0);
    op.readParams(paramStr);
    
    op.write(dst, env);
    // HACK: why
    dst.writeu8(0x80);
  }
}


}
