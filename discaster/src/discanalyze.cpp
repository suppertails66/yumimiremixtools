#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TCdStream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TSort.h"
#include "util/TOpt.h"
#include "discaster/Config.h"
#include "discaster/Object.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>

using namespace std;
using namespace BlackT;
using namespace Discaster;

typedef std::map<int, Object> SectorObjectMap;
SectorObjectMap sectorToObj;

int baseIsoSector = 0;

std::string genFileScrPrefix = "gen";
std::string outputFileScrPrefix = "files";
std::string outputRawFileScrPrefix = "rawfiles";

char syncHeader[]
  = "\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00";
const int syncHeaderLen = sizeof(syncHeader) - 1;



std::string getFixedLenString(TStream& ifs, int size) {
  std::string result;
  ifs.readFixedLenString(result, size);
  return result;
}

std::string getFixedLenStringSpaceStripped(TStream& ifs, int size) {
  std::string result;
  ifs.readFixedLenString(result, size);
  while ((result.size() > 0) && (result[result.size() - 1] == ' '))
    result = result.substr(0, result.size() - 1);
  return result;
}

Object getDirectoryRecord(TStream& ifs) {
  Object result;
  
  int basePos = ifs.tell();
  
  int len = ifs.readu8();
  if (len == 0) return result;
  result.setMember("len", len);
  
  int extAttrRecLen = ifs.readu8();
  if (extAttrRecLen != 0) {
    throw TGenericException(T_SRCANDLINE,
                            "getDirectoryRecord()",
                            "Tried to read extended attribute record");
  }
  result.setMember("extAttrRecLen", extAttrRecLen);
  
  result.setMember("dataLbn", ifs.readu32lebe());
  result.setMember("dataByteSize", ifs.readu32lebe());
  result.setMember("timestamp", getFixedLenString(ifs, 7));
  result.setMember("fileFlags", ifs.readu8());
  result.setMember("fileUnitSize", ifs.readu8());
  result.setMember("interleaveGapSize", ifs.readu8());
  result.setMember("volSeqNum", ifs.readu16lebe());
  
  int nameLen = ifs.readu8();
  result.setMember("nameLen", nameLen);
  result.setMember("name", getFixedLenString(ifs, nameLen));
  if ((nameLen % 2) == 0) ifs.get();
  
  if (ifs.tell() - basePos < len) {
    // assume XA
    int xaBasePos = ifs.tell();
    result.setMember("xa_ownerIdGroup", ifs.readu16be());
    result.setMember("xa_ownerIdUser", ifs.readu16be());
    result.setMember("xa_fileAttr", ifs.readu16be());
    std::string signature = getFixedLenString(ifs, 2);
    if (signature.compare("XA") != 0) {
      throw TGenericException(T_SRCANDLINE,
                              "getDirectoryRecord()",
                              string("Non-XA system use area in directory record")
                                + " at "
                                + TStringConversion::intToString(
                                    ifs.tell(), TStringConversion::baseHex));
    }
    result.setMember("xa_signature", signature);
    result.setMember("xa_fileNum", ifs.readu8());
  }
  
  ifs.seek(basePos + len);
  
  return result;
}

