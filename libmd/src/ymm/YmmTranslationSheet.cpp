#include "ymm/YmmTranslationSheet.h"
//#include "pom/PomCmp.h"
//#include "pom/PomPak.h"
//#include "pom/PomPlmStringScanException.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TByte.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include <vector>
#include <map>
#include <cctype>
#include <iostream>
#include <fstream>

using namespace BlackT;

namespace Md {


void YmmTranslationSheet::addStringEntry(std::string id, std::string content,
                    std::string prefix, std::string suffix,
                    std::string translationPlaceholder) {
  YmmTranslationSheetEntry entry;
  entry.type = YmmTranslationSheetEntry::type_string;
  entry.stringId = id;
  entry.stringContent = content;
  entry.stringPrefix = prefix;
  entry.stringSuffix = suffix;
  entry.translationPlaceholder = translationPlaceholder;
  
  entries.push_back(entry);
  stringEntryMap[id] = entry;
}

void YmmTranslationSheet::addCommentEntry(std::string comment) {
  YmmTranslationSheetEntry entry;
  entry.type = YmmTranslationSheetEntry::type_comment;
  entry.stringContent
    = std::string("//==================================\n")
      + "// " + comment
      + std::string("\n//==================================");
  
  entries.push_back(entry);
}

void YmmTranslationSheet::addMarkerEntry(std::string content) {
  YmmTranslationSheetEntry entry;
  entry.type = YmmTranslationSheetEntry::type_marker;
  entry.stringId = content;
  entries.push_back(entry);
}

bool YmmTranslationSheet::hasStringEntryForId(std::string id) const {
  return (stringEntryMap.find(id) != stringEntryMap.end());
}

const YmmTranslationSheetEntry& YmmTranslationSheet
    ::getStringEntryById(std::string id) const {
  return stringEntryMap.at(id);
}

void YmmTranslationSheet::importCsv(std::string filename) {
  TCsv csv;
  {
    TBufStream ifs;
    ifs.open(filename.c_str());
//    csv.readSjis(ifs);
    csv.readUtf8(ifs);
  }
  
  for (int j = 0; j < csv.numRows(); j++) {
    std::string type = csv.cell(0, j);
    if (type.compare("string") == 0) {
      std::string id = csv.cell(1, j);
      std::string prefix = csv.cell(2, j);
      std::string suffix = csv.cell(3, j);
      std::string content = csv.cell(5, j);
      addStringEntry(id, content, prefix, suffix);
    }
  }
}
  
void YmmTranslationSheet
  ::outputQuotedIfNotEmpty(std::ostream& ofs, std::string content) const {
  if (!content.empty()) {
    ofs << "\"" << content << "\"";
  }
}

void YmmTranslationSheet::exportCsv(std::string filename,
                      YmmTranslationSheet::LinkData* linkData) const {
  LinkData newLinkData;
  if (linkData == NULL) {
    // do "dummy" export to get link data
    exportCsv(filename, &newLinkData);
    linkData = &newLinkData;
  }
  
  std::ofstream ofs(filename.c_str(), std::ios_base::binary);

  // map of string content to first cell containing that content,
  // for generating cell equates for duplicate strings
  std::map<std::string, std::string> stringMap;
  std::map<std::string, std::string> stringMapPrefix;
  std::map<std::string, std::string> stringMapSuffix;
  
  std::map<std::string, std::string> boxMap;
  
  int rowNum = 0;
  bool inDictSection = false;
  for (YmmTranslationSheetEntryCollection::const_iterator it
        = entries.cbegin();
       it != entries.cend();
       ++it) {
    const YmmTranslationSheetEntry& entry = *it;
    switch (entry.type) {
    case YmmTranslationSheetEntry::type_string:
    {
      // col A = type
      ofs << "\"string\"";
      
      // col B = id
      ofs << ",";
//      ofs << "\"" << entry.stringId << "\"";
      outputQuotedIfNotEmpty(ofs, entry.stringId);
      
      // col C = prefix
      ofs << ",";
//      ofs << "\"" << entry.stringPrefix << "\"";
      if (!entry.stringPrefix.empty()) {
        std::map<std::string, std::string>::iterator findIt
          = stringMapPrefix.find(entry.stringPrefix);
        
        if (findIt != stringMapPrefix.end()) {
          ofs << "\"=T(" << findIt->second << ")\"";
        }
        else {
//          ofs << "\"" << entry.stringPrefix << "\"";
          outputQuotedIfNotEmpty(ofs, entry.stringPrefix);
          stringMapPrefix[entry.stringPrefix]
            = TCsv::coordinatePosToCellId(prefixContentColNum, rowNum);
        }
      }
      
      // col D = suffix
      ofs << ",";
//      ofs << "\"" << entry.stringSuffix << "\"";
      if (!entry.stringSuffix.empty()) {
        std::map<std::string, std::string>::iterator findIt
          = stringMapSuffix.find(entry.stringSuffix);
        
        if (findIt != stringMapSuffix.end()) {
          ofs << "\"=T(" << findIt->second << ")\"";
        }
        else {
//          ofs << "\"" << entry.stringSuffix << "\"";
          outputQuotedIfNotEmpty(ofs, entry.stringSuffix);
          stringMapSuffix[entry.stringSuffix]
            = TCsv::coordinatePosToCellId(suffixContentColNum, rowNum);
        }
      }
      
      // col E = content
      ofs << ",";
      std::map<std::string, std::string>::iterator findIt
        = stringMap.find(entry.stringContent);
      // to simplify clutter a bit: pretend null strings are not repeats
      if (findIt != stringMap.end()) {
        if (entry.stringContent.empty()) findIt = stringMap.end();
      }
      
      if (findIt != stringMap.end()) {
//        ofs << "\"=T(" << findIt->second << ")\"";
//        ofs << "\"" << entry.stringContent << "\"";
        outputQuotedIfNotEmpty(ofs, entry.stringContent);
      }
      else {
//        ofs << "\"" << entry.stringContent << "\"";
//        stringMap[entry.stringContent]
//          = TCsv::coordinatePosToCellId(translationContentColNum, rowNum);
        
        // if not linked, produce link data
        if (!linkData->linked) {
//          ofs << "\"" << entry.stringContent << "\"";
          outputQuotedIfNotEmpty(ofs, entry.stringContent);
          
          // add to dictionary if enabled
          if (inDictSection && (!entry.stringContent.empty())) {
            DictEntry dictEntry;
            dictEntry.content = entry.stringContent;
            dictEntry.rowNum = rowNum;
//            std::cerr << dictEntry.content << " " << dictEntry.rowNum << std::endl;
            linkData->dictEntries[dictEntry.content] = dictEntry;
          }
        }
        // if linked, check for dictionary substitution
        else {
          // of course, don't link dictionary entries to themselves
          if (inDictSection) {
//            ofs << "\"" << entry.stringContent << "\"";
            outputQuotedIfNotEmpty(ofs, entry.stringContent);
          }
          else {
//            ofs << "\"" << getDictionariedString(*linkData, entry.stringContent)
//                << "\"";
//            ofs << "\"" << entry.stringContent << "\"";
            outputQuotedIfNotEmpty(ofs, entry.stringContent);
          }
        }
        
        stringMap[entry.stringContent]
          = TCsv::coordinatePosToCellId(translationContentColNum, rowNum);
      }
      
      // col F = translation placeholder
      ofs << ",";
      if (findIt != stringMap.end()) {
        ofs << "\"=T(" << findIt->second << ")\"";
      }
      else {
//        ofs << "\"\"";

//        ofs << "\""
//          << entry.translationPlaceholder
//          << "\"";
        
        // zenki says this the original text is more useful
        // than the placeholders i went to so much trouble to generate :(
//        ofs << "\""
//          << entry.stringContent
////          << getQuoteSubbedString(entry.stringContent)
//          << "\"";

//        ofs << "\""
//          << "\"";
        // lol
        outputQuotedIfNotEmpty(ofs, "");
      }
      
      // col G = comment
      ofs << ",";
      if (findIt != stringMap.end()) {
        ofs << "\"DUPE: " << findIt->second << "\"";
      }
      else {
        // box duplication check
/*        std::map<std::string, std::string>::iterator boxFindIt
          = findBoxDuplicate(boxMap, entry.stringContent);
        if (boxFindIt != boxMap.end()) {
          ofs << "\"PARTIAL DUPE: " << boxFindIt->second << "\"";
        }
        addBoxDuplicates(boxMap, entry.stringContent,
          TCsv::coordinatePosToCellId(translationContentColNum, rowNum)); */
      }
      
      // col H = filled check
      ofs << ",";
      if (findIt == stringMap.end()) {
//        ofs << "\"=IF(F" << rowNum+1 << "=\"\"\"\""
//          << ", \"\"UNTRANSLATED\"\", \"\"\"\")\"";
        ofs << "\"=IF(E" << rowNum+1 << "=\"\"\"\""
          << ", \"\"untranscribed\"\", IF(F" << rowNum+1
          << "=\"\"\"\", \"\"UNTRANSLATED\"\", \"\"\"\")\"";
      }
      
      // col I = filled check value
      ofs << ",";
      if (findIt == stringMap.end()) {
//        ofs << "\""
//          << entry.translationPlaceholder
//          << "\"";

//        ofs << "\""
//          << entry.stringContent
////          << getQuoteSubbedString(entry.stringContent)
//          << "\"";
        
//        ofs << "\""
//          << "\"";
        outputQuotedIfNotEmpty(ofs, "");
      }
      
      ofs << std::endl;
      ++rowNum;
    }
      break;
    case YmmTranslationSheetEntry::type_comment:
    {
      // col A = type (blank to ignore)
      ofs << "";
      
      // col B
      ofs << ",";
      ofs << "";
      
      // col C
      ofs << ",";
      ofs << "";
      
      // col D
      ofs << ",";
      ofs << "";
      
      // col E
      ofs << ",";
//      ofs << "\"" << entry.stringContent << "\"";
      outputQuotedIfNotEmpty(ofs, entry.stringContent);
      
      // col F
      ofs << ",";
      ofs << "";
      
      // col G
      ofs << ",";
      ofs << "";
      
      // col H
      ofs << ",";
      ofs << "";
      
      // col I
      ofs << ",";
      ofs << "";
      
      ofs << std::endl;
      ++rowNum;
    }
      break;
    case YmmTranslationSheetEntry::type_marker:
    {
      if (entry.stringId.compare("dict_section_start") == 0) {
        inDictSection = true;
      }
      else if (entry.stringId.compare("dict_section_end") == 0) {
        inDictSection = false;
      }
      else {
        throw TGenericException(T_SRCANDLINE,
                                "YmmTranslationSheet::exportCsv()",
                                "unknown marker: "
                                  + entry.stringId);
      }
    }
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "YmmTranslationSheet::exportCsv()",
                              "illegal type");
      break;
    }
  }
  
  if (!linkData->linked) {
    linkData->linked = true;
  }
}

int YmmTranslationSheet::numEntries() const {
  return entries.size();
}

YmmTranslationSheet::LinkData::LinkData()
  : linked(false) { }


}
