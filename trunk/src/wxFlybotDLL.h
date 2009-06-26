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
	Dictionary m_dictionary;
public:
	bool OnInit();
	void ReloadDictionary();
	void HandlePM(UserInfo& userinfo, wxString& msg);
	int OnExit();
	~wxFlybotDLL();
};