void traverseDirectoryTreeStep(TCdStream& ifs, Object& baseDir) {
//  cerr << "\"" << baseDir.getMemberString("name") << "\"" << endl;

  int sectorNum = baseDir.getMemberInt("dataLbn");
  // note: includes null padding at end of records
  int dataSize = baseDir.getMemberInt("dataByteSize");
  ifs.seekSector(baseIsoSector + sectorNum);
  
  TBufStream recIfs;
  ifs.readBytes(recIfs, dataSize);
  recIfs.seek(0);
  
  // skip self/parent references
  getDirectoryRecord(recIfs);
  getDirectoryRecord(recIfs);
  
  baseDir.setMember("subtree", ArrayObject());
  
  while (!recIfs.eof()) {
    // check length of next record
    if (recIfs.peek() == 0) {
      // if zero, ignore (this can occur if the directory information
      // takes up multiple sectors; each record is required to fit
      // entirely within a single sector, and null padding is used
      // to achieve this if needed)
      recIfs.get();
      continue;
    }
    
    Object record = getDirectoryRecord(recIfs);
    std::string name = record.getMemberString("name");
    baseDir.getMember("subtree").setMember(name, record);
    if ((record.getMemberInt("fileFlags") & 0x02) != 0) {
      // directory: recur
      traverseDirectoryTreeStep(
        ifs, baseDir.getMember("subtree").getMember(name));
    }
    else {
      // regular file
//      cerr << "  \"" << name << "\"" << endl;
      
    }
  }
}

void traverseDirectoryTree(TCdStream& ifs, Object& baseDir) {
  traverseDirectoryTreeStep(ifs, baseDir);
}

void outputLine(ostream& ofs, string content) {
  ofs << content << endl;
}

void outputObjValue(ostream& ofs, Object& obj) {
  if (obj.type() == "string") {
    std::string rawStr = obj.stringValue();
    std::string str;
    for (int i = 0; i < rawStr.size(); i++) {
//      if (rawStr[i] == 0x00) {
//        str += "\\0";
//      }
      if (rawStr[i] < 0x20) {
        str += "\\";
        str += TStringConversion::intToString((unsigned char)rawStr[i]);
      }
      else {
        str += rawStr[i];
      }
    }
    ofs << "\"" << str << "\"";
  }
  else if (obj.type() == "int") {
    ofs << obj.intValue();
  }
  else {
    throw TGenericException(T_SRCANDLINE,
                            "outputObjValue()",
                            string("Tried to output impossible prop type"));
  }
}

void outputProp(ostream& ofs, Object& obj, string name, string propName) {
  ofs << name << "." << propName << " = ";
  
  Object& propObj = obj.getMember(propName);
  outputObjValue(ofs, propObj);
  
  ofs << endl;
}

void outputFuncCall(ostream& ofs, string name, string funcName, Object& obj) {
  ofs << name << "." << funcName << "(";
  
  outputObjValue(ofs, obj);
  
  ofs << ")" << endl;
}

void outputFuncCall(ostream& ofs, string name, string funcName, std::string val) {
  ofs << name << "." << funcName << "(";
  
  ofs << "\"" << val << "\"";
  
  ofs << ")" << endl;
}

void outputFuncCall(ostream& ofs, string name, string funcName, int val) {
  ofs << name << "." << funcName << "(";
  
  ofs << val;
  
  ofs << ")" << endl;
}

void prepFilesystemStep(
//    TCdStream& ifs,
    Object& dirRecord,
//    string outputFileDir,
//    string outputRawFileDir,
    string currentTreeName) {
  dirRecord.setMember("outputName", currentTreeName);
  sectorToObj[dirRecord.getMemberInt("dataLbn")] = dirRecord;
  
  ObjectMemberMap& members = dirRecord.getMember("subtree").members();
  for (ObjectMemberMap::iterator it = members.begin();
       it != members.end();
       ++it) {
    string name = it->first;
    Object& obj = it->second;
    
    std::string outputNameBase = currentTreeName;
    if (currentTreeName != "") outputNameBase += "/";
    
    if (obj.getMemberInt("fileFlags") & 0x02) {
      // directory
      // recur
      prepFilesystemStep(obj,
        outputNameBase + name);
    }
    else {
      // scrub version numbers from filename
      std::string physicalFileName = outputNameBase + name;
      for (int i = 0; i < physicalFileName.size(); i++) {
        if (physicalFileName[i] == ';') {
          physicalFileName = physicalFileName.substr(0, i);
          break;
        }
      }
      
      obj.setMember("outputName", physicalFileName);
      sectorToObj[obj.getMemberInt("dataLbn")] = obj;
    }
  }
}

