#include "stdwx.h"
#include <wx/stdpaths.h>
#include <wx/utils.h>
#include "wxFlybotDLL.h"
#include "wxLogBalloon.h"
#include "FlybotConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_APP_NO_MAIN(wxFlybotDLL)

wxCriticalSection gSession;

bool wxFlybotDLL::GetEnabledState()
{
	bool enabled = true;
	Config.Read(SETTING_BOT_ONLINE, &enabled, true);
	return enabled;
}

void wxFlybotDLL::SwitchState()
{
    bool enabled = true;
	Config.Read(SETTING_BOT_ONLINE, &enabled, true);
	Config.Write(SETTING_BOT_ONLINE, !enabled);
}


bool wxFlybotDLL::OnInit()
{
    m_taskBarIcon = NULL;
    m_locale = NULL;
        
    SelectLanguage(wxLANGUAGE_RUSSIAN);

    m_taskBarIcon = new FlybotTaskBarIcon();
    if (!m_taskBarIcon)
	{
		return false;
	}
	m_taskBarIcon->SetupIcon();
    // set new logger (SetActiveTarget returns old logger)
    delete wxLog::SetActiveTarget(new wxLogBalloon(m_taskBarIcon));

    Dict.Load();
    return true;
}

void wxFlybotDLL::SelectLanguage(int lang)
{
    delete m_locale;
    m_locale = new wxLocale(lang);
	if (!m_locale)
	{
		return;
	}

    m_locale->AddCatalogLookupPathPrefix(wxStandardPaths::Get().GetPluginsDir());
    if (!m_locale->AddCatalog(wxT("Chatbot")) )
    {
        // fall back to embedded english (do nothing)
    }
}

void wxFlybotDLL::ReloadDictionary()
{
    if (SUCCESS == Dict.Load())
    {
        wxLogMessage(_("Dictionary was successfully loaded"));
    }
}

void wxFlybotDLL::OpenDictionary()
{
    if (!wxLaunchDefaultApplication(Dictionary::GetDictionaryFilename()) )
    {
        // TODO: handle case when no program associated to .ini files
    }
}

void wxFlybotDLL::HandlePM(UserInfo& userinfo, wxString& msg)
{
    // do not process favourites
    if ( userinfo.Favourite() )
        return;

    wxCriticalSectionLocker locker(gSession); // leaves cs in destructor
	
	// if it is a new PM, create new session
    wxString cid = userinfo[FLYBOT_API_CID];
    if (NULL == m_sessions[cid])
    {
        m_sessions[cid] = new Session(userinfo);
    }

    // answer pm, according to previous replies, etc.
    if (NULL != m_sessions[cid])
	{
		m_sessions[cid]->Answer(msg);
	}
}

int wxFlybotDLL::OnExit()
{
    if (m_taskBarIcon)
	{
		m_taskBarIcon->RemoveIcon();
		delete m_taskBarIcon;
	}

	// free session info
    SessionMap::iterator it;
    for( it = m_sessions.begin(); it != m_sessions.end(); ++it )
    {
        delete it->second;
    }
    m_sessions.clear();

	delete m_locale;

    return 0;
}

wxFlybotDLL::~wxFlybotDLL()
{
}