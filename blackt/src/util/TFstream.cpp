#include "util/TFstream.h"

namespace BlackT {


TFstream::TFstream() { }

TFstream::TFstream(const char* filename,
          std::ios_base::openmode mode)
  : ifs(filename, mode) { }
          
TFstream::~TFstream() { }

void TFstream::open(const char* filename,
          std::ios_base::openmode mode) {
  ifs.open(filename, mode);
}

void TFstream::close() {
  ifs.close();
}

char TFstream::get() {
  char result = ifs.get();
  ifs.seekp(ifs.tellg());
  return result;
}

void TFstream::unget() {
  ifs.unget();
  ifs.seekp(ifs.tellg());
}

char TFstream::peek() {
  return ifs.peek();
}

void TFstream::put(char c) {
  ifs.put(c);
  ifs.seekg(ifs.tellp());
}

void TFstream::read(char* dst, int size) {
  ifs.read(dst, size);
  ifs.seekp(ifs.tellg());
}

void TFstream::write(const char* src, int size) {
  ifs.write(src, size);
  ifs.seekg(ifs.tellp());
}

bool TFstream::good() const {
  return ifs.good();
}

bool TFstream::bad() const {
  return ifs.bad();
}

bool TFstream::fail() const {
  return ifs.fail();
}

bool TFstream::eof() const {
  return ifs.eof();
}

void TFstream::clear() {
  ifs.clear();
}

int TFstream::tell() {
  return ifs.tellg();
}

void TFstream::seek(int pos) {
  ifs.seekg(pos);
  ifs.seekp(pos);
}

int TFstream::size() {
  int old = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(old);
  return sz;
}


}
