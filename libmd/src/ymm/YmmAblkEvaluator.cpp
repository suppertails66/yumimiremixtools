#include "ymm/YmmAblkEvaluator.h"
#include "ymm/YmmAblkOps.h"
#include "ymm/YmmSubscene.h"
#include "ymm/YmmSubtitle.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "exception/TGenericException.h"
#include <cstring>
#include <cctype>
#include <string>

using namespace BlackT;

namespace Md {


YmmAblkEvaluator::YmmAblkEvaluator(YmmAblkLexStream& src__,
                 BlackT::TStream& dst__)
  : src_(&src__),
    dst_(&dst__),
    patternsTransferred(false),
    pendingAfterDraw(true),
    subscene_(NULL),
    thingy_(NULL),
    subtitleSet_(NULL),
    subtitleResources_(NULL) { }

void YmmAblkEvaluator::evaluate() {
  while (!src_->eof()) {
    evaluateStatement();
  }
  dst_->writeu8be(0x00);
  
  // resolve label references
  for (unsigned int i = 0; i < env_.labelTargets.size(); i++) {
    LabelTarget& target = env_.labelTargets[i];
    int dstAddr = env_.labels.at(target.name);
    int width = target.width;
    // note: offset must be 1 less than actual target
    // because the interpreter loop assumes that the
    // previous op's handler left the script pointer
    // pointed at its argument terminator byte,
    // which it then skips.
    // we also must account for the width of the offset
    // (destination is based on address following offset)
    int relOffset = dstAddr - (target.offset + width) - 1;
    
//    std::cerr << dstAddr << " " << relOffset << " " << width << std::endl;
    
    dst_->seek(target.offset);
    dst_->writeInt(relOffset, width,
                  EndiannessTypes::big,
                  SignednessTypes::sign);
  }
  dst_->seek(dst_->size());
}
  
void YmmAblkEvaluator::evaluateStatement() {
  // Check whether this is a dot directive
  bool isDot = false;
  if (src_->peek().type == ".") {
    isDot = true;
    src_->get();
  }

  // Get name
  int lineNum = src_->peek().lineNum;
  std::string name = src_->get().stringVal;
  
  // If followed by a colon, this is a label
  if (src_->peek().type == ":") {
    src_->get();
    // make a note of current script putpos
    env_.addLabel(name, dst_->tell());
    return;
  }
  
  if (isDot) {
    evaluateDirective(name, lineNum);
    return;
  }
  
  // find matching function in op list
  for (int i = 0; i < numYmmAblkOps; i++) {
    if (strcmp(name.c_str(), ymmAblkOps[i].name) == 0) {
//      std::cout << i << std::endl;
      
      // match open paren
      if (src_->get().type.compare("(") != 0) {
        throw TGenericException(T_SRCANDLINE,
                                "YmmAblkEvaluator::evaluateStatement()",
                                "Error:\n  Line "
                                  + TStringConversion::intToString(lineNum)
                                  + ": no open parenthesis for arguments");
      }
      
      YmmAblkOp op = makeYmmAblkOp(ymmAblkOps[i].id);
      op.readParams(*src_, env_);
      
      // match close paren
      if (src_->get().type.compare(")") != 0) {
        throw TGenericException(T_SRCANDLINE,
                                "YmmAblkEvaluator::evaluateStatement()",
                                "Error:\n  Line "
                                  + TStringConversion::intToString(lineNum)
                                  + ": no close parenthesis for arguments");
      }
      
      //=====================================
      // TEMP:
      // this and the following commented
      // section are all leftovers from the
      // phase where this project was targeting
      // the Mega CD version instead of the Saturn.
      // this part automatically generates subtitles
      // indicating what sound is currently playing
      // (intended to be referenced by translators
      // in videos recorded for this purpose).
      //=====================================
      
      // if opcode is a PCM playback trigger, generate and display an
      // extra subtitle indicating the sound's ID
/*      if ((op.id == 0x1C)    // playPcm (actually "play channel A sound")
          || (op.id == 0x22) // playSe (actually "play channel B sound")
          ) {
        int soundId = op.getParam(0).value;
        // 0x80 = stop sound
        if (soundId != 0x80) {
          int newSubId = soundId + 1024;
          
          // add subtitle entry if none exists
          if (subtitleSet_->subtitles.find(newSubId)
                == subtitleSet_->subtitles.end()) {
            YmmSubtitle newSub;
            newSub.text = TStringConversion::intToString(soundId);
            newSub.styleName = "soundId";
            subtitleSet_->subtitles[newSubId] = newSub;
          }
          
          // remove any existing sub from buffer
          subtitleBuffer.removeItemsWithStyleName(
            subtitleSet_->getSubtitle(newSubId).styleName);
          
          // display new subtitle
          addSubtitle(newSubId);
        }
        else {
          subtitleBuffer.removeItemsWithStyleName(
            "soundId");
        }
      }
      // remove sound codes after PCM sync wait
      else if ((op.id == 0x1D)  // waitForPcm
               ) {
        subtitleBuffer.removeItemsWithStyleName(
          "soundId");
      }*/
      
      //============================================
      // this next part does a lot of ridiculous
      // antics to inject subtitles into the game
      // using auto-generated script commands.
      // while largely functional,
      // it's completely unnecessary now that we're
      // working with the saturn version and the
      // subtitles are done by an external overlay
      // completely independent of the game itself.
      // oh well, i'll take it over having to try
      // to make this run on the mega cd any day of the week.
      //============================================
      
//      #if USE_SUBBING_EXTENSIONS
      /* 
      - as we process the script, keep track of the original buffer contents
        and state (dirty/clean).
        - remember that, aside from the actual draw() op, several other ops
          have implicit or optional embedded commands (e.g. scrollFg/scrollBg)
          that will flag the buffer as dirty.
      - also keep track separately of which subtitles are active
      - after each draw command, set a flag.
        the first time a render OR a draw command is encountered while
        this flag is set:
          - if the trigger command was a render command, 
            prepend renderObj commands for all active subtitles.
          - if the trigger command was a draw command:
            - if subtitles have been added or removed since the last
              time this operation ran (subtitleBufferModified flag),
              prepend renderObj commands for all active subtitles,
              followed by contents of original-content buffer.
            - otherwise, do nothing.
              subtitles and content from previous draw will remain. */
        
/*        if (op.isPatternTransferOp()) patternsTransferred = true;
        
        // do not add any subtitles until patterns have been transferred
        if (patternsTransferred) {
          // check if this is a draw or render command.
          // if so, output subtitle/render buffer data as needed.
          if (pendingAfterDraw) {
            if (op.isRenderOp()) {
  //  std::cerr << "render: " << lineNum << std::endl;
              // prepend renderObj commands for all active subtitles
              subtitleBuffer.write(*dst_, env_);
              
              pendingAfterDraw = false;
            }
            else if (op.isDrawOp()) {
              if (subtitleBuffer.modifiedSinceLastDraw) {
  //  std::cerr << "draw: " << lineNum << std::endl;
  //  std::cerr << renderBuffer.items.size() << std::endl;
                if ((renderBuffer.items.size() == 0)
                    && (subtitleBuffer.items.size() == 0)) {
                  // clear objects from screen
                  // there has to be a better way to do this...?
                  // (no, this is how the game does it: autoRender an
                  // empty list)
                  dst_->writeu16be(0x0680);
                }
                else {
                  subtitleBuffer.write(*dst_, env_);
                  renderBuffer.write(*dst_, env_);
                }
              }
            }
          }
          
          // check if this is a render command.
          // if so, add to render buffer
          if (op.isRenderOp()) {
            renderBuffer.addItem(op);
  //  std::cerr << "render add: " << lineNum << std::endl;
  //  std::cerr << renderBuffer.items.size() << std::endl;
          }
          // if a draw command, flag render buffer as dirty
          else if (op.isDrawOp()) {
            renderBuffer.dirty = true;
            subtitleBuffer.modifiedSinceLastDraw = false;
            pendingAfterDraw = true;
          }
          
          // TODO: do we need this?
          // scrolling ops require us to reposition the subtitle objects
//          if (op.isScrollOp()) {
//            subtitleBuffer.modifiedSinceLastDraw = true;
//          }
        }*/
//      #endif
      
//      op.printParams(std::cerr);
//      std::cerr << std::endl;
      op.write(*dst_, env_);
      
      return;
    }
  }
  
  throw TGenericException(T_SRCANDLINE,
                          "YmmAblkEvaluator::evaluateStatement()",
                          "Error:\n  Line "
                            + TStringConversion::intToString(lineNum)
                            + ": unknown function "
                            + name);
}

void YmmAblkEvaluator::evaluateDirective(std::string name, int lineNum) {
  for (unsigned int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("SUB") == 0) {
    YmmSubtitleId id = nextSubtitleId();
    
    // remove any existing subtitle whose style name is the same as
    // that of the new subtitle
    subtitleBuffer.removeItemsWithStyleName(
      subtitleSet_->getSubtitle(id).styleName);
    
    addSubtitle(id);
  }
  else if (name.compare("SUBON") == 0) {
    YmmSubtitleId id = nextSubtitleId();
    addSubtitle(id);
  }
  else if (name.compare("SUBOFF") == 0) {
    YmmSubtitleId id = nextSubtitleId();
    subtitleBuffer.removeItem(id);
  }
  else if (name.compare("CLEARSUBS") == 0) {
    subtitleBuffer.clear();
  }
  else if (name.compare("OBJOVERLAYON") == 0) {
    
    YmmSubtitle subtitle;
    
    YmmSubtitle::SubObjEntry entry;
    YmmSubtitleId id = src_->match("INTEGER").intVal;
    entry.graphicIndex = src_->match("INTEGER").intVal;
    entry.x = src_->match("INTEGER").intVal;
    entry.y = src_->match("INTEGER").intVal;
    subtitle.subObjEntries.push_back(entry);
    
    subtitleBuffer.addItem(id, subtitle);
  }
  else {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkEvaluator::evaluateDirective()",
                            "Error:\n  Line "
                              + TStringConversion::intToString(lineNum)
                              + ": unknown directive "
                              + name);
  }
}

void YmmAblkEvaluator
    ::setUpTranslationData(YmmTranslationData& translationData__) {
//  translationData_ = &translationData__;
  env_.translationData = &translationData__;
}

void YmmAblkEvaluator::setUpSubtitleParams(YmmSubscene& subscene__,
                         BlackT::TThingyTable& thingy__,
                         YmmSubtitleSet& subtitleSet__,
                         YmmSubtitleResources& subtitleResources__) {
  subscene_ = &subscene__;
  thingy_ = &thingy__;
  subtitleSet_ = &subtitleSet__;
  subtitleResources_ = &subtitleResources__;
}

void YmmAblkEvaluator::generateSubtitleContent(YmmSubtitle& subtitle) {
  TBufStream textStr;
  textStr.writeString(subtitle.text);
  textStr.seek(0);
  
  std::vector<TGraphic> subGraphics;
  int w = 0;
  int h = 0;
  int lineH = 0;
  
  while (!textStr.eof()) {
    // copy until eof or linebreak
    TBufStream str;
    while (!textStr.eof()
           && (textStr.peek() != '\r')
           && (textStr.peek() != '\n')) {
      str.put(textStr.get());
    }
    
    // skip linebreak characters
    while (!textStr.eof()
           && ((textStr.peek() == '\n')
               || (textStr.peek() == '\r'))) {
      textStr.get();
    }
    
    TGraphic grp;
    str.seek(0);
    subtitleResources_->font.render(grp, str, *thingy_);
    
    if (grp.w() > w) w = grp.w();
    h += grp.h();
    lineH = grp.h();
    
    subGraphics.push_back(grp);
  }
  
  int oldH = h;
  
  // round output size up to nearest pattern boundary
  if ((w % 8) != 0) w = ((w / 8) * 8) + 8;
  if ((h % 8) != 0) h = ((h / 8) * 8) + 8;
  
  TGraphic fullGrp(w, h);
  fullGrp.clearTransparent();
  
  // center generated graphic within pattern boundaries
  int centerOffsetY = (h - oldH) / 2;
  for (unsigned int i = 0; i < subGraphics.size(); i++) {
    // center each line within the output graphic
    TGraphic& grp = subGraphics[i];
    int centerOffsetX = (w - grp.w()) / 2;
    
    fullGrp.copy(grp,
                 TRect(centerOffsetX, (i * lineH) + centerOffsetY, 0, 0));
  }
  
  // compute target x/y
  
  int baseTargetX = 160;
  int baseTargetY = 200;
  
  if (subtitle.styleName.compare("top") == 0) {
    // top
    baseTargetY = 24;
  }
  else if (subtitle.styleName.compare("soundId") == 0) {
    // bottom-left corner
    baseTargetX = 20;
    baseTargetY = 208;
  }
  else {
    // default
    baseTargetY = 200;
  }
  
  // center on both axes at target coordinate
  int targetX = baseTargetX - (fullGrp.w() / 2);
  int targetY = baseTargetY - (fullGrp.h() / 2);
  
  // force into safe display area
  if (targetY + fullGrp.h() > 216) targetY = 216 - fullGrp.h();
  if (targetY < 8) targetY = 8;
  
  // write output
  
  // if output graphic contains more than 48 patterns, split it into
  // four sub-objects to avoid the 48-pattern rule.
  // the resultant objects could still violate the rule, but this is
  // unlikely to occur in practice.
  if ((fullGrp.w() / 8) * (fullGrp.h() / 8) > 48) {
    int subGrpW = fullGrp.w() / 2;
    int subGrpH = fullGrp.h() / 2;
    
    int subGrpPhysW = subGrpW;
    int subGrpPhysH = subGrpH;
    
    if (subGrpPhysW % 8 != 0) subGrpPhysW = ((subGrpPhysW / 8) * 8) + 8;
    if (subGrpPhysH % 8 != 0) subGrpPhysH = ((subGrpPhysH / 8) * 8) + 8;
    
    int subGrpPhysH_2 = fullGrp.h() - subGrpPhysH;
    
    TGraphic ul(subGrpPhysW, subGrpPhysH);
    TGraphic ur(subGrpPhysW, subGrpPhysH);
    TGraphic ll(subGrpPhysW, subGrpPhysH_2);
    TGraphic lr(subGrpPhysW, subGrpPhysH_2);
    
    ul.clearTransparent();
    ur.clearTransparent();
    ll.clearTransparent();
    lr.clearTransparent();
    
    int ulX = targetX;
    int ulY = targetY;
    int urX = targetX + subGrpW;
    int urY = targetY;
    int llX = targetX;
    int llY = targetY + subGrpPhysH;
    int lrX = targetX + subGrpW;
    int lrY = targetY + subGrpPhysH;
    
    ul.copy(fullGrp,
            TRect(0, 0, 0, 0),
            TRect(0, 0, subGrpW, subGrpPhysH));
    ur.copy(fullGrp,
            TRect(0, 0, 0, 0),
            TRect(subGrpW, 0, subGrpW, subGrpPhysH));
    ll.copy(fullGrp,
            TRect(0, 0, 0, 0),
            TRect(0, subGrpPhysH, subGrpW, subGrpPhysH_2));
    lr.copy(fullGrp,
            TRect(0, 0, 0, 0),
            TRect(subGrpW, subGrpPhysH, subGrpW, subGrpPhysH_2));
    
    int ulIndex = subscene_->numObjMaps() + 0;
    int urIndex = subscene_->numObjMaps() + 1;
    int llIndex = subscene_->numObjMaps() + 2;
    int lrIndex = subscene_->numObjMaps() + 3;
    
    subscene_->addObjMap(ul, ulX, ulY);
    subscene_->addObjMap(ur, urX, urY);
    subscene_->addObjMap(ll, llX, llY);
    subscene_->addObjMap(lr, lrX, lrY);
    
    YmmSubtitle::SubObjEntry entry;
    
    entry.graphicIndex = ulIndex;
    entry.x = ulX;
    entry.y = ulY;
    subtitle.subObjEntries.push_back(entry);
    
    entry.graphicIndex = urIndex;
    entry.x = urX;
    entry.y = urY;
    subtitle.subObjEntries.push_back(entry);
    
    entry.graphicIndex = llIndex;
    entry.x = llX;
    entry.y = llY;
    subtitle.subObjEntries.push_back(entry);
    
    entry.graphicIndex = lrIndex;
    entry.x = lrX;
    entry.y = lrY;
    subtitle.subObjEntries.push_back(entry);
  }
  else {
    int newObjIndex = subscene_->numObjMaps();
    subscene_->addObjMap(fullGrp, targetX, targetY);
      
    YmmSubtitle::SubObjEntry entry;
    entry.graphicIndex = newObjIndex;
    entry.x = targetX;
    entry.y = targetY;
    subtitle.subObjEntries.push_back(entry);
  }
}

void YmmAblkEvaluator::addSubtitle(YmmSubtitleId id) {
  YmmSubtitle subtitle = subtitleSet_->getSubtitle(id);
  generateSubtitleContent(subtitle);
  subtitleBuffer.addItem(id, subtitle);
}

YmmSubtitleId YmmAblkEvaluator::nextSubtitleId() {
  YmmSubtitleId id;
  
  if (src_->peek().isType("INTEGER")) {
    id = src_->match("INTEGER").intVal;
  }
  else if (src_->peek().isType("DOUBLE")) {
    id = src_->match("DOUBLE").doubleVal;
  }
  else {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkEvaluator::nextSubtitleId()",
                            std::string("Line ")
                              + TStringConversion::intToString(
                                  src_->peek().lineNum)
                              + ": Unknown subtitle ID token");
  }
  
  return id;
}


}
