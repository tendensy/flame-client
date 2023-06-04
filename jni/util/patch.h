#pragma once
#include "inlinehook.h"
#include "../main.h"
#include <string.h>

extern "C"
{
    void sub_naebal(uintptr_t dest, uintptr_t src, size_t size);
}

class CPatch {
public:

    static uintptr_t mmap_start, mmap_end, memlib_start, memlib_end;

    static void UnFuck(uintptr_t ptr);

    template<typename Addr>
    static void NOP(Addr addr, unsigned int count)
    {   
        addr += CRYPT_MASK;

        UnFuck(addr - CRYPT_MASK);

        for(uintptr_t ptr = addr - CRYPT_MASK; ptr != ((addr - CRYPT_MASK)+(count*2)); ptr += 2)
        {
            *(char*)(ptr) = 0x00;
            *(char*)(ptr+1) = 0x46;
        }

        cacheflush((addr - CRYPT_MASK), (uintptr_t)((addr - CRYPT_MASK) + count*2), 0);
    }

    template<typename Addr>
    static void RET(Addr addr)
    {
        WriteMemory(addr, "\xF7\x46", 2);
    }

    template <typename Src>
    static void WriteMemory(uintptr_t dest, Src src, size_t size)
    {   
        dest += CRYPT_MASK;
        size += CRYPT_MASK;

        sub_naebal(dest, (uintptr_t)src, size);
    }

    template <typename Src>
    static Src Write(uintptr_t dest, Src src, size_t size = 0)
    {   
	    size = sizeof(Src);
        CPatch::WriteMemory(dest, &src, size);
	    return src;
    }

    static void ReadMemory(uintptr_t dest, uintptr_t src, size_t size);
    static void InitHookStuff();
    static void WriteHookProc(uintptr_t addr, uintptr_t func);
    static void CodeInject(uintptr_t addr, uintptr_t func, int register);
    static void JMPCode(uintptr_t func, uintptr_t addr);
    
    template <typename Addr, typename Func, typename Orig>
    static void SetUpHook(Addr addr, Func func, Orig orig)
    {
        if(memlib_end < (memlib_start + 0x10) || mmap_end < (mmap_start + 0x20)) exit(1);

        ReadMemory(mmap_start, addr, 4);
        WriteHookProc(mmap_start+4, addr+4);
        *orig = mmap_start+1;
        mmap_start += 32;

        JMPCode(addr, memlib_start);
        WriteHookProc(memlib_start, func);
        memlib_start += 16;
    }

    template <typename Addr, typename Func>
    static void MethodHook(uintptr_t lib, Addr addr, Func func)
    {
        UnFuck(lib + addr);
        *(uintptr_t*)(lib + addr) = (uintptr_t)func;
    }

    template <typename Ret, typename... Args>
    static Ret CallFunction(unsigned int address, Args... args) 
    {   
	    return reinterpret_cast<Ret(__cdecl *)(Args...)>(address)(args...);
    }
    
    template <typename Addr, typename Func, typename Orig>
    static void InlineHook(uintptr_t lib, Addr addr, Func func, Orig orig)
    {   
        *orig = NULL;
        addr += CRYPT_MASK;
        registerInlineHook(lib + addr + 1, (uint32_t)func, (uint32_t**)orig);
        inlineHook(lib + addr + 1);
    }
};