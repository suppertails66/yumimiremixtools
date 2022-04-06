#include "stdafx.h"
#include "ArmOpcodes.h"
#include "Core/Common.h"
#include "Arm.h"
#include "CArmInstruction.h"


const unsigned char LdmModes[8] = { 3,1,2,0,3,1,2,0 };
const unsigned char StmModes[8] = { 3,1,2,0,0,2,1,3 };

/*	Placeholders
	sX	register
	dX	register
	nX	register
	mX	register
		X = 0:	all registers
		X = 1:	all registers except r15
	i:	shifted 8 bit immediate
	I:	32 bit immediate
	jx:	x bit immediate
	SX:	shift (0 = immediate or register, 1 = immediate only)
	W:	writeback
	p:	psr
	v:	sign for register operands
	/X:	optional character X
	R:	rlist
	D,N,M:	cop registers
	X:	cop number
	Y:	cop opcode
	Z:	cop information
*/

const tArmOpcode ArmOpcodes[] = {
	{ "bxC",	"n1",					0x012FFF10, ARM_TYPE3,	ARM_N },
	{ "blxC",	"n1",					0x012FFF30, ARM_TYPE3,	ARM_ARM9|ARM_N },

	{ "bC",		"/#I",					0x0A000000,	ARM_TYPE4,	ARM_BRANCH|ARM_IMMEDIATE|ARM_WORD },
	{ "blC",	"/#I",					0x0B000000,	ARM_TYPE4,	ARM_BRANCH|ARM_IMMEDIATE|ARM_WORD },
	{ "blx",	"/#I",					0xFA000000,	ARM_TYPE4,	ARM_ARM9|ARM_BRANCH|ARM_UNCOND|ARM_IMMEDIATE|ARM_HALFWORD|ARM_EXCHANGE },

	{ "andCS",	"d0,n0,m0S0",			0x00000000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N },
	{ "andCS",	"d0,m0S0",				0x00000000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N|ARM_DN },
	{ "andCS",	"d0,n0,/#i",			0x02000000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_OPTIMIZE|ARM_OPANDBIC },
	{ "andCS",	"d0,/#i",				0x02000000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_DN|ARM_OPTIMIZE|ARM_OPANDBIC },
	{ "eorCS",	"d0,n0,m0S0",			0x00200000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N },
	{ "eorCS",	"d0,m0S0",				0x00200000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N|ARM_DN },
	{ "xorCS",	"d0,n0,m0S0",			0x00200000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N },
	{ "xorCS",	"d0,m0S0",				0x00200000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N|ARM_DN },
	{ "eorCS",	"d0,n0,/#i",			0x02200000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N },
	{ "eorCS",	"d0,/#i",				0x02200000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_DN },
	{ "xorCS",	"d0,n0,/#i",			0x02200000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N },
	{ "xorCS",	"d0,/#i",				0x02200000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_DN },
	{ "subCS",	"d0,n0,m0S0",			0x00400000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N },
	{ "subCS",	"d0,m0S0",				0x00400000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N|ARM_DN },
	{ "subCS",	"d0,n0,/#i",			0x02400000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N },
	{ "subCS",	"d0,/#i",				0x02400000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_DN },
	{ "rsbCS",	"d0,n0,m0S0",			0x00600000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N },
	{ "rsbCS",	"d0,m0S0",				0x00600000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N|ARM_DN },
	{ "rsbCS",	"d0,n0,/#i",			0x02600000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N },
	{ "rsbCS",	"d0,/#i",				0x02600000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_DN },
	{ "addCS",	"d0,n0,m0S0",			0x00800000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N },
	{ "addCS",	"d0,m0S0",				0x00800000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N|ARM_DN },
	{ "addCS",	"d0,n0,/#i",			0x02800000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N },
	{ "addCS",	"d0,/#i",				0x02800000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_DN },
	{ "adcCS",	"d0,n0,m0S0",			0x00A00000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N },
	{ "adcCS",	"d0,m0S0",				0x00A00000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N|ARM_DN },
	{ "adcCS",	"d0,n0,/#i",			0x02A00000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N },
	{ "adcCS",	"d0,/#i",				0x02A00000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_DN },
	{ "sbcCS",	"d0,n0,m0S0",			0x00C00000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N },
	{ "sbcCS",	"d0,m0S0",				0x00C00000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N|ARM_DN },
	{ "sbcCS",	"d0,n0,/#i",			0x02C00000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N },
	{ "sbcCS",	"d0,/#i",				0x02C00000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_DN },
	{ "rscCS",	"d0,n0,m0S0",			0x00E00000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N },
	{ "rscCS",	"d0,m0S0",				0x00E00000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N|ARM_DN },
	{ "rscCS",	"d0,n0,/#i",			0x02E00000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N },
	{ "rscCS",	"d0,/#i",				0x02E00000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_DN },
	{ "tstC",	"n0,m0S0",				0x01100000,	ARM_TYPE5,	ARM_REGISTER|ARM_M|ARM_N },
	{ "tstC",	"n0,/#i",				0x03100000,	ARM_TYPE5,	ARM_SHIFT|ARM_IMMEDIATE|ARM_N },
	{ "teqC",	"n0,m0S0",				0x01300000,	ARM_TYPE5,	ARM_REGISTER|ARM_M|ARM_N },
	{ "teqC",	"n0,/#i",				0x03300000,	ARM_TYPE5,	ARM_SHIFT|ARM_IMMEDIATE|ARM_N },
	{ "cmpC",	"n0,m0S0",				0x01500000,	ARM_TYPE5,	ARM_REGISTER|ARM_M|ARM_N|ARM_OPTIMIZE|ARM_OPCMPCMN },
	{ "cmpC",	"n0,/#i",				0x03500000,	ARM_TYPE5,	ARM_SHIFT|ARM_IMMEDIATE|ARM_N|ARM_OPTIMIZE|ARM_OPCMPCMN },
	{ "cmnC",	"n0,m0S0",				0x01700000,	ARM_TYPE5,	ARM_REGISTER|ARM_M|ARM_N|ARM_OPTIMIZE|ARM_OPCMPCMN },
	{ "cmnC",	"n0,/#i",				0x03700000,	ARM_TYPE5,	ARM_SHIFT|ARM_IMMEDIATE|ARM_N|ARM_OPTIMIZE|ARM_OPCMPCMN },
	{ "orrCS",	"d0,n0,m0S0",			0x01800000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N },
	{ "orrCS",	"d0,m0S0",				0x01800000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N|ARM_DN },
	{ "orrCS",	"d0,n0,/#i",			0x03800000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N },
	{ "orrCS",	"d0,/#i",				0x03800000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_DN },
	{ "movCS",	"d0,m0S0",				0x01A00000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M },
	{ "nop",	"",						0xE1A00000,	ARM_TYPE5,	ARM_UNCOND },
	{ "movCS",	"d0,/#i",				0x03A00000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_OPTIMIZE|ARM_OPMOVMVN },
	{ "bicCS",	"d0,n0,m0S0",			0x01C00000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N },
	{ "bicCS",	"d0,m0S0",				0x01C00000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M|ARM_N|ARM_DN },
	{ "bicCS",	"d0,n0,/#i",			0x03C00000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_OPTIMIZE|ARM_OPANDBIC },
	{ "bicCS",	"d0,/#i",				0x03C00000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_N|ARM_DN|ARM_OPTIMIZE|ARM_OPANDBIC },
	{ "mvnCS",	"d0,m0S0",				0x01E00000,	ARM_TYPE5,	ARM_REGISTER|ARM_D|ARM_M },
	{ "mvnCS",	"d0,/#i",				0x03E00000,	ARM_TYPE5,	ARM_SHIFT|ARM_D|ARM_IMMEDIATE|ARM_OPTIMIZE|ARM_OPMOVMVN },

	{ "addCS",	"d1,=/#I",				0x028F0000,	ARM_TYPE5,	ARM_SHIFT|ARM_IMMEDIATE|ARM_D|ARM_PCR },
	{ "subCS",	"d1,=/#I",				0x028F0000,	ARM_TYPE5,	ARM_SHIFT|ARM_IMMEDIATE|ARM_D|ARM_PCR },
	{ "adrCS",	"d1,/#I",				0x028F0000,	ARM_TYPE5,	ARM_SHIFT|ARM_IMMEDIATE|ARM_D|ARM_PCR },


	{ "lslCS",	"d0,m0,z\x00",			0x01A00000,	ARM_TYPE5,	ARM_D|ARM_M|ARM_REGISTER },
	{ "lslCS",	"d0,z\x00",				0x01A00000,	ARM_TYPE5,	ARM_D|ARM_M|ARM_DM|ARM_REGISTER },
	{ "lsrCS",	"d0,m0,z\x01",			0x01A00000,	ARM_TYPE5,	ARM_D|ARM_M|ARM_REGISTER },
	{ "lsrCS",	"d0,z\x01",				0x01A00000,	ARM_TYPE5,	ARM_D|ARM_M|ARM_DM|ARM_REGISTER },
	{ "asrCS",	"d0,m0,z\x02",			0x01A00000,	ARM_TYPE5,	ARM_D|ARM_M|ARM_REGISTER },
	{ "asrCS",	"d0,z\x02",				0x01A00000,	ARM_TYPE5,	ARM_D|ARM_M|ARM_DM|ARM_REGISTER },
	{ "rorCS",	"d0,m0,z\x03",			0x01A00000,	ARM_TYPE5,	ARM_D|ARM_M|ARM_REGISTER },
	{ "rorCS",	"d0,z\x03",				0x01A00000,	ARM_TYPE5,	ARM_D|ARM_M|ARM_DM|ARM_REGISTER },

	{ "msrC",	"P0,m1",				0x0120F000,	ARM_TYPE6,	ARM_REGISTER|ARM_M },
	{ "movC",	"P0,m1",				0x0120F000,	ARM_TYPE6,	ARM_REGISTER|ARM_M },		 // msrC alias
	{ "msrC",	"P0,/#i",				0x0320F000,	ARM_TYPE6,	ARM_IMMEDIATE|ARM_SHIFT },
	{ "movC",	"P0,/#i",				0x0320F000,	ARM_TYPE6,	ARM_IMMEDIATE|ARM_SHIFT },	 // msrC alias
	{ "mrsC",	"d1,P1",				0x010F0000,	ARM_TYPE6,	ARM_REGISTER|ARM_D|ARM_MRS },
	{ "movC",	"d1,P1",				0x010F0000,	ARM_TYPE6,	ARM_REGISTER|ARM_D|ARM_MRS },//mrsC alias

	{ "mulCS",	"d1,m1,s1",				0x00000090,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S },
	{ "mulCS",	"d1,s1",				0x00000090,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_DM },
	{ "mlaCS",	"d1,m1,s1,n1",			0x00200090,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_N },
	{ "mlaCS",	"d1,s1,n1",				0x00200090,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_N|ARM_DM },
	{ "umullCS","n1,d1,m1,s1",			0x00800090,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_N },
	{ "umlalCS","n1,d1,m1,s1",			0x00A00090,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_N },
	{ "smullCS","n1,d1,m1,s1",			0x00C00090,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_N },
	{ "smlalCS","n1,d1,m1,s1",			0x00E00090,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_N },
	{ "smlaXYC","d1,m1,s1,n1",			0x01000080,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_N|ARM_X|ARM_Y|ARM_ARM9 },	
	{ "smlawYC","d1,m1,s1,n1",			0x01200080,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_N|ARM_Y|ARM_ARM9 },	
	{ "smulwYC","d1,m1,s1",				0x012000A0,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_Y|ARM_ARM9 },	
	{ "smlalXYC","n1,d1,m1,s1",			0x014000A0,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_N|ARM_X|ARM_Y|ARM_ARM9 },	
	{ "smulXYC","d1,m1,s1",				0x01600080,	ARM_TYPE7,	ARM_D|ARM_M|ARM_S|ARM_X|ARM_Y|ARM_ARM9 },	

	{ "strC",	"d0,[n0]",				0x05800000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "strC",	"d0,[n0,vm1S1]W",		0x07800000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_M|ARM_REGISTER|ARM_SIGN },
	{ "strC",	"d0,[n0],/#j\x0C",		0x04800000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "strC",	"d0,[n0],vm1S1",		0x06800000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_REGISTER|ARM_SIGN },
	{ "strC",	"d0,[n0,/#j\x0C]W",		0x05800000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldrC",	"d0,[n0]",				0x05900000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "ldrC",	"d0,[n0,vm1S1]W",		0x07900000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_M|ARM_REGISTER|ARM_SIGN },
	{ "ldrC",	"d0,[n0,/#j\x0C]W",		0x05900000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldrC",	"d0,[n0],vm1S1",		0x06900000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_REGISTER|ARM_SIGN },
	{ "ldrC",	"d0,[n0],/#j\x0C",		0x04900000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "strCb",	"d0,[n0]",				0x05C00000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "strCb",	"d0,[n0,vm1S1]W",		0x07C00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_M|ARM_REGISTER|ARM_SIGN },
	{ "strCb",	"d0,[n0,/#j\x0C]W",		0x05C00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "strCb",	"d0,[n0],vm1S1",		0x06C00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_REGISTER|ARM_SIGN },
	{ "strCb",	"d0,[n0],/#j\x0C",		0x04C00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldrCb",	"d0,[n0]",				0x05D00000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "ldrCb",	"d0,[n0,vm1S1]W",		0x07D00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_M|ARM_REGISTER|ARM_SIGN },
	{ "ldrCb",	"d0,[n0,/#j\x0C]W",		0x05D00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldrCb",	"d0,[n0],vm1S1",		0x06D00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_REGISTER|ARM_SIGN },
	{ "ldrCb",	"d0,[n0],/#j\x0C",		0x04D00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "strC",	"d0,[n0]!",				0x05A00000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "strCt",	"d0,[n0]",				0x05A00000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "strCt",	"d0,[n0],vm1S1",		0x06A00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_REGISTER|ARM_SIGN },
	{ "strCt",	"d0,[n0],/#j\x0C",		0x04A00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldrC",	"d0,[n0]!",				0x05B00000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "ldrCt",	"d0,[n0]",				0x05B00000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "ldrCt",	"d0,[n0],vm1S1",		0x06B00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_REGISTER|ARM_SIGN },
	{ "ldrCt",	"d0,[n0],/#j\x0C",		0x04B00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "strCb",	"d0,[n0]!",				0x05E00000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "strCbt",	"d0,[n0]",				0x05E00000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "strCbt",	"d0,[n0],vm1S1",		0x06E00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_REGISTER|ARM_SIGN },
	{ "strCbt",	"d0,[n0],/#j\x0C",		0x04E00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldrCb",	"d0,[n0]!",				0x05F00000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "ldrCbt",	"d0,[n0]",				0x05F00000,	ARM_TYPE9,	ARM_D|ARM_N },
	{ "ldrCbt",	"d0,[n0],vm1S1",		0x06F00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_REGISTER|ARM_SIGN },
	{ "ldrCbt",	"d0,[n0],/#j\x0C",		0x04F00000,	ARM_TYPE9,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldrC",	"d0,[/#j\x0C]",			0x059F0000,	ARM_TYPE9,	ARM_D|ARM_LOAD|ARM_ABSIMM|ARM_IMMEDIATE },
	{ "strC",	"d0,[/#j\x0C]",			0x058F0000,	ARM_TYPE9,	ARM_D|ARM_LOAD|ARM_ABSIMM|ARM_IMMEDIATE },
	{ "ldrC",	"d0,=/#i",				0x059F0000,	ARM_TYPE9,	ARM_D|ARM_POOL|ARM_IMMEDIATE },
	{ "pld",	"[n0]",					0xF5D0F000,	ARM_TYPE9,	ARM_ARM9|ARM_UNCOND|ARM_N },
	{ "pld",	"[n0,vm1S1]",			0xF7D0F000,	ARM_TYPE9,	ARM_ARM9|ARM_UNCOND|ARM_N |ARM_M|ARM_REGISTER|ARM_SIGN },
	{ "pld",	"[n0,/#j\x0C]",			0xF5D0F000,	ARM_TYPE9,	ARM_ARM9|ARM_UNCOND|ARM_N |ARM_IMMEDIATE|ARM_ABS },


	{ "strCh",	"d0,[n0]",				0x01C000B0,	ARM_TYPE10,	ARM_D|ARM_N },
	{ "strCh",	"d0,[n0,vm1]W",			0x018000B0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_REGISTER|ARM_M|ARM_SIGN },
	{ "strCh",	"d0,[n0,/#j\x08]W",		0x01C000B0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "strCh",	"d0,[n0],vm1",			0x008000B0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_REGISTER|ARM_SIGN },
	{ "strCh",	"d0,[n0],/#j\x08",		0x00C000B0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "strCh",	"d0,[/#j\x08]",			0x01CF00B0,	ARM_TYPE10,	ARM_D|ARM_LOAD|ARM_ABSIMM|ARM_IMMEDIATE },

	{ "ldrCh",	"d0,[n0,m1]",			0x019000B0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_REGISTER|ARM_M },
	{ "ldrCh",	"d0,[n0],/#j\x08",		0x00D000B0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldrCh",	"d0,[n0]",				0x01D000B0,	ARM_TYPE10,	ARM_D|ARM_N },
	{ "ldrCh",	"d0,[n0,/#j\x08]",		0x01D000B0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldrCsb",	"d0,[n0,m1]",			0x019000D0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_REGISTER|ARM_M },
	{ "ldrCsb",	"d0,[n0],/#j\x08",		0x00D000D0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldrCsb",	"d0,[n0]",				0x01D000D0,	ARM_TYPE10,	ARM_D|ARM_N },
	{ "ldrCsb",	"d0,[n0,/#j\x08]",		0x01D000D0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldCsb",	"d0,[n0,m1]",			0x019000D0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_REGISTER|ARM_M },
	{ "ldCsb",	"d0,[n0]",				0x01D000D0,	ARM_TYPE10,	ARM_D|ARM_N },
	{ "ldCsb",	"d0,[n0,/#j\x08]",		0x01D000D0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldrCsh",	"d0,[n0,m1]",			0x019000F0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_REGISTER|ARM_M },
	{ "ldrCsh",	"d0,[n0]",				0x01D000F0,	ARM_TYPE10,	ARM_D|ARM_N },
	{ "ldrCsh",	"d0,[n0,/#j\x08]",		0x01D000F0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },
	{ "ldCsh",	"d0,[n0,m1]",			0x019000F0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_REGISTER|ARM_M },
	{ "ldCsh",	"d0,[n0]",				0x01D000F0,	ARM_TYPE10,	ARM_D|ARM_N },
	{ "ldCsh",	"d0,[n0,/#j\x08]",		0x01D000F0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ABS },

	{ "ldrCd",	"d0,[n0,m1]",			0x018000D0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_REGISTER|ARM_M|ARM_ARM9 },
	{ "ldrCd",	"d0,[n0]",				0x01C000D0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_ARM9 },
	{ "ldrCd",	"d0,[n0,/#j\x08]",		0x01C000D0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ARM9 },
	{ "strCd",	"d0,[n0,m1]",			0x018000F0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_REGISTER|ARM_M|ARM_ARM9 },
	{ "strCd",	"d0,[n0]",				0x01C000F0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_ARM9 },
	{ "strCd",	"d0,[n0,/#j\x08]",		0x01C000F0,	ARM_TYPE10,	ARM_D|ARM_N|ARM_IMMEDIATE|ARM_ARM9 },

	{ "stmCA",	"/[n1/]W,/{R/}p",		0x08000000,	ARM_TYPE11,	ARM_STORE|ARM_N },
	{ "ldmCA",	"/[n1/]W,/{R/}p",		0x08100000,	ARM_TYPE11,	ARM_LOAD|ARM_N },
	{ "pushC",	"/{R/}",				0x092D0000,	ARM_TYPE11,	0 },
	{ "popC",	"/{R/}",				0x08BD0000,	ARM_TYPE11,	0 },

	{ "swpC",	"d1,m1,[n1]",			0x01000090,	ARM_TYPE12,	ARM_D|ARM_N|ARM_M },
	{ "swpC",	"d1,[n1]",				0x01000090,	ARM_TYPE12,	ARM_D|ARM_N|ARM_M|ARM_DM },
	{ "swpCb",	"d1,m1,[n1]",			0x01400090,	ARM_TYPE12,	ARM_D|ARM_N|ARM_M },
	{ "swpCb",	"d1,[n1]",				0x01400090,	ARM_TYPE12,	ARM_D|ARM_N|ARM_M|ARM_DM },

	{ "swiC",	"/#j\x18",				0x0F000000,	ARM_TYPE13,	ARM_SWI|ARM_IMMEDIATE },
	{ "bkpt",	"/#j\x10",				0xE1200070,	ARM_TYPE13,	ARM_UNCOND|ARM_IMMEDIATE },

	{ "cdpC",	"X,Y,D,N,M",			0x0E000000,	ARM_TYPE14,	ARM_COPOP },
	{ "cdpC",	"X,Y,D,N,M,Z",			0x0E000000,	ARM_TYPE14,	ARM_COPOP|ARM_COPINF },
	{ "cdp2",	"X,Y,D,N,M",			0xFE000000,	ARM_TYPE14,	ARM_ARM9|ARM_UNCOND|ARM_COPOP },
	{ "cdp2",	"X,Y,D,N,M,Z",			0xFE000000,	ARM_TYPE14,	ARM_ARM9|ARM_UNCOND|ARM_COPOP|ARM_COPINF },
	
	{ "mcrC",	"X,Y,d1,N,M",			0x0E000010,	ARM_TYPE16,	ARM_COPOP },
	{ "mcrC",	"X,Y,d1,N,M,Z",			0x0E000010,	ARM_TYPE16,	ARM_COPOP|ARM_COPINF },
	{ "movC",	"X,Y,N,M,Z,d1",			0x0E000010,	ARM_TYPE16,	ARM_COPOP|ARM_COPINF },	// alias
	{ "mcr2",	"X,Y,d1,N,M",			0xFE000010,	ARM_TYPE16,	ARM_ARM9|ARM_COPOP },
	{ "mcr2",	"X,Y,d1,N,M,Z",			0xFE000010,	ARM_TYPE16,	ARM_ARM9|ARM_COPOP|ARM_COPINF },
	{ "mrcC",	"X,Y,d1,N,M",			0x0E100010,	ARM_TYPE16,	ARM_COPOP },
	{ "mrcC",	"X,Y,d1,N,M,Z",			0x0E100010,	ARM_TYPE16,	ARM_COPOP|ARM_COPINF },
	{ "movC",	"d1,X,Y,N,M,Z",			0x0E100010,	ARM_TYPE16,	ARM_COPOP|ARM_COPINF },	// alias
	{ "mrc2",	"X,Y,d1,N,M",			0xFE100010,	ARM_TYPE16,	ARM_ARM9|ARM_COPOP },
	{ "mrc2",	"X,Y,d1,N,M,Z",			0xFE100010,	ARM_TYPE16,	ARM_ARM9|ARM_COPOP|ARM_COPINF },

	{ "mcrrC",	"X,Y,d1,n1,M",			0x0C400000,	ARM_TYPE17,	ARM_COPOP },
	{ "mrrcC",	"X,Y,d1,n1,M",			0x0C500000,	ARM_TYPE17,	ARM_COPOP },

	{ "clzC",	"d1,m1",				0x016F0F10,	ARM_MISC,	ARM_D|ARM_M|ARM_ARM9 },
	{ "qaddC",	"d1,m1,n1",				0x01000050,	ARM_MISC,	ARM_D|ARM_N|ARM_M|ARM_ARM9 },
	{ "qaddC",	"d1,n1",				0x01000050,	ARM_MISC,	ARM_D|ARM_N|ARM_M|ARM_ARM9|ARM_DM },
	{ "qsubC",	"d1,m1,n1",				0x01200050,	ARM_MISC,	ARM_D|ARM_N|ARM_M|ARM_ARM9 },
	{ "qsubC",	"d1,n1",				0x01200050,	ARM_MISC,	ARM_D|ARM_N|ARM_M|ARM_ARM9|ARM_DM },
	{ "qdaddC",	"d1,m1,n1",				0x01400050,	ARM_MISC,	ARM_D|ARM_N|ARM_M|ARM_ARM9 },
	{ "qdaddC",	"d1,n1",				0x01400050,	ARM_MISC,	ARM_D|ARM_N|ARM_M|ARM_ARM9|ARM_DM },
	{ "qdsubC",	"d1,m1,n1",				0x01600050,	ARM_MISC,	ARM_D|ARM_N|ARM_M|ARM_ARM9 },
	{ "qdsubC",	"d1,n1",				0x01600050,	ARM_MISC,	ARM_D|ARM_N|ARM_M|ARM_ARM9|ARM_DM },

	{ nullptr,	nullptr,				0,			0,			0 }
};