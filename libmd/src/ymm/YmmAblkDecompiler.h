#ifndef YMM_ABLK_DECOMPILER_H
#define YMM_ABLK_DECOMPILER_H


#include "util/TStream.h"
#include "ymm/YmmEngineVersions.h"
#include "ymm/YmmTranslationData.h"
#include <string>
#include <iostream>

namespace Md {


class YmmAblkDecompiler {
public:
  YmmAblkDecompiler(const std::string& outprefix__,
                    YmmEngineVersions::YmmEngineVersion version__);
  
  void decompile(BlackT::TStream& ifs,
                 std::ostream& ofs,
                 YmmTranslationData* translationData = NULL);
protected:

  std::string outprefix_;
  YmmEngineVersions::YmmEngineVersion version_;
  
  int indentLevel_;
  
};


}


#endif
