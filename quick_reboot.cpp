#include <Windows.h>

bool PrivilegeMgr(const char* Name, bool Enable) {
    HANDLE hToken; TOKEN_PRIVILEGES Priv; Priv.PrivilegeCount = 1;
    auto CurrentProcess =  GetCurrentProcess();
    OpenProcessToken(CurrentProcess, TOKEN_ALL_ACCESS, &hToken);
    Priv.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;
    auto Ret = LookupPrivilegeValueA( nullptr, Name, &Priv.Privileges[0].Luid);
    AdjustTokenPrivileges( hToken, false, &Priv, sizeof(Priv), nullptr, nullptr);
    CloseHandle(hToken); return Ret;
}

typedef enum _SHUTDOWN_ACTION {




    ShutdownNoReboot,
    ShutdownReboot,
    ShutdownPowerOff


} SHUTDOWN_ACTION, * PSHUTDOWN_ACTION;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    int ret = MessageBoxA(0, "Reboot (YES) or Shutdown (NO)", "Quick Reboot", MB_YESNOCANCEL);

   
    typedef NTSTATUS(
        NTAPI*


        NtShutdownSystemFn)(



            IN SHUTDOWN_ACTION      Action);

    HINSTANCE ntdll = GetModuleHandleA("ntdll.dll");
    auto NtShutdownSystem = (NtShutdownSystemFn)GetProcAddress(ntdll, "NtShutdownSystem");

    if (ret == IDYES) {
        PrivilegeMgr("SeShutdownPrivilege", 1);
        NtShutdownSystem(ShutdownReboot);
        PrivilegeMgr("SeShutdownPrivilege", 0);
    }

    if (ret == IDNO) {
        PrivilegeMgr("SeShutdownPrivilege", 1);
        NtShutdownSystem(ShutdownPowerOff);
        PrivilegeMgr("SeShutdownPrivilege", 0);
    }

    return 0;
}