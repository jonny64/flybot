#include "stdwx.h"
#include "flybot.h"
#include "wxFlybotDLL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DECLARE_APP(wxFlybotDLL)

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
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

#ifdef _MANAGED
#pragma managed(pop)
#endif

void __stdcall OnRecvMessage2(int msgid, const WCHAR* objid, const void* param, unsigned paramsize)
{
	WCHAR *msg = NULL;
	WCHAR *info = NULL;

	switch (msgid)
	{
		case RECV_PM:
			// fall down
		case RECV_PM_NEW:
			msg = (WCHAR*)param;
			info = (WCHAR*)g_botAPI.QueryInfo(QUERY_USER_BY_CID, objid, NULL, 0);
			wxGetApp().HandlePM(UserInfo(info), wxString(msg));
			break;
		default:
			break;
	}
}

extern "C" 
FLYBOT_API init(BotInit* _init)
{
	if (_init->apiVersion < 2) 
		return false;
	_init->botId = "flybot";
	_init->botVersion = "0.3";
	_init->RecvMessage2 = OnRecvMessage2;
	memcpy(&::g_botAPI, _init, sizeof(BotInit));

	return true;
}