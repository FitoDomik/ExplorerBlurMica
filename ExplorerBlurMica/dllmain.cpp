#include "framework.h"
#include "module.h"
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    return MBox::OnDllMain(hModule, ul_reason_for_call);
}