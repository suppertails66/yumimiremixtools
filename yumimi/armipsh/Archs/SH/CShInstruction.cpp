#include "stdafx.h"
#include "CShInstruction.h"
#include "Core/Common.h"
#include "Sh.h"
#include "Core/FileManager.h"
#include <cstddef>
#include <cmath>

/*const char ShConditions[16][3] = {
	"eq","ne","cs","cc","mi","pl","vs","vc","hi","ls","ge","lt","gt","le","","nv"
};
const char ShAddressingModes[4][3] = { "da","ia","db","ib"};
const char ShShiftModes[4][4] = { "lsl", "lsr", "asr", "ror" };

const char ShPsrModes[16][5] = {
	"_???",	"_ctl",	"_x",	"_xc",	"_s",	"_sc",	"_sx",	"_sxc",
	"_flg",	"_fc",	"_fx",	"_fxc",	"_fs",	"_fsc",	"_fsx",	""
};*/

bool CShInstruction::Load(char *Name, char *Params)
{
	return false;
}

CShInstruction::CShInstruction(const tShOpcode& sourceOpcode, ShOpcodeVariables& vars)
{
	this->Opcode = sourceOpcode;
	this->Vars = vars;
	arch = Sh.getVersion();
}

/*int CShInstruction::getShiftedImmediate(unsigned int num, int& ShiftAmount)
{
	for (int i = 0; i < 32; i+=2)
	{
		unsigned int andval = (0xFFFFFF00 >> i) | (0xFFFFFF00 << (-i & 31));

		if ((num & andval) == 0)	// found it
		{
			ShiftAmount = i;
			return (num << i) | (num >> (-i & 31));
		}
	}
	return -1;
}*/

void CShInstruction::setPoolAddress(int64_t address, int byteWidth)
{
/*  int pos = (int) (address-((RamPos+8) & 0xFFFFFFFD));
	if (abs(pos) > 4095)
	{
		Logger::queueError(Logger::Error,L"Literal pool out of range");
		return;
	}

	Vars.Immediate = pos; */
	
	int gap;
	if (byteWidth == 4) {
	  // low 2 bits are masked for long access
	  gap = (int) (address-((RamPos + 4) & 0xFFFFFFFC));
	} else {
	  gap = (int) (address-(RamPos + 4));
	}
	
	if (gap % byteWidth != 0)
	{
		Logger::queueError(Logger::Error,L"Misaligned literal pool access?");
		return;
	}
	
	gap /= byteWidth;
	
	if (gap > 255)
	{
		Logger::queueError(Logger::Error,L"Literal pool out of range");
		return;
	}

	Vars.Immediate = gap;
}

bool CShInstruction::Validate()
{
  RamPos = g_fileManager->getVirtualAddress();

	Vars.Opcode.UseNewEncoding = false;
	Vars.Opcode.UseNewType = false;


	if (RamPos & 1)
	{
		Logger::queueError(Logger::Warning,L"Opcode not word aligned");
	}
	
	bool memoryAdvanced = false;
	
	// resolve immediate/displacement expressions
	// TODO: flag these ops instead of doing a stupid switch on the type
	switch (Opcode.type)
	{
	case SH_IFMT_MD:
	case SH_IFMT_ND4:
	case SH_IFMT_NMD:
	case SH_IFMT_D:
	case SH_IFMT_D12:
	case SH_IFMT_ND8:
	case SH_IFMT_I:
	case SH_IFMT_NI:
	{
		ExpressionValue value = Vars.ImmediateExpression.evaluate();

		switch (value.type)
		{
		case ExpressionValueType::Integer:
			Vars.Immediate = (int) value.intValue;
			break;
		default:
			Logger::queueError(Logger::Error,L"Invalid expression type");
			return false;
		}

		Vars.OriginalImmediate = Vars.Immediate;
		Vars.negative = false;
		
		g_fileManager->advanceMemory(2);
		memoryAdvanced = true;
    
	  switch (Opcode.type)
	  {
	  // displacement (by literal)
	  case SH_IFMT_MD:
	  case SH_IFMT_ND4:
	  case SH_IFMT_NMD:
	  case SH_IFMT_D:
	  case SH_IFMT_D12:
	  // displacement (pc-relative register load)
	  case SH_IFMT_ND8:
	  // immediate
	  case SH_IFMT_I:
	  case SH_IFMT_NI:
	  {
	    if ((Opcode.type == SH_IFMT_ND8)
	        && (Vars.DisplacementTargetsAddr == false)) {
        // this is a pool literal
				Sh.addPoolValue(this,Vars.Immediate,Vars.ImmediateScale);
      }
	    else
	    {
	      if (Vars.DisplacementTargetsAddr == true)
	      {
	        if (Vars.ImmediateScale != 4) {
	          Vars.Immediate = Vars.Immediate - (RamPos + 4);
	        } else {
	          Vars.Immediate = Vars.Immediate
	            - ((RamPos + 4) & ~Vars.DisplacementPcMask);
	        }
	      }
	      
	      int64_t immedLowerBound = 0;
	      int64_t immedUpperBound = (1 << Vars.ImmediateBitLen);
	      
	      if (Vars.ImmediateScale != 0)
	      {
	        if ((Vars.Immediate % Vars.ImmediateScale) != 0)
	        {
			      Logger::queueError(Logger::Error,L"Access not aligned to required boundary");
	        }
	      
	        immedLowerBound *= Vars.ImmediateScale;
	        immedUpperBound *= Vars.ImmediateScale;
	      }
	      
	      if (Vars.ImmediateSigned == true)
	      {
	        immedLowerBound = -(immedUpperBound / 2);
	        immedUpperBound = (immedUpperBound / 2);
	      }
	      
	      if ((Vars.Immediate < immedLowerBound)
	          || (Vars.Immediate >= immedUpperBound))
	      {
			    Logger::queueError(Logger::Error,L"Displacement or immediate out of range");
			    return false;
	      }
	      
	      if (Vars.ImmediateScale != 0)
	      {
	        Vars.Immediate = Vars.Immediate / Vars.ImmediateScale;
	      }
	      
	      // mask to only used bits
	      Vars.Immediate &= (1 << Vars.ImmediateBitLen) - 1;
	    }
	  }
	    break;
	  default:
	    break;
	  }
    
  }
	  break;
	
	default:
	  break;
	}
	
	if (!memoryAdvanced)
		g_fileManager->advanceMemory(2);
	
	return false;
}