void prepFilesystem(Object& rootDirRecord) {
  prepFilesystemStep(rootDirRecord,
    "");
}



int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "discanalyze: discaster script creation tool" << std::endl;
    std::cout << "Usage: "
      << argv[0] << " <discfile> <outdir> [options]" << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << "  -w       Suppress warnings" << std::endl;
    std::cout << "  -d       Enable debug output" << std::endl;
    
    return 0;
  }
  
  std::string inputFile(argv[1]);
  std::string outDir(argv[2]);
  
  if (TOpt::hasFlag(argc, argv, "-w")) config.setWarningsOn(false);
  if (TOpt::hasFlag(argc, argv, "-d")) config.setDebugOutput(true);

  double timer = clock();
  
  TCdStream ifs;
  ifs.open(inputFile.c_str());
  
  TFileManip::createDirectory(outDir);
  std::string genFileDir = outDir + "/" + genFileScrPrefix;
  TFileManip::createDirectory(genFileDir);
  
  std::string outputFileDir = outDir + "/" + outputFileScrPrefix;
  TFileManip::createDirectory(outputFileDir);
  
  std::string outputRawFileDir = outDir + "/" + outputRawFileScrPrefix;
  TFileManip::createDirectory(outputRawFileDir);
  
  std::ofstream scrOfs(outDir + "/script_template.dsc");
  
  //============================
  // detect default sector mode
  //============================
  
  TCdStream::SectorMode defaultSectorMode;
  ifs.setSectorMode(TCdStream::raw);
  {
    ifs.seekSector(baseIsoSector);
    ifs.seekoff(15);
    int mode = ifs.readu8();
    
    if (mode == 0) {
      std::cerr << "Error: mode 0 sector" << std::endl;
      return 1;
    }
    else if (mode == 1) {
      defaultSectorMode = TCdStream::mode1;
    }
    else if (mode == 2) {
      ifs.seekoff(2);
      int submode = ifs.readu8();
      if ((submode & 0x20) != 0) {
        defaultSectorMode = TCdStream::mode2form2;
      }
      else {
        defaultSectorMode = TCdStream::mode2form1;
      }
    }
    else {
      std::cerr << "Error: unknown sector mode " << mode << std::endl;
      return 1;
    }
  }
  
  //============================
  // volume descriptors
  //============================
  
  ifs.setSectorMode(defaultSectorMode);
  
  BlobObject isoObj;
  ArrayObject discStructureArray;
  BlobObject pDesc("PrimaryVolDescriptor");
  
  for (int i = 0; ; i++) {
    TBufStream volIfs;
    ifs.seekSector(baseIsoSector + 16 + i);
    ifs.readSectors(volIfs, 1);
    
    volIfs.seek(0);
    int type = volIfs.readu8();
    
    if (type == 0xFF) {
      discStructureArray.arrayValue().push_back(
        BlobObject("VolDescSetTerminator"));
      break;
    }
    else if (type == 0x01) {
      
      volIfs.seek(0x008);
      pDesc.setMember("systemIdentifier",
        getFixedLenStringSpaceStripped(volIfs, 32));
//      std::cerr << "\"" << result << "\"" << std::endl;
      pDesc.setMember("volumeIdentifier",
        getFixedLenStringSpaceStripped(volIfs, 32));
      
      volIfs.seek(0x084);
      pDesc.setMember("pathTableSize",
        volIfs.readu32lebe());
      pDesc.setMember("pathTable1Lbn",
        volIfs.readu32le());
      pDesc.setMember("pathTable2Lbn",
        volIfs.readu32le());
      pDesc.setMember("pathTable3Lbn",
        volIfs.readu32be());
      pDesc.setMember("pathTable4Lbn",
        volIfs.readu32be());
      
      volIfs.seek(0x09C);
      pDesc.setMember("rootDirRecord",
        getDirectoryRecord(volIfs));
      
      traverseDirectoryTree(ifs, pDesc.getMember("rootDirRecord"));
      
      volIfs.seek(0x0BE);
      pDesc.setMember("volumeSetIdentifier",
        getFixedLenStringSpaceStripped(volIfs, 128));
      pDesc.setMember("publisherIdentifier",
        getFixedLenStringSpaceStripped(volIfs, 128));
      pDesc.setMember("dataPreparerIdentifier",
        getFixedLenStringSpaceStripped(volIfs, 128));
      pDesc.setMember("applicationIdentifier",
        getFixedLenStringSpaceStripped(volIfs, 128));
      pDesc.setMember("copyrightFileIdentifier",
        getFixedLenStringSpaceStripped(volIfs, 37));
      pDesc.setMember("abstractFileIdentifier",
        getFixedLenStringSpaceStripped(volIfs, 37));
      pDesc.setMember("bibliographicFileIdentifier",
        getFixedLenStringSpaceStripped(volIfs, 37));
      pDesc.setMember("volumeCreationTime",
        getFixedLenString(volIfs, 17));
      pDesc.setMember("volumeModificationTime",
        getFixedLenString(volIfs, 17));
      pDesc.setMember("volumeExpirationTime",
        getFixedLenString(volIfs, 17));
      pDesc.setMember("volumeEffectiveTime",
        getFixedLenString(volIfs, 17));
      
      volIfs.seek(0x400);
      string xaSigStr = getFixedLenString(volIfs, 8);
      if (xaSigStr.compare("CD-XA001") == 0) {
        isoObj.setMember("format", "CDXA");
      }
      else {
        isoObj.setMember("format", "CDROM");
      }
      
//      discStructureArray.arrayValue().push_back(pDesc);
    }
    else {
      cerr << "Unhandled volume descriptor type: " << type << endl;
      return 1;
    }
  }
  
  //============================
  // system area
  //============================
  
  {
//    ifs.setSectorMode(defaultSectorMode);
    ifs.setSectorMode(TCdStream::raw);
    TBufStream ofs;
    ifs.seekSector(baseIsoSector);
    ifs.readSectors(ofs, 16);
    ofs.save((genFileDir + "/sysarea.bin").c_str());
    
//    scrOfs << "isoFs.setRawSectorSystemArea(\""
//      << (genFileScrPrefix + "sysarea.bin")
//      << "\")" << endl << endl;
  }
  
  //============================
  // output descriptor info
  //============================
  
  outputLine(scrOfs, "IsoPrimaryVolumeDescriptor pDesc");
    outputProp(scrOfs, pDesc, "pDesc", "systemIdentifier");
    outputProp(scrOfs, pDesc, "pDesc", "volumeIdentifier");
    outputProp(scrOfs, pDesc, "pDesc", "volumeSetIdentifier");
    outputProp(scrOfs, pDesc, "pDesc", "publisherIdentifier");
    outputProp(scrOfs, pDesc, "pDesc", "dataPreparerIdentifier");
    outputProp(scrOfs, pDesc, "pDesc", "applicationIdentifier");
    outputProp(scrOfs, pDesc, "pDesc", "copyrightFileIdentifier");
    outputProp(scrOfs, pDesc, "pDesc", "abstractFileIdentifier");
    outputProp(scrOfs, pDesc, "pDesc", "bibliographicFileIdentifier");
    outputProp(scrOfs, pDesc, "pDesc", "volumeCreationTime");
    outputProp(scrOfs, pDesc, "pDesc", "volumeModificationTime");
    outputProp(scrOfs, pDesc, "pDesc", "volumeExpirationTime");
    outputProp(scrOfs, pDesc, "pDesc", "volumeEffectiveTime");
  outputLine(scrOfs, "");
  
  // TODO
