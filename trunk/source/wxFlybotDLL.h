#pragma once
#include "stdwx.h"

#include "FlybotConfig.h"
#include "UserInfo.h"
#include "Session.h"
#include "Dictionary.h"
#include "FlybotTaskBarIcon.h"
#include "DictionaryFSWatcher.h"

const wxString PATH_TO_TEXT_LOG = wxT("Logs\\flybot.log");

class wxFlybotDLL: public wxApp
{
    FlybotTaskBarIcon *m_taskBarIcon;
    wxLocale *m_locale;
    WX_DECLARE_STRING_HASH_MAP(Session*, SessionMap);
    SessionMap m_sessions;
    bool m_online; // bot status (active/inactive)
    DictionaryFSWatcher *m_dictWatcher;
    
    void SelectLanguage(int lang);
public:
    Dictionary Dict;
    FlybotConfig Config;
    
    void SwitchState();
    bool GetEnabledState();
    bool OnInit();
	bool LogFileExists();
    void ReloadDictionary();
    void OpenDictionary();
    void OpenLog();
    wxString GetLogFilename();
    void HandlePM(UserInfo& userinfo, wxString& msg);
    int OnExit();
    ~wxFlybotDLL();
};

DECLARE_APP(wxFlybotDLL)
