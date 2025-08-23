#pragma once
#pragma pack(push, 1)
typedef struct _JMP_REL_SHORT
{
    UINT8  opcode;      
    UINT8  operand;     
} JMP_REL_SHORT, *PJMP_REL_SHORT;
typedef struct _JMP_REL
{
    UINT8  opcode;      
    UINT32 operand;     
} JMP_REL, *PJMP_REL, CALL_REL;
typedef struct _JMP_ABS
{
    UINT8  opcode0;     
    UINT8  opcode1;
    UINT32 dummy;
    UINT64 address;     
} JMP_ABS, *PJMP_ABS;
typedef struct _CALL_ABS
{
    UINT8  opcode0;     
    UINT8  opcode1;
    UINT32 dummy0;
    UINT8  dummy1;      
    UINT8  dummy2;
    UINT64 address;     
} CALL_ABS;
typedef struct _JCC_REL
{
    UINT8  opcode0;     
    UINT8  opcode1;
    UINT32 operand;     
} JCC_REL;
typedef struct _JCC_ABS
{
    UINT8  opcode;      
    UINT8  dummy0;
    UINT8  dummy1;      
    UINT8  dummy2;
    UINT32 dummy3;
    UINT64 address;     
} JCC_ABS;
#pragma pack(pop)
#if defined(_M_X64) || defined(__x86_64__)
    typedef JMP_ABS  JMP_RELAY;
    typedef PJMP_ABS PJMP_RELAY;
#else
    typedef JMP_REL  JMP_RELAY;
    typedef PJMP_REL PJMP_RELAY;
#endif
typedef struct _TRAMPOLINE
{
    LPVOID pTarget;         
    LPVOID pTrampoline;     
    UINT   trampolineSize;  
    BOOL   patchAbove;      
    UINT   nIP;             
    UINT8  oldIPs[8];       
    UINT8  newIPs[8];       
} TRAMPOLINE, *PTRAMPOLINE;
VOID CreateRelayFunction(PJMP_RELAY pJmpRelay, LPVOID pDetour);
BOOL CreateTrampolineFunction(PTRAMPOLINE ct);
