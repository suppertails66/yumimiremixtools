#include "util/TCdStream.h"
#include "util/TSerialize.h"
#include <cctype>
#include <iostream>

namespace BlackT { 


TCdStream::TCdStream()
  : TIfstream() { }

TCdStream::TCdStream(const char* filename,
          std::ios_base::openmode mode)
  : TIfstream(filename, mode) { }

TCdStream::~TCdStream() { }

void TCdStream::open(const char* filename,
          std::ios_base::openmode mode) {
  TIfstream::open(filename, mode);
}

TCdStream::SectorMode TCdStream::sectorMode() const {
  return config.mode;
}

void TCdStream::setSectorMode(TCdStream::SectorMode mode) {
  config.mode = mode;
}

void TCdStream::seekSector(int sectorNum) {
  seek(sectorNum * rawSectorSize);
  adjustPosByMode();
}

void TCdStream::readBytes(TStream& dst, int byteCount) {
/*  for (int i = 0; i < byteCount; i++) {
    dst.put(get());
  } */
  
  adjustPosByMode();
  
  char buf[rawSectorSize];
  
  int secSize = currentSectorSize();
  int secStart = currentSectorStart();
  while (byteCount > 0) {
    int remainingBytesInSector
      = secSize - ((tell() % rawSectorSize) - secStart);
    if (byteCount < remainingBytesInSector) remainingBytesInSector = byteCount;
    
    ifs.read(buf, remainingBytesInSector);
    dst.write(buf, remainingBytesInSector);
    byteCount -= remainingBytesInSector;
    
    adjustPosByMode();
  }
}

void TCdStream::readSectors(TStream& dst, int sectorCount) {
  int secSize = currentSectorSize();
  readBytes(dst, sectorCount * secSize);
}

char TCdStream::get() {
  adjustPosByMode();
  char result = TIfstream::get();
  adjustPosByMode();
  return result;
}

void TCdStream::unget() {
  // FIXME: never gonna work
  TIfstream::unget();
  adjustPosByMode(true);
}

char TCdStream::peek() {
  adjustPosByMode();
  return TIfstream::peek();
}

void TCdStream::put(char c) {
  TIfstream::put(c);
}

void TCdStream::read(char* dst, int size) {
  // FIXME: do proper sector-split read like with readBytes()
  for (int i = 0; i < size; i++) {
    *dst++ = get();
  }
}

void TCdStream::write(const char* src, int size) {
/*  for (int i = 0; i < size; i++) {
    put(*src++);
  }*/
}

bool TCdStream::good() const {
  return TIfstream::good();
}

bool TCdStream::bad() const {
  return TIfstream::bad();
}

bool TCdStream::fail() const {
  return TIfstream::fail();
}

bool TCdStream::eof() const {
  return TIfstream::eof();
}

void TCdStream::clear() {
  TIfstream::clear();
}

int TCdStream::tell() {
  return TIfstream::tell();
}

void TCdStream::seek(int pos) {
  TIfstream::seek(pos);
  adjustPosByMode();
}

int TCdStream::size() {
  return TIfstream::size();
}

int TCdStream::currentSectorSize() const {
  switch (config.mode) {
  case SectorMode::mode1:
    return mode1SectorSize;
    break;
  case SectorMode::mode2form1:
    return mode2form1SectorSize;
    break;
  case SectorMode::mode2form2:
    return mode2form2SectorSize;
    break;
  case SectorMode::raw:
  default:
    return rawSectorSize;
    break;
  }
}

int TCdStream::currentSectorStart() const {
  switch (config.mode) {
  case SectorMode::mode1:
    return mode1SectorBase;
    break;
  case SectorMode::mode2form1:
  case SectorMode::mode2form2:
    return mode2SectorBase;
  case SectorMode::raw:
  default:
    return 0;
    break;
  }
}

void TCdStream::adjustPosByMode(bool reverse) {
  int lowerLimit = 0;
  int upperLimit = 0;
  switch (config.mode) {
  case SectorMode::mode1:
    lowerLimit = mode1SectorBase;
    upperLimit = mode1SectorBase + mode1SectorSize;
    break;
  case SectorMode::mode2form1:
    lowerLimit = mode2SectorBase;
    upperLimit = mode2SectorBase + mode2form1SectorSize;
    break;
  case SectorMode::mode2form2:
    lowerLimit = mode2SectorBase;
    upperLimit = mode2SectorBase + mode2form2SectorSize;
    break;
  case SectorMode::raw:
  default:
    // no adjustment
    return;
    break;
  }
  
  int currentPos = tell();
  int sectorOffset = currentPos % rawSectorSize;
  int thisSectorStart = currentPos - sectorOffset;
  int nextSectorStart = thisSectorStart + rawSectorSize;
  int prevSectorStart = thisSectorStart - rawSectorSize;
  
  if (!reverse) {
    if ((sectorOffset < lowerLimit)) {
      seek(thisSectorStart + lowerLimit);
    }
    else if ((sectorOffset >= upperLimit)) {
      seek(nextSectorStart + lowerLimit);
    }
  }
  else {
    if ((sectorOffset < lowerLimit)) {
      seek(prevSectorStart + upperLimit - 1);
    }
    else if ((sectorOffset >= upperLimit)) {
      seek(prevSectorStart + upperLimit - 1);
    }
  }
}

TCdStream::TCdStreamConfig::TCdStreamConfig() {
  mode = SectorMode::raw;
}


}
