#include "discaster/Config.h"

namespace Discaster {


  Config::Config()
    : debugOutput_(false),
      warningsOn_(true) { }
  
  bool Config::debugOutput() const {
    return debugOutput_;
  }
  
  void Config::setDebugOutput(bool debugOutput__) {
    debugOutput_ = debugOutput__;
  }
  
  bool Config::warningsOn() const {
    return warningsOn_;
  }
  
  void Config::setWarningsOn(bool warningsOn__) {
    warningsOn_ = warningsOn__;
  }
  
  bool Config::fileReportModeOn() const {
    return fileReportModeOn_;
  }
  
  void Config::setFileReportModeOn(bool fileReportModeOn__) {
    fileReportModeOn_ = fileReportModeOn__;
  }
  
  std::string Config::fileReportOutputName() const {
    return fileReportOutputName_;
  }
  
  void Config::setFileReportOutputName(std::string fileReportOutputName__) {
    fileReportOutputName_ = fileReportOutputName__;
  }
  
  Config config;


}
