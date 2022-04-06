#ifndef DISCASTERCONFIG_H
#define DISCASTERCONFIG_H


#include <string>

namespace Discaster {


  class Config {
  public:
    Config();
    
    bool debugOutput() const;
    void setDebugOutput(bool debugOutput__);
    
    bool warningsOn() const;
    void setWarningsOn(bool warningsOn__);
    
    bool fileReportModeOn() const;
    void setFileReportModeOn(bool fileReportModeOn__);
    
    std::string fileReportOutputName() const;
    void setFileReportOutputName(std::string fileReportOutputName__);
    
  protected:
    bool debugOutput_;
    bool warningsOn_;
    bool fileReportModeOn_;
    std::string fileReportOutputName_;
  };
  
  extern Config config;


}


#endif