//  outputLine(scrOfs, "IsoDescriptorSetTerminator descTerminator");
//  outputLine(scrOfs, "");
  
  //============================
  // set up iso fs
  //============================
  
  outputLine(scrOfs, "IsoFilesystem isoFs");
    outputFuncCall(scrOfs, "isoFs", "setFormat",
                   isoObj.getMember("format"));
    outputFuncCall(scrOfs, "isoFs", "setRawSectorSystemArea",
                   (genFileScrPrefix + "/" + "sysarea.bin"));
    outputFuncCall(scrOfs, "isoFs", "importDirectoryListing",
                   (outputFileScrPrefix));
  outputLine(scrOfs, "");
  
  //============================
  // pass 1: generate files and metadata
  //============================
  
  prepFilesystem(pDesc.getMember("rootDirRecord"));
  
  for (SectorObjectMap::iterator it = sectorToObj.begin();
       it != sectorToObj.end();
       ++it) {
    int lbn = it->first;
    Object& obj = it->second;
//    cerr << lbn << " " << obj.getMemberString("outputName") << endl;
    
    std::string outputName = obj.getMemberString("outputName");
    int fileSize = obj.getMemberInt("dataByteSize");

    // if a directory, skip
    bool isDirectory = false;
    if ((obj.getMemberInt("fileFlags") & 0x02) != 0) {
//      isDirectory = true;
      continue;
    }

    if (obj.hasMember("xa_fileAttr")) {
      int xaAttr = obj.getMemberInt("xa_fileAttr");
      // if flagged as CDDA, warn and skip
      if ((xaAttr & 0x4000) != 0) {
        cout << "NOTE: file \"" << outputName << "\" appears to be a"
          " CD audio pointer and will need manual handling" << endl;
        continue;
      }
    }
    
    // we detect mode by directly checking the target sectors on disc,
    // as games tend to do whatever the hell they want with the XA
    // mode/form flags in the directory record
    TCdStream::SectorMode mode;
    
    ifs.setSectorMode(TCdStream::raw);
    ifs.seekSector(baseIsoSector + lbn);
    
//    std::cerr << "file: " << outputName << " " << lbn << " " << fileSize << std::endl;
    
    // first, verify that the sync data in the header exists
    {
      string headerStr;
      ifs.readFixedLenString(headerStr, syncHeaderLen);
      
      if (memcmp(headerStr.c_str(), syncHeader, syncHeaderLen) != 0) {
        std::cerr << "Warning: no header sync data at start of file?"
            << std::endl;
        std::cerr << outputName << " " << lbn << " " << fileSize << std::endl;
        std::cerr << "guessing this is CDDA and skipping" << std::endl;
        continue;
      }
    }
    
//    ifs.seekoff(15);
    ifs.seekoff(3);
    int rawmode = ifs.readu8();
    
    if (rawmode == 0) {
      std::cerr << "Warning: mode 0 sector in file?" << std::endl;
      std::cerr << outputName << " " << lbn << " " << fileSize << std::endl;
//      return 1;
      std::cerr << "guessing this is CDDA and skipping" << std::endl;
      continue;
    }
    else if (rawmode == 1) {
      mode = TCdStream::mode1;
    }
    else if (rawmode == 2) {
      ifs.seekoff(2);
      int submode = ifs.readu8();
      if ((submode & 0x20) != 0) {
        mode = TCdStream::mode2form2;
      }
      else {
        mode = TCdStream::mode2form1;
      }
      
      // if submode has real-time set, assume raw needed
      if ((submode & 0x40) != 0) mode = TCdStream::raw;
    }
    else {
      std::cerr << "Error: unknown sector mode "
        << rawmode
        << " at "
        << TStringConversion::intToString(ifs.tell(),
              TStringConversion::baseHex)
        << std::endl;
      return 1;
    }

    // if flagged as XA interleave, assume raw needed
    // (checking only the first sector of the file is not sufficient,
    // because different sectors of an interleave can and do use
    // different modes)
    if (obj.hasMember("xa_fileAttr")) {
      int xaAttr = obj.getMemberInt("xa_fileAttr");
      // if flagged as CDDA, warn and skip
      if ((xaAttr & 0x2000) != 0) {
        mode = TCdStream::raw;
      }
    }
    
    std::string outputFileBase;
    string refFileBase;
    bool dumpedAsRaw = false;
    
//    if (!isDirectory) {
      TBufStream ofs;
      
      if ((mode == TCdStream::mode1) || (mode == TCdStream::mode2form1)) {
        // dump as standard file
        ifs.setSectorMode(mode);
        refFileBase = outputFileScrPrefix;
        outputFileBase = outputFileDir;
        dumpedAsRaw = false;
      }
      else {
        // dump as raw
        // it appears that mode2form2 files list their filesize
        // as if they consisted of 0x800-byte sectors, even though
        // they actually have 0x914 bytes per sector, so we don't
        // have to do a separate adjustment for that case
        fileSize = ((fileSize + 0x7FF) / 0x800) * 0x930;
        
        ifs.setSectorMode(TCdStream::raw);
        refFileBase = outputRawFileScrPrefix;
        outputFileBase = outputRawFileDir;
        dumpedAsRaw = true;
      }
      
      std::string copiedFileName = outputFileBase + "/" + outputName;
      std::string refFileName = refFileBase + "/" + outputName;
      
  //    cerr << copiedFileName << " " << lbn << " " << fileSize << endl;
      
      ifs.seekSector(baseIsoSector + lbn);
      ifs.readBytes(ofs, fileSize);

      TFileManip::createDirectoryForFile(copiedFileName);
      ofs.save(copiedFileName.c_str());
      
      if (dumpedAsRaw) {
        outputLine(scrOfs,
          string("isoFs.importRawSectorFile(\"")
            + refFileName
            + "\", "
            + "\""
            + outputName
            + "\")");
      }
//    }
    
//    outputLine(scrOfs, "");
  }
  
  //============================
  // pass 2
  //============================
  
  for (SectorObjectMap::iterator it = sectorToObj.begin();
       it != sectorToObj.end();
       ++it) {
    Object& obj = it->second;
    std::string outputName = obj.getMemberString("outputName");
    
    //=======
    // set up any necessary properties
    //=======
    
    std::string listedFileStr = string("isoFs.getListedFile(")
      + "\"" + outputName + "\""
      + ")";

    bool hasProp = false;
    if (obj.hasMember("xa_fileAttr")) {
      int xaAttr = obj.getMemberInt("xa_fileAttr");
      // cdda
      if ((xaAttr & 0x4000) != 0) {
        outputLine(scrOfs, listedFileStr + ".xa_flags_isCdda = 1");
        hasProp = true;
      }
      // interleave
      if ((xaAttr & 0x2000) != 0) {
        outputLine(scrOfs, listedFileStr + ".xa_flags_isInterleaved = 1");
        hasProp = true;
      }
      // m2f2
      if ((xaAttr & 0x1000) != 0) {
        outputLine(scrOfs, listedFileStr + ".xa_flags_isMode2Form2 = 1");
        hasProp = true;
      }
      // m2
      if ((xaAttr & 0x0800) != 0) {
        outputLine(scrOfs, listedFileStr + ".xa_flags_isMode2 = 1");
        hasProp = true;
      }
      
      int xaFileNum = obj.getMemberInt("xa_fileNum");
      if (xaFileNum != 0) {
        outputLine(scrOfs, listedFileStr + ".xa_fileNumber = "
          + TStringConversion::intToString(xaFileNum));
        hasProp = true;
      }
    }
    
    if (hasProp) outputLine(scrOfs, "");
  }
  
  //============================
  // pass 3: generate listing commands
  //============================
  
  outputLine(scrOfs, "isoFs.addPrimaryVolumeDescriptor(pDesc)");
  outputLine(scrOfs, "isoFs.addDescriptorSetTerminator()");
  if (pDesc.getMemberInt("pathTable1Lbn") != 0)
    outputLine(scrOfs, "isoFs.addTypeLPathTable()");
  if (pDesc.getMemberInt("pathTable2Lbn") != 0)
    outputLine(scrOfs, "isoFs.addTypeLPathTableCopy()");
  if (pDesc.getMemberInt("pathTable3Lbn") != 0)
    outputLine(scrOfs, "isoFs.addTypeMPathTable()");
  if (pDesc.getMemberInt("pathTable4Lbn") != 0)
    outputLine(scrOfs, "isoFs.addTypeMPathTableCopy()");
  outputLine(scrOfs, "");
  
  for (SectorObjectMap::iterator it = sectorToObj.begin();
       it != sectorToObj.end();
       ++it) {
    Object& obj = it->second;
    
    std::string outputName = obj.getMemberString("outputName");
    int fileSize = obj.getMemberInt("dataByteSize");

    // if a directory
    if ((obj.getMemberInt("fileFlags") & 0x02) != 0) {
      outputLine(scrOfs, string("  isoFs.addDirectoryDescriptor(\"")
        + outputName
        + "\")");
    }
    else {
      outputLine(scrOfs, string("    isoFs.addListedFile(\"")
        + outputName
        + "\")");
    }
  }
  
  outputLine(scrOfs, "");
  
  outputLine(scrOfs, "CdImage cd");
  outputLine(scrOfs, "");
  
  outputLine(scrOfs, "// *** FILL IN TRACK LAYOUT DATA HERE ***");
  outputLine(scrOfs, "");
  
  outputLine(scrOfs, "// DEBUG");
  outputLine(scrOfs, "//cd.disableEdcCalculation = 1");
  outputLine(scrOfs, "//cd.disableEccCalculation = 1");
  outputLine(scrOfs, "");
  
  outputLine(scrOfs, "cd.exportBinCue(\"BASENAME_GOES_HERE\")");
  outputLine(scrOfs, "");
  
  for (SectorObjectMap::iterator it = sectorToObj.begin();
       it != sectorToObj.end();
       ++it) {
    Object& obj = it->second;
    
    std::string outputName = obj.getMemberString("outputName");
    int fileSize = obj.getMemberInt("dataByteSize");

    // if a directory
    if ((obj.getMemberInt("fileFlags") & 0x02) != 0) {
      
    }
    else {
      for (int i = outputName.size() - 1; i >= 0; i--) {
        if (outputName[i] == '/') {
          outputName = outputName.substr(i + 1, string::npos);
          break;
        }
      }
      
//      cout << outputName << " " << (fileSize + 0x7FF) / 0x800 << " " << it->first << std::endl;
    }
  }
  
//  if (!discStructureArray.hasMember("primaryVolDesc")) {
//    
//  }

/*  try {
    Discaster::ScriptLexer lexer;
    Discaster::LexicalStream lexStream;
    std::ifstream ifs(inputFile.c_str(), ios_base::binary);
    Discaster::StdInputStream inputStream(ifs);
    
    lexer.lexProgram(lexStream, inputStream);
  //  while (!lexStream.eof()) std::cout << lexStream.get();
    
    ScriptParser parser;
    parser.parseProgram(lexStream);
//    parser.parseTree().prettyPrint(std::cout);

    ScriptEvaluator evaluator;
    evaluator.evaluateProgram(parser.parseTree());
  }
  catch (BlackT::TGenericException& e) {
    std::cout << "Exception:" << std::endl
      << e.problem() << std::endl;
    return 1;
  } */
  
  
  
  timer = clock() - timer;
  std::cout << "Running time: " << timer/(double)CLOCKS_PER_SEC
    << " sec. " << std::endl;
  
  return 0;
}
