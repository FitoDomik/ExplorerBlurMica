#include <windows.h>
#ifdef _MSC_VER
    #include <intrin.h>
#endif
#ifndef ARRAYSIZE
    #define ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))
#endif
#if defined(_M_X64) || defined(__x86_64__)
    #include "./hde/hde64.h"
    typedef hde64s HDE;
    #define HDE_DISASM(code, hs) hde64_disasm(code, hs)
#else
    #include "./hde/hde32.h"
    typedef hde32s HDE;
    #define HDE_DISASM(code, hs) hde32_disasm(code, hs)
#endif
#include "trampoline.h"
#include "buffer.h"
static BOOL IsCodePadding(LPBYTE pInst, UINT size)
{
    UINT i;
    if (pInst[0] != 0x00 && pInst[0] != 0x90 && pInst[0] != 0xCC)
        return FALSE;
    for (i = 1; i < size; ++i)
    {
        if (pInst[i] != pInst[0])
            return FALSE;
    }
    return TRUE;
}
VOID CreateRelayFunction(PJMP_RELAY pJmpRelay, LPVOID pDetour)
{
#if defined(_M_X64) || defined(__x86_64__)
    JMP_ABS jmp = {
        0xFF, 0x25, 0x00000000, 
        0x0000000000000000ULL   
    };
    jmp.address = (ULONG_PTR)pDetour;
#else
    JMP_REL jmp = {
        0xE9,                   
        0x00000000              
    };
    jmp.operand = (UINT32)((LPBYTE)pDetour - ((LPBYTE)pJmpRelay + sizeof(jmp)));
#endif
    memcpy(pJmpRelay, &jmp, sizeof(jmp));
}
BOOL CreateTrampolineFunction(PTRAMPOLINE ct)
{
#if defined(_M_X64) || defined(__x86_64__)
    CALL_ABS call = {
        0xFF, 0x15, 0x00000002, 
        0xEB, 0x08,             
        0x0000000000000000ULL   
    };
    JMP_ABS jmp = {
        0xFF, 0x25, 0x00000000, 
        0x0000000000000000ULL   
    };
    JCC_ABS jcc = {
        0x70, 0x0E,             
        0xFF, 0x25, 0x00000000, 
        0x0000000000000000ULL   
    };
#else
    CALL_REL call = {
        0xE8,                   
        0x00000000              
    };
    JMP_REL jmp = {
        0xE9,                   
        0x00000000              
    };
    JCC_REL jcc = {
        0x0F, 0x80,             
        0x00000000              
    };
#endif
    UINT8     oldPos   = 0;
    UINT8     newPos   = 0;
    ULONG_PTR jmpDest  = 0;     
    BOOL      finished = FALSE; 
#if defined(_M_X64) || defined(__x86_64__)
    UINT8     instBuf[16];
#endif
    ct->patchAbove = FALSE;
    ct->nIP        = 0;
    do
    {
        HDE       hs;
        UINT      copySize;
        LPVOID    pCopySrc;
        ULONG_PTR pOldInst = (ULONG_PTR)ct->pTarget     + oldPos;
        ULONG_PTR pNewInst = (ULONG_PTR)ct->pTrampoline + newPos;
        copySize = HDE_DISASM((LPVOID)pOldInst, &hs);
        if (hs.flags & F_ERROR)
            return FALSE;
        pCopySrc = (LPVOID)pOldInst;
        if (oldPos >= sizeof(JMP_REL))
        {
#if defined(_M_X64) || defined(__x86_64__)
            jmp.address = pOldInst;
#else
            jmp.operand = (UINT32)(pOldInst - (pNewInst + sizeof(jmp)));
#endif
            pCopySrc = &jmp;
            copySize = sizeof(jmp);
            finished = TRUE;
        }
#if defined(_M_X64) || defined(__x86_64__)
        else if ((hs.modrm & 0xC7) == 0x05)
        {
            PUINT32 pRelAddr;
#ifndef _MSC_VER
            memcpy(instBuf, (LPBYTE)pOldInst, copySize);
#else
            __movsb(instBuf, (LPBYTE)pOldInst, copySize);
#endif
            pCopySrc = instBuf;
            pRelAddr = (PUINT32)(instBuf + hs.len - ((hs.flags & 0x3C) >> 2) - 4);
            *pRelAddr
                = (UINT32)((pOldInst + hs.len + (INT32)hs.disp.disp32) - (pNewInst + hs.len));
            if (hs.opcode == 0xFF && hs.modrm_reg == 4)
                finished = TRUE;
        }
#endif
        else if (hs.opcode == 0xE8)
        {
            ULONG_PTR dest = pOldInst + hs.len + (INT32)hs.imm.imm32;
#if defined(_M_X64) || defined(__x86_64__)
            call.address = dest;
#else
            call.operand = (UINT32)(dest - (pNewInst + sizeof(call)));
#endif
            pCopySrc = &call;
            copySize = sizeof(call);
        }
        else if ((hs.opcode & 0xFD) == 0xE9)
        {
            ULONG_PTR dest = pOldInst + hs.len;
            if (hs.opcode == 0xEB) 
                dest += (INT8)hs.imm.imm8;
            else
                dest += (INT32)hs.imm.imm32;
            if ((ULONG_PTR)ct->pTarget <= dest
                && dest < ((ULONG_PTR)ct->pTarget + sizeof(JMP_REL)))
            {
                if (jmpDest < dest)
                    jmpDest = dest;
            }
            else
            {
#if defined(_M_X64) || defined(__x86_64__)
                jmp.address = dest;
#else
                jmp.operand = (UINT32)(dest - (pNewInst + sizeof(jmp)));
#endif
                pCopySrc = &jmp;
                copySize = sizeof(jmp);
                finished = (pOldInst >= jmpDest);
            }
        }
        else if ((hs.opcode & 0xF0) == 0x70
            || (hs.opcode & 0xFC) == 0xE0
            || (hs.opcode2 & 0xF0) == 0x80)
        {
            ULONG_PTR dest = pOldInst + hs.len;
            if ((hs.opcode & 0xF0) == 0x70      
                || (hs.opcode & 0xFC) == 0xE0)  
                dest += (INT8)hs.imm.imm8;
            else
                dest += (INT32)hs.imm.imm32;
            if ((ULONG_PTR)ct->pTarget <= dest
                && dest < ((ULONG_PTR)ct->pTarget + sizeof(JMP_REL)))
            {
                if (jmpDest < dest)
                    jmpDest = dest;
            }
            else if ((hs.opcode & 0xFC) == 0xE0)
            {
                return FALSE;
            }
            else
            {
                UINT8 cond = ((hs.opcode != 0x0F ? hs.opcode : hs.opcode2) & 0x0F);
#if defined(_M_X64) || defined(__x86_64__)
                jcc.opcode  = 0x71 ^ cond;
                jcc.address = dest;
#else
                jcc.opcode1 = 0x80 | cond;
                jcc.operand = (UINT32)(dest - (pNewInst + sizeof(jcc)));
#endif
                pCopySrc = &jcc;
                copySize = sizeof(jcc);
            }
        }
        else if ((hs.opcode & 0xFE) == 0xC2)
        {
            finished = (pOldInst >= jmpDest);
        }
        if (pOldInst < jmpDest && copySize != hs.len)
            return FALSE;
        if ((newPos + copySize) > ct->trampolineSize)
            return FALSE;
        if (ct->nIP >= ARRAYSIZE(ct->oldIPs))
            return FALSE;
        ct->oldIPs[ct->nIP] = oldPos;
        ct->newIPs[ct->nIP] = newPos;
        ct->nIP++;
#ifndef _MSC_VER
        memcpy((LPBYTE)ct->pTrampoline + newPos, pCopySrc, copySize);
#else
        __movsb((LPBYTE)ct->pTrampoline + newPos, (LPBYTE)pCopySrc, copySize);
#endif
        newPos += copySize;
        oldPos += hs.len;
    }
    while (!finished);
    if (oldPos < sizeof(JMP_REL)
        && !IsCodePadding((LPBYTE)ct->pTarget + oldPos, sizeof(JMP_REL) - oldPos))
    {
        if (oldPos < sizeof(JMP_REL_SHORT)
            && !IsCodePadding((LPBYTE)ct->pTarget + oldPos, sizeof(JMP_REL_SHORT) - oldPos))
        {
            return FALSE;
        }
        if (!IsExecutableAddress((LPBYTE)ct->pTarget - sizeof(JMP_REL)))
            return FALSE;
        if (!IsCodePadding((LPBYTE)ct->pTarget - sizeof(JMP_REL), sizeof(JMP_REL)))
            return FALSE;
        ct->patchAbove = TRUE;
    }
    return TRUE;
}