void CShInstruction::FormatOpcode(char* Dest, const char* Source) const
{
  // TODO
/*	while (*Source != 0)
	{
		switch (*Source)
		{
		case 'C':	// condition
			Dest += sprintf(Dest,"%s",ShConditions[Vars.Opcode.c]);
			Source++;
			break;
		case 'S':	// set flag
			if (Vars.Opcode.s == true) *Dest++ = 's';
			Source++;
			break;
		case 'A':	// addressing mode
			if (Opcode.flags & SH_LOAD)
			{
				Dest += sprintf(Dest,"%s",ShAddressingModes[LdmModesSh[Vars.Opcode.a]]);
			} else {
				Dest += sprintf(Dest,"%s",ShAddressingModes[StmModesSh[Vars.Opcode.a]]);
			}
			Source++;
			break;
		case 'X':	// x flag
			if (Vars.Opcode.x == false) *Dest++ = 'b';
			else *Dest++ = 't';
			Source++;
			break;
		case 'Y':	// y flag
			if (Vars.Opcode.y == false) *Dest++ = 'b';
			else *Dest++ = 't';
			Source++;
			break;
		default:
			*Dest++ = *Source++;
			break;
		}
	}
	*Dest = 0; */
}

void CShInstruction::FormatInstruction(const char* encoding, char* dest) const
{
  
}

void CShInstruction::writeTempData(TempData& tempData) const
{
  // TODO
/*	char OpcodeName[32];
	char str[256];

	FormatOpcode(OpcodeName,Opcode.name);
	int pos = sprintf(str,"   %s",OpcodeName);
	while (pos < 11) str[pos++] = ' ';
	str[pos] = 0;
	FormatInstruction(Opcode.mask,&str[pos]);

	tempData.writeLine(RamPos,convertUtf8ToWString(str));*/
}

void CShInstruction::Encode() const
{
  unsigned int encoding = Opcode.encoding;
	switch (Vars.Opcode.UseNewType == true ? Vars.Opcode.NewType : Opcode.type)
	{
	case SH_IFMT_0:
	  // do nothing
	  break;
	case SH_IFMT_N:
	  encoding |= Vars.rn.num << 8;
	  break;
	case SH_IFMT_M:
	  encoding |= Vars.rm.num << 8;
	  break;
	case SH_IFMT_NM:
	  encoding |= Vars.rn.num << 8;
	  encoding |= Vars.rm.num << 4;
	  break;
	case SH_IFMT_MD:
	  encoding |= Vars.rm.num << 4;
	  encoding |= Vars.Immediate;
	  break;
	case SH_IFMT_ND4:
	  encoding |= Vars.rn.num << 4;
	  encoding |= Vars.Immediate;
	  break;
	case SH_IFMT_NMD:
	  encoding |= Vars.rn.num << 8;
	  encoding |= Vars.rm.num << 4;
	  encoding |= Vars.Immediate;
	  break;
	case SH_IFMT_D:
	  encoding |= Vars.Immediate;
	  break;
	case SH_IFMT_D12:
	  encoding |= Vars.Immediate;
	  break;
	case SH_IFMT_ND8:
	  encoding |= Vars.rn.num << 8;
	  encoding |= Vars.Immediate;
	  break;
	case SH_IFMT_I:
	  encoding |= Vars.Immediate;
	  break;
	case SH_IFMT_NI:
	  encoding |= Vars.rn.num << 8;
	  encoding |= Vars.Immediate;
	  break;
	default:
//		printf("bad");
		break;
	}

	g_fileManager->writeU16((uint16_t)encoding);
}
