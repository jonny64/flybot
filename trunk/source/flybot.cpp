#include "stdwx.h"
#include "flybot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID )
{
    int argc = 0;
    char **argv = NULL;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        wxSetInstance((HINSTANCE)hModule);
        wxEntryStart(argc, argv);
        if( !wxTheApp || !wxTheApp->CallOnInit() )
        {
            wxEntryCleanup();
            if( wxTheApp )
                wxTheApp->OnExit();
            return FALSE;
        }

        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if( wxTheApp )
            wxTheApp->OnExit();
        wxEntryCleanup();
        break;
    }
    return TRUE;
}

void __stdcall OnRecvMessage2(int msgid, const WCHAR* objid, const void* param, unsigned WXUNUSED(paramsize))
{
    if (!wxGetApp().GetEnabledState())
        return;

    wxString msg;
    UserInfo userinfo;

    switch (msgid)
    {
        case BotInit::RECV_PM:
            // fall down
        case BotInit::RECV_PM_NEW:
            msg = wxString((WCHAR*)param);
            if (FlybotAPI.QueryUserinfo(objid, &userinfo))
            {
                wxGetApp().HandlePM(userinfo, msg);
            }
            break;
        default:
            break;
    }
}

extern "C" 
FLYBOT_API init(BotInit* apiInfo)
{
    if (NULL == apiInfo || apiInfo->apiVersion < 2) 
        return false;

    apiInfo->botId = APP_NAME;
    apiInfo->botVersion = APP_VERSION;
    apiInfo->RecvMessage2 = OnRecvMessage2;
    FlybotAPI.Initialize(apiInfo);

    wxGetApp().Dict.Load();

    return true;
}