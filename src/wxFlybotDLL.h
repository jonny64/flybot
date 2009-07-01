#pragma once
#include "stdwx.h"
#include <wx/config.h>

#include "UserInfo.h"
#include "Session.h"
#include "Dictionary.h"
#include "FlybotTaskBarIcon.h"

const wxString SETTING_USE_BALLOONS = wxT("EnableBalloons");

class wxFlybotDLL: public wxApp
{
	FlybotTaskBarIcon   *m_taskBarIcon;

	WX_DECLARE_STRING_HASH_MAP(Session*, SessionMap);
	SessionMap m_sessions;
	bool m_enabled;
public:
	wxFlybotDLL(): Config(wxT("flybot")) {};
	
	Dictionary Dict;
	wxConfig Config;

	void SwitchState();
	bool GetEnabledState();
	bool OnInit();
	void ReloadDictionary();
	void OpenDictionary();
	void HandlePM(UserInfo& userinfo, wxString& msg);
	bool BalloonsEnabled();
	int OnExit();
	~wxFlybotDLL();
};

DECLARE_APP(wxFlybotDLL)
