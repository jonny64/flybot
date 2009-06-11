#include "stdwx.h"
#include "flybot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class wxFlybotDLL: public wxApp
{
	MyTaskBarIcon   *m_taskBarIcon;

public:
	bool OnInit()
	{
		m_taskBarIcon = NULL;
		m_taskBarIcon = new MyTaskBarIcon();
		
		m_taskBarIcon->SwitchIcon();
		return true;
	}
	
	int OnExit()
	{
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
	// parse some test commands
	if (msgid != BotInit::RECV_PM_NEW && msgid != BotInit::RECV_PM) return;

	WCHAR cid[64], *msg = (WCHAR*)param, *p, *q = 0;
	WCHAR *str;
	DWORD value;
	WCHAR *userinfo = (WCHAR*)g_botAPI.QueryInfo(BotInit::CODES::QUERY_USER_BY_CID, objid, NULL, 0);
    // HandlePM(userinfo, msg);

	switch (msg[0]) {
		// s <cid> <message>
		// send private message to user <cid>
	  case 's':
		  memcpy(cid, msg+2, 39*sizeof(WCHAR));
		  cid[39] = 0;
		  str = msg+2+39+1;
		  g_botAPI.SendMessage2(BotInit::SEND_PM, cid, str, (wcslen(str)+1)*sizeof(WCHAR));
		  break;
		  // t <huburl> <message>
		  // send public message to hub <huburl>
	  case 't':
		  p = wcschr(msg+2, ' ');
		  if (!p) break;
		  memcpy(cid, msg+2, (p-(msg+2))*sizeof(WCHAR));
		  cid[p-(msg+2)] = 0;
		  str = p+1;
		  g_botAPI.SendMessage2(BotInit::SEND_CM, cid, str, (wcslen(str)+1)*sizeof(WCHAR));
		  break;
		  // x <cid>
		  // close PM window
	  case 'x':
		  memcpy(cid, msg+2, 39*sizeof(WCHAR));
		  cid[39] = 0;
		  g_botAPI.SendMessage2(BotInit::USER_CLOSE, cid, 0, 0);
		  break;
		  // b <cid> <0/1>
		  // ban/unban
	  case 'b':
		  memcpy(cid, msg+2, 39*sizeof(WCHAR));
		  cid[39] = 0;
		  value = msg[2+39+1]-'0';
		  g_botAPI.SendMessage2(BotInit::USER_BAN, cid, &value, sizeof(value));
		  break;
		  // i <cid> <0/1>
		  // ignore/unignore
	  case 'i':
		  memcpy(cid, msg+2, 39*sizeof(WCHAR));
		  cid[39] = 0;
		  value = msg[2+39+1]-'0';
		  g_botAPI.SendMessage2(BotInit::USER_IGNORE, cid, &value, sizeof(value));
		  break;
		  // l <cid> <time>
		  // give/remove slot
	  case 'l':
		  memcpy(cid, msg+2, 39*sizeof(WCHAR));
		  cid[39] = 0;
		  value = _wtoi(&msg[2+39+1]);
		  g_botAPI.SendMessage2(BotInit::USER_SLOT, cid, &value, sizeof(value));
		  break;
	  case 'q':
		  switch (msg[1]) {
			  // qu <cid>
			  // query user by CID
	  case 'u':
		  q = (WCHAR*)g_botAPI.QueryInfo(BotInit::QUERY_USER_BY_CID, msg+3, NULL, 0);
		  break;
		  // qh <url>
		  // query hub by url
	  case 'h':
		  q = (WCHAR*)g_botAPI.QueryInfo(BotInit::QUERY_HUB_BY_URL, msg+3, NULL, 0);
		  break;
		  // qs
		  // query self CID
	  case 's':
		  q = (WCHAR*)g_botAPI.QueryInfo(BotInit::QUERY_SELF, NULL, NULL, 0);
		  break;
		  // qc
		  // query connected hubs
	  case 'c':
		  q = (WCHAR*)g_botAPI.QueryInfo(BotInit::QUERY_CONNECTED_HUBS, NULL, NULL, 0);
		  break;
		  // ql <url>
		  // query hub users
	  case 'l':
		  q = (WCHAR*)g_botAPI.QueryInfo(BotInit::QUERY_HUB_USERS, NULL, NULL, 0);
		  break;
	  case '1':
	  case '2':
	  case '3':
	  case '4':
		  break;
	}
}
}

extern "C" 
FLYBOT_API init(BotInit* _init)
{
	if (_init->apiVersion < 2) 
		return false;
	_init->botId = "Api2Test";
	_init->botVersion = "1.0";
	_init->RecvMessage2 = OnRecvMessage2;
	memcpy(&::g_botAPI, _init, sizeof(BotInit));

	return true;
}