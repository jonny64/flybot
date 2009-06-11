#include "stdwx.h"
#include "flybot.h"
#include "UserInfo.h"
#include "Session.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class wxFlybotDLL: public wxApp
{
	MyTaskBarIcon   *m_taskBarIcon;
	WX_DECLARE_STRING_HASH_MAP(Session*, SessionMap);
	SessionMap m_sessions;
public:
	bool OnInit()
	{
		m_taskBarIcon = NULL;
		m_taskBarIcon = new MyTaskBarIcon();
		
		m_taskBarIcon->SwitchIcon();
		return true;
	}

	void HandlePM(UserInfo& userinfo, wxString& msg)
	{
		// if session is not opened, open one
		// answer pm, according to session enviroment
	}
	
	int OnExit()
	{
		m_sessions.clear();

		m_taskBarIcon->RemoveIcon();
		delete m_taskBarIcon;

		return 0;
	}

	~wxFlybotDLL()
	{
	}
};

IMPLEMENT_APP_NO_MAIN(wxFlybotDLL)


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
	WCHAR *msg = (WCHAR*)param;
	WCHAR *info = (WCHAR*)g_botAPI.QueryInfo(CODES::QUERY_USER_BY_CID, objid, NULL, 0);

	switch (msgid)
	{
		case CODES::RECV_PM:
			// fall down
		case CODES::RECV_PM_NEW:
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