#ifndef TSOUNDFILE_H
#define TSOUNDFILE_H


#include "util/TStream.h"
#include "util/TArray.h"
#include <string>
#include <vector>

namespace BlackT {


class TSoundFile {
public:
  typedef double TSample;

  TSoundFile();
  virtual ~TSoundFile();
  
  void importWav(TStream& ifs);
  void exportWav(TStream& ofs) const;
  void exportRaw16BitSigned(TStream& ofs) const;
  
  int channels() const;
  void setChannels(int channels__);
  
  int rate() const;
  void setRate(int rate__);
  
  void addSample(TSample sample);
  // add unsigned 8-bit sample
  void addSampleU8(int sample);
  // add signed 8-bit sample
  void addSampleS8(int sample);
  // add signed 16-bit sample
  void addSampleS16(int sample);
  
  std::vector<TSample>& data();
  const std::vector<TSample>& data() const;
  
protected:
  std::vector<TSample> data_;
  int channels_;
  int rate_;
};


}


#endif
