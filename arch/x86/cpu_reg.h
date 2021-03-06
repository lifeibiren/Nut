#ifndef _CPU_REGS_
#define _CPU_REGS_


#define REG_FLAGS_CF_MASK		0x00000001
#define REG_FLAGS_PF_MASK		0x00000004
#define REG_FLAGS_AF_MASK		0x00000010
#define REG_FLAGS_ZF_MASK		0x00000040
#define REG_FLAGS_SF_MASK		0x00000080
#define REG_FLAGS_TF_MASK		0x00000100
#define REG_FLAGS_IF_MASK		0x00000200
#define REG_FLAGS_DF_MASK		0x00000400
#define REG_FLAGS_OF_MASK		0x00000800

#define REG_FLAGS_IOPL_MASK		0x00003000
#define REG_FLAGS_IOPL_EXT(x)		((x & REG_FLAGS_IOPL_MASK) >> 12)
#define REG_FLAGS_IOPL_SYTH(x)		((x << 12) & REG_FLAGS_IOPL_MASK)

#define REG_FLAGS_NT_MASK		0x00004000
#define REG_FLAGS_RF_MASK		0x00010000
#define REG_FLAGS_VM_MASK		0x00020000
#define REG_FLAGS_AC_MASK		0x00040000
#define REG_FLAGS_VIF_MASK		0x00080000
#define REG_FLAGS_VIP_MASK		0x00100000
#define REG_FLAGS_ID_MASK		0x00200000

#endif
