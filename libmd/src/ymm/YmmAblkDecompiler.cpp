#include "ymm/YmmAblkDecompiler.h"
#include "ymm/YmmAblkOps.h"
#include "ymm/YmmAblkOpNotFoundExp.h"
#include "util/TStringConversion.h"
#include <iomanip>
#include <cstring>

namespace Md {


YmmAblkDecompiler::YmmAblkDecompiler(
    const std::string& outprefix__,
    YmmEngineVersions::YmmEngineVersion version__)
  : outprefix_(outprefix__),
    version_(version__),
    indentLevel_(0) { }
  
void YmmAblkDecompiler::decompile(BlackT::TStream& ifs,
                                  std::ostream& ofs,
                                  YmmTranslationData* translationData) {
//  for (int i = 0; i < numYmmAblkOps; i++) {
//    std::cout << std::hex << ymmAblkOps[i].id << std::endl;
//  }

  // Fetch next op byte
  char next = ifs.get();
  
  // 00 = script terminator
  // TODO: what if more than one of these is allowed and can be conditionally
  // branched over?
  int choiceNum = 0;
  while (next != 0x00) {
    try {
//      std::cerr << std::hex << ifs.tell() - 1 << " " << (unsigned int)next << std::endl;
      int pos = ifs.tell() - 1;
      YmmAblkOp op = makeYmmAblkOp((int)next);
      op.readParams(ifs);
      
      if (translationData != NULL) {
        if (op.id == 0x02) {
          // get subcode
          int opSubcode = op.getParam(0).value;
          
          // choice prompt
          if (opSubcode == 0x01) {
            YmmChoiceData choiceData;
            choiceData.tilemapId = op.getParam(1).value;
            choiceData.x = op.getParam(2).value;
            choiceData.y = op.getParam(3).value;
            choiceData.w = op.getParam(4).value;
            choiceData.h = op.getParam(5).value;
            
            translationData->choiceData.push_back(choiceData);
            
            std::string varNameBase = std::string("CHOICE")
              + BlackT::TStringConversion::intToString(choiceNum)
              + "_";
            
            if (translationData->doTranslationModifications) {
              // replace params with placeholder variables
              op.getParam(2).type = YmmAblkOpParamTypes::typeVariable;
              op.getParam(3).type = YmmAblkOpParamTypes::typeVariable;
              op.getParam(4).type = YmmAblkOpParamTypes::typeVariable;
              op.getParam(5).type = YmmAblkOpParamTypes::typeVariable;
              
              op.getParam(2).labelRep = varNameBase + "X";
              op.getParam(3).labelRep = varNameBase + "Y";
              op.getParam(4).labelRep = varNameBase + "W";
              op.getParam(5).labelRep = varNameBase + "H";
            }
            
            ++choiceNum;
          }
        }
      }
      
      ofs << "loc_" << std::setw(8) << std::setfill('0') << std::hex
        << pos << ": " << op.name;
      int total = 8 + 2 + std::strlen(op.name);
      while (total < 40) { ofs << " "; ++total; }
      ofs << "(";
      op.printParams(ofs);
      ofs << ")";
      ofs << std::endl;
    }
    catch (YmmAblkOpNotFoundExp& e) {
      std::cerr << "Error: unknown op 0x" << std::hex << std::setw(2)
        << std::setfill('0')
        << (int)(unsigned char)next
        << " at 0x" << std::hex << ifs.tell() - 1 << std::endl;
      throw e;
//      return;
    }
    
    next = ifs.get();
  }
  
  
  
  
}


}
