#pragma once


// REMOVE: opcode types
#define SH_TYPE3 0
#define SH_TYPE4 1
#define SH_TYPE5 2
#define SH_TYPE6 3
#define SH_TYPE7 4
#define SH_TYPE8 5
#define SH_TYPE9 6
#define SH_TYPE10 7
#define SH_TYPE11 8
#define SH_TYPE12 9
#define SH_TYPE13 10
#define SH_TYPE14 11
#define SH_TYPE15 12
#define SH_TYPE16 13
#define SH_TYPE17 14
#define SH_MISC 15

// REMOVE: opcode flags
#define SH_SH9			0x00000001
#define SH_S				0x00000002
#define SH_D				0x00000004
#define SH_N				0x00000008
#define SH_M				0x00000010
#define SH_IMMEDIATE		0x00000020
#define SH_WORD			0x00000040
#define SH_HALFWORD		0x00000080
#define SH_EXCHANGE		0x00000100	
#define SH_UNCOND			0x00000200
#define SH_REGISTER		0x00000400
#define SH_LOAD			0x00000800
#define SH_STORE			0x00001000
#define SH_X				0x00002000
#define SH_Y				0x00004000
#define SH_SHIFT			0x00008000
#define SH_POOL			0x00010000
#define SH_ABS				0x00020000	// absolute immediate value for range check
#define SH_BRANCH			0x00040000
#define SH_ABSIMM			0x00080000	// ldr r0,[200h]
#define SH_MRS				0x00100000	// differentiate between msr und mrs
#define SH_SWI				0x00200000	// software interrupt
#define SH_COPOP			0x00400000	// cp opcode number
#define SH_COPINF			0x00800000	// cp information number
#define SH_DN				0x01000000	// rn = rd
#define SH_DM				0x02000000	// rm = rd
#define SH_SIGN			0x04000000	// sign
#define SH_RDEVEN			0x08000000	// rd has to be even
#define SH_OPTIMIZE		0x10000000	// optimization
#define SH_OPMOVMVN		0x20000000	// ... of mov/mvn
#define SH_OPANDBIC		0x40000000	// ... of and/bic
#define SH_OPCMPCMN		0x80000000	// ... of cmp/cmn
#define SH_PCR			   0x100000000	// pc relative

/*typedef struct {
	const char* name;
	const char* mask;
	unsigned int encoding;
	unsigned int type:4;
	int64_t flags;
} tShOpcode;*/

// REMOVE
#define SH_AMODE_IB 0
#define SH_AMODE_IA 1
#define SH_AMODE_DB 2
#define SH_AMODE_DA 3
#define SH_AMODE_ED 4
#define SH_AMODE_FD 5
#define SH_AMODE_EA 6
#define SH_AMODE_FA 7

/*typedef struct {
	unsigned char p:1;
	unsigned char u:1;
} tShAddressingMode;

extern const tShOpcode ShOpcodes[];
extern const unsigned char LdmModesSh[8];
extern const unsigned char StmModesSh[8]; */






// opcode flags
#define SH_SH2ONLY			0x00000001

// instruction formats
#define SH_IFMT_0 0
#define SH_IFMT_N 1
#define SH_IFMT_M 2
#define SH_IFMT_NM 3
#define SH_IFMT_MD 4
#define SH_IFMT_ND4 5
#define SH_IFMT_NMD 6
#define SH_IFMT_D 7
#define SH_IFMT_D12 8
#define SH_IFMT_ND8 9
#define SH_IFMT_I 10
#define SH_IFMT_NI 11
//#define SH_IFMT_VIRTUAL_IMMED 12


// addressing modes
#define SH_AMODE_NONE 0
#define SH_AMODE_RN 1
#define SH_AMODE_RM 2
#define SH_AMODE_ATRN 3
#define SH_AMODE_ATRM 4
#define SH_AMODE_ATRNPL 5
#define SH_AMODE_ATRMPL 6
#define SH_AMODE_ATMIRN 7
#define SH_AMODE_ATMIRM 8
#define SH_AMODE_ATD4RN 9
#define SH_AMODE_ATD4RM 10
#define SH_AMODE_ATR0RN 11
#define SH_AMODE_ATR0RM 12
#define SH_AMODE_ATD8GBR 13
#define SH_AMODE_ATR0GBR 14
#define SH_AMODE_ATD8PC 15
#define SH_AMODE_D8 16
#define SH_AMODE_D12 17
//#define SH_AMODE_QSN 18
//#define SH_AMODE_QSM 19
#define SH_AMODE_IZ 18
#define SH_AMODE_IS 19
#define SH_AMODE_IZQ 20
//#define SH_AMODE_VIRTUAL_IMMED 21
//#define SH_AMODE_UNKNOWN 21


typedef struct {
	const char* name;
	const char* argstr;
	unsigned int encoding;
	unsigned int datsize;
	unsigned int type:4;
	unsigned int flags;
} tShOpcode;

extern const tShOpcode ShOpcodes[];

typedef struct {
	const char* name;
	unsigned int type;
} tShAmode;

extern const tShAmode ShAmodes[];


