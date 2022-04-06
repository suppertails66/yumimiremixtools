#include "util/TThingyTable.h"
#include "util/TStringConversion.h"
#include "util/TCharFmt.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cctype>

using namespace std;

namespace BlackT {


TThingyTable::TThingyTable() { }

TThingyTable::TThingyTable(string filename) {
  readGeneric(filename);
}
  
void TThingyTable::readGeneric(std::string filename) {
  ifstream ifs(filename.c_str(), ios_base::binary);
  
  while (ifs.good()) {
    string numberString;
    
    // skip whitespace
    while (ifs.good() && isspace(ifs.peek())) ifs.get();
    
    // ignore comment lines
    char next = ifs.get();
    if ((next == '/') && (ifs.peek() == '/')) {
      getline(ifs, numberString);
      continue;
    }
    else {
      ifs.unget();
    }
    
    getline(ifs, numberString, '=');
    if (!ifs.good()) {
      break;
    }
    
    // add hex ID for int conversion
    numberString = string("0x") + numberString;
    int rawID = TStringConversion::stringToInt(numberString);
    
    string charString;
    getline(ifs, charString);
    
    // motherfucking windows
    while (charString.size() >= 1
            && ((charString[charString.size() - 1] == '\n')
            || (charString[charString.size() - 1] == '\r'))) {
      charString = charString.substr(0, charString.size() - 1);
    }
    
    // ignore null entries
    if (charString.size() <= 0) continue;
    
//    std::cerr << numberString << " " << charString << " " << rawID << std::endl;
    entries[rawID] = TableEntry(charString);
  }
  
  generateEntriesByLength();
  
//  std::cerr << matchTableEntry(std::string("[bigzero]")).id << std::endl;
}
  
void TThingyTable::readSjis(std::string filename) {
  readGeneric(filename);
}
  
void TThingyTable::readUtf8(std::string filename) {
  ifstream ifs(filename.c_str(), ios_base::binary);
  
  while (ifs.good()) {
    string numberString;
    
    // skip whitespace
    while (ifs.good() && isspace(ifs.peek())) ifs.get();
    
    // ignore comment lines
    char next = ifs.get();
    if ((next == '/') && (ifs.peek() == '/')) {
      getline(ifs, numberString);
      continue;
    }
    else {
      ifs.unget();
    }
    
    getline(ifs, numberString, '=');
    if (!ifs.good()) {
      break;
    }
    
    // add hex ID for int conversion
    numberString = string("0x") + numberString;
    int rawID = TStringConversion::stringToInt(numberString);
    
    string charString;
    getline(ifs, charString);
    
    // motherfucking windows
    while (charString.size() >= 1
            && ((charString[charString.size() - 1] == '\n')
            || (charString[charString.size() - 1] == '\r'))) {
      charString = charString.substr(0, charString.size() - 1);
    }
    
    // ignore null entries
    if (charString.size() <= 0) continue;
    
    entries[rawID] = TableEntry(charString);
    
    TUtf16Chars chars;
    TCharFmt::utf8To16(charString, chars);
    if (chars.size() > 0) {
      revEntries[chars[0]] = rawID;
    }
  }
  
  generateEntriesByLength();
}

string TThingyTable::getEntry(int charID) const {
  RawTable::const_iterator it = entries.find(charID);
  
  if (it == entries.cend()) {
//    return string("?");
    std::string str = TStringConversion::intToString(charID,
                        TStringConversion::baseHex).substr(2,
                                std::string::npos);
    while (str.size() < 2) str = "0" + str;
    return ("<$" + str + ">");
  }
  
  return it->second;
}
  
int TThingyTable::getRevEntry(int charID) const {
  ReverseMap::const_iterator it = revEntries.find(charID);
  
  if (it == revEntries.cend()) {
    return -1;
  }
  
  return it->second;
}

TThingyTable::MatchResult
    TThingyTable::matchTableEntry(TableEntry entry) const {
  MatchResult result;
  result.id = -1;
  result.size = -1;
  
  for (LengthCollection::const_iterator it = entriesByLength.cbegin();
       it != entriesByLength.cend();
       ++it) {
    if (it->entry.size() > entry.size()) continue;
    
    TableEntry str = entry.substr(0, it->entry.size());
    if (it->entry.compare(str) == 0) {
      result.id = it->id;
      result.size = it->entry.size();
      break;
    }
  }
  
  return result;
}

TThingyTable::MatchResult TThingyTable::matchTableEntry(TStream& ifs) const {
  MatchResult result;
  result.id = -1;
  result.size = -1;
  
  for (LengthCollection::const_iterator it = entriesByLength.cbegin();
       it != entriesByLength.cend();
       ++it) {
    if (it->entry.size() > ifs.remaining()) continue;
    
    int pos = ifs.tell();
    
    TableEntry str;
    for (int i = 0; i < it->entry.size(); i++) {
      str += ifs.get();
    }
    
    if (it->entry.compare(str) == 0) {
//      return IdLengthPair(it->id, it->entry.size());
      result.id = it->id;
      result.size = it->entry.size();
      break;
    }
    
    ifs.seek(pos);
  }
  
  return result;
}

TThingyTable::MatchResult TThingyTable::matchId(TStream& ifs,
                                                int bytesPerChar) const {
  MatchResult result;
  result.id = -1;
  result.size = -1;
  
  for (LengthCollection::const_iterator it = entriesByIdLength.cbegin();
       it != entriesByIdLength.cend();
       ++it) {
//    int byteLen = (it->id / 0x100) + 1;
    
//    int check = it->id;
//    int byteLen = 1;
//    check >>= 8;
//    while (check != 0) {
//      ++byteLen;
//      check >>= 8;
//    }
    
    int byteLen;
    if (bytesPerChar != -1) byteLen = bytesPerChar;
    else {
      if (it->id <= 0xFF) byteLen = 1;
      else if (it->id <= 0xFFFF) byteLen = 2;
      else if (it->id <= 0xFFFFFF) byteLen = 3;
      else byteLen = 4;
    }
    
    if (byteLen > ifs.remaining()) continue;
    
    int pos = ifs.tell();
    int value = ifs.readInt(byteLen,
                            EndiannessTypes::big, SignednessTypes::nosign);
    if (value == it->id) {
      result.id = it->id;
      result.size = it->entry.size();
      break;
    }
    
    ifs.seek(pos);
  }
  
  return result;
}
  
bool TThingyTable::hasEntry(int charID) const {
  RawTable::const_iterator it = entries.find(charID);
  
  if (it == entries.cend()) {
    return false;
  }
  
  return true;
}

void TThingyTable::generateEntriesByLength() {
  entriesByLength.resize(entries.size());
  entriesByIdLength.resize(entries.size());
  int pos = 0;
  for (RawTable::iterator it = entries.begin();
       it != entries.end();
       ++it) {
    entriesByLength[pos].entry = it->second;
    entriesByLength[pos].id = it->first;
    
    entriesByIdLength[pos].entry = it->second;
    entriesByIdLength[pos].id = it->first;
    
    ++pos;
  }
  
  std::sort(entriesByLength.begin(), entriesByLength.end(), sortByLength);
  std::sort(entriesByIdLength.begin(), entriesByIdLength.end(),
            sortByIdLength);
}

bool TThingyTable::sortByLength(const TableEntryInfo& first,
                         const TableEntryInfo& second) {
  if (second.entry.size() < first.entry.size()) return true;
  return false;
}

bool TThingyTable::sortByIdLength(const TableEntryInfo& first,
                           const TableEntryInfo& second) {
  if (second.id < first.id) return true;
  return false;
}


} 
