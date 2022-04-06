#ifndef TCDSTREAM_H
#define TCDSTREAM_H


#include "util/TIfstream.h"

namespace BlackT {


class TCdStream : public TIfstream {
public:
  enum SectorMode {
    raw,
    mode1,
    mode2form1,
    mode2form2
  };
  
  TCdStream();
  TCdStream(const char* filename,
            std::ios_base::openmode mode
              = std::ios_base::in | std::ios_base::binary);
  virtual ~TCdStream();
  
  void open(const char* filename,
            std::ios_base::openmode mode
              = std::ios_base::in | std::ios_base::binary);
  
  SectorMode sectorMode() const;
  void setSectorMode(SectorMode mode);
  
  void seekSector(int sectorNum);
  void readBytes(TStream& dst, int byteCount);
  void readSectors(TStream& dst, int sectorCount);
  
  virtual char get();
  virtual void unget();
  virtual char peek();
  virtual void put(char c);
  virtual void read(char* dst, int size);
  virtual void write(const char* src, int size);
  virtual bool good() const;
  virtual bool bad() const;
  virtual bool fail() const;
  virtual bool eof() const;
  virtual void clear();
  virtual int tell();
  virtual void seek(int pos);
  virtual int size();
protected:
  struct TCdStreamConfig {
    TCdStreamConfig();
    
    SectorMode mode;
  };
  
  TCdStreamConfig config;
  
  const static int rawSectorSize = 0x930;
  const static int mode1SectorBase = 0x10;
  const static int mode1SectorSize = 0x800;
  const static int mode2SectorBase = 0x18;
  const static int mode2form1SectorSize = 0x800;
  const static int mode2form2SectorSize = 0x914;
  
  int currentSectorSize() const;
  int currentSectorStart() const;
  void adjustPosByMode(bool reverse = false);
};


}


#endif
