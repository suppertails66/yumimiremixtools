#include "ymm/YmmScene.h"
#include "util/TStringConversion.h"
#include "util/TIniFile.h"
#include "util/TBufStream.h"
#include "exception/TGenericException.h"
#include <string>
#include <cstring>

using namespace BlackT;

namespace Md {


const char* YmmScene::sceneName_("scene");
const char* YmmScene::infoFileName_("info.txt");
  
YmmScene::YmmScene()
  : thingy_(NULL),
    subtitleSceneSet_(NULL) { }
  
void YmmScene::read(BlackT::TStream& ifs,
    YmmEngineVersions::YmmEngineVersion engineVersion__) {
  engineVersion_ = engineVersion__;
  
  char headcheck[5];
  ifs.read(headcheck, 4);
  headcheck[4] = 0x00;
  
  if (strcmp(headcheck, "ABLK") == 0) sceneType_ = YmmSceneTypes::ablk;
  else {
    sceneType_ = YmmSceneTypes::oneshot;
    
    // Read the one contained subscene and return
    
    ifs.seekoff(-4);
    subscenes_.push_back(YmmSubscene());
    subscenes_[0].setUpTranslationData(translationData_);
    
    // no PCMs allowed -- use empty list
    YmmPcmList pcmList;
    subscenes_[0].read(ifs, 0, pcmList);
    
    if (subtitleSceneSet_ != NULL) {
      subscenes_[0].setUpSubtitleParams(*thingy_,
        subtitleSceneSet_->getSubtitleSet(fileName_, subscenes_.size() - 1),
        *subtitleResources_);
    }
    
    return;
  }
  
  // loop over entries in header until subscene terminator (high nybble 0xF)
  // reached
  int next = ifs.readu32be();
  while ((next & 0xF0000000) != 0xF0000000) {
//    std::cout << "Processing subscene " << subscenes_.size() << std::endl;
    
//    std::cout << std::hex << ifs.tell() << std::endl;
//    std::cout << std::hex << next << std::endl;
//    char c; std::cin >> c;
  
    YmmPcmList pcmList;
    pcmList.read(ifs);
    
    int nextpos = ifs.tell();
    
    subscenes_.push_back(YmmSubscene());
    YmmSubscene& scene = subscenes_[subscenes_.size() - 1];
    scene.setUpTranslationData(translationData_);
    ifs.seek((next & 0x00FFFFFF) + 4);
    scene.read(ifs, 4, pcmList, engineVersion_);
    
    if (subtitleSceneSet_ != NULL) {
      scene.setUpSubtitleParams(*thingy_,
        subtitleSceneSet_->getSubtitleSet(fileName_, subscenes_.size() - 1),
        *subtitleResources_);
    }
    
    ifs.clear();
    ifs.seek(nextpos);
    
    next = ifs.readu32be();
  }
}

void YmmScene::write(BlackT::TStream& ofs) const {
  if (sceneType_ == YmmSceneTypes::oneshot) {
    writeSub1(ofs);
    return;
  }
  
  std::vector<TBufStream> sceneBuffers;
  std::vector<YmmPcmList> pcmLists;
  int numPcmFiles = 0;
  for (int i = 0; i < subscenes_.size(); i++) {
    sceneBuffers.push_back(TBufStream(0x100000));
    pcmLists.push_back(YmmPcmList());
    subscenes_[i].write(sceneBuffers.back(), pcmLists.back());
    numPcmFiles += pcmLists.back().pcmIds.size();
  }
  
  writeSub2(ofs, sceneBuffers, pcmLists, numPcmFiles);
}

void YmmScene::writeSub1(BlackT::TStream& ofs) const {
  if (subscenes_.size() > 0) {
    YmmPcmList pcm;
    subscenes_[0].write(ofs, pcm);
  }
  return;
}

void YmmScene::writeSub2(BlackT::TStream& ofs,
                 std::vector<BlackT::TBufStream>& sceneBuffers,
                 std::vector<YmmPcmList>& pcmLists,
                 int& numPcmFiles) const {
  // compute index size
  // 4 bytes per subscene offset
  // 4 bytes per PCM offset
  // 4 bytes for list terminator
  int indexSize = (subscenes_.size() * 4)
    + (numPcmFiles * 4)
    + 4;
  
  ofs.write("ABLK", 4);
  
  int subsceneOffset = indexSize;
  for (int i = 0; i < pcmLists.size(); i++) {
    ofs.writeu32be(subsceneOffset);
    for (int j = 0; j < pcmLists[i].pcmIds.size(); j++) {
      int offset = pcmLists[i].pcmIds[j].offset + subsceneOffset;
      ofs.writeu32be(0x80000000 | offset);
    }
    subsceneOffset += sceneBuffers[i].size();
    
    // write terminator after last entry
    if (i == (pcmLists.size() - 1)) {
      ofs.writeu32be(0xF0000000 | subsceneOffset);
    }
  }
  
  // write each subscene's data
  for (int i = 0; i < sceneBuffers.size(); i++) {
    ofs.write((char*)sceneBuffers[i].data().data(),
              sceneBuffers[i].size());
  }
}

void YmmScene::writeWithGeneratedSubs(BlackT::TStream& ofs) {
  if (sceneType_ == YmmSceneTypes::oneshot) {
    if (subscenes_.size() > 0) {
      YmmPcmList pcm;
      subscenes_[0].writeWithGeneratedSubs(ofs, pcm);
    }
    return;
  }
  
  std::vector<TBufStream> sceneBuffers;
  std::vector<YmmPcmList> pcmLists;
  int numPcmFiles = 0;
  for (int i = 0; i < subscenes_.size(); i++) {
    sceneBuffers.push_back(TBufStream());
    pcmLists.push_back(YmmPcmList());
    subscenes_[i].writeWithGeneratedSubs(sceneBuffers.back(), pcmLists.back());
    numPcmFiles += pcmLists.back().pcmIds.size();
  }
  
  writeSub2(ofs, sceneBuffers, pcmLists, numPcmFiles);
}
  
void YmmScene::save(const char* prefix) const {
  std::string prefixStr(prefix);
  
  {
    TIniFile infoFile;
    
    infoFile.setValue("Properties", "SceneType",
                      TStringConversion::intToString(sceneType_,
                        TStringConversion::baseHex));
    
    infoFile.setValue("Properties", "NumSubscenes",
                      TStringConversion::intToString(subscenes_.size(),
                        TStringConversion::baseHex));
    
    infoFile.writeFile(prefixStr + infoFileName_);
  }
  
  for (int i = 0; i < subscenes_.size(); i++) {
    std::string name = prefixStr + "scene"
      + TStringConversion::intToString(i);
    subscenes_[i].save(name.c_str());
  }
}

void YmmScene::load(const char* prefix) {
  std::string prefixStr(prefix);
  
  subscenes_.clear();
  
  {
    TIniFile infoFile(prefixStr + infoFileName_);
    
    sceneType_ = static_cast<YmmSceneTypes::YmmSceneType>(
      TStringConversion::stringToInt(
        infoFile.valueOfKey("Properties", "SceneType")));
    
    subscenes_.resize(TStringConversion::stringToInt(
      infoFile.valueOfKey("Properties", "NumSubscenes")));
  }
  
  for (int i = 0; i < subscenes_.size(); i++) {
    subscenes_[i].setUpTranslationData(translationData_);
    subscenes_[i].load((prefixStr + "scene"
      + TStringConversion::intToString(i)).c_str());
    
    if (subtitleSceneSet_ != NULL) {
      subscenes_[i].setUpSubtitleParams(*thingy_,
        subtitleSceneSet_->getSubtitleSet(fileName_, i),
        *subtitleResources_);
    }
  }
}

int YmmScene::numSubscenes() const {
  return subscenes_.size();
}

YmmSubscene& YmmScene::subscene(int index) {
  if (index >= subscenes_.size()) {
    throw TGenericException(T_SRCANDLINE,
                      "subscene()",
                      std::string("Subscene index out of range: ")
                        + TStringConversion::intToString(subscenes_.size())
                        + " subscenes total, tried to access index "
                        + TStringConversion::intToString(index));
  }
  
  return subscenes_[index];
}

YmmTranslationData& YmmScene::translationData() {
  return translationData_;
}

const YmmTranslationData& YmmScene::translationData() const {
  return translationData_;
}

void YmmScene::setUpSubtitleParams(std::string fileName__,
                         BlackT::TThingyTable& thingy__,
                         YmmSubtitleSceneSet& subtitleSceneSet__,
                         YmmSubtitleResources& subtitleResources__) {
  fileName_ = fileName__;
  thingy_ = &thingy__;
  subtitleSceneSet_ = &subtitleSceneSet__;
  subtitleResources_ = &subtitleResources__;
}


}
