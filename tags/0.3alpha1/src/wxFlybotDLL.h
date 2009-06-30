#pragma once
#include "stdwx.h"

#include "UserInfo.h"
#include "Session.h"
#include "Dictionary.h"
#include "tray.h"


class wxFlybotDLL: public wxApp
{
	MyTaskBarIcon   *m_taskBarIcon;

	WX_DECLARE_STRING_HASH_MAP(Session*, SessionMap);
	SessionMap m_sessions;
public:
	Dictionary Dict;

	bool OnInit();
	void ReloadDictionary();
	void OpenDictionary();
	void HandlePM(UserInfo& userinfo, wxString& msg);
	int OnExit();
	~wxFlybotDLL();
};

DECLARE_APP(wxFlybotDLL)
