#pragma once
#include "stdwx.h"

#include "FlybotConfig.h"
#include "UserInfo.h"
#include "Session.h"
#include "Dictionary.h"
#include "FlybotTaskBarIcon.h"

class wxFlybotDLL: public wxApp
{
    FlybotTaskBarIcon *m_taskBarIcon;
    wxLocale *m_locale;
    WX_DECLARE_STRING_HASH_MAP(Session*, SessionMap);
    SessionMap m_sessions;
    bool m_online; // bot status (active/inactive)

    void SelectLanguage(int lang);
public:
    
    Dictionary Dict;
    FlybotConfig Config;

    void SwitchState();
    bool GetEnabledState();
    bool OnInit();
    void ReloadDictionary();
    void OpenDictionary();
    void HandlePM(UserInfo& userinfo, wxString& msg);
    int OnExit();
    ~wxFlybotDLL();
};

DECLARE_APP(wxFlybotDLL)
