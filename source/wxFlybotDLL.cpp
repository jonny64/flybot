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
    return m_online;
}

void wxFlybotDLL::SwitchState()
{
    m_online = !m_online;
}


bool wxFlybotDLL::OnInit()
{
    m_taskBarIcon = NULL;
    m_locale = NULL;
    
    Config.Read(SETTING_BOT_ONLINE, &m_online, true);
        
    SelectLanguage(wxLANGUAGE_RUSSIAN);

    m_taskBarIcon = new FlybotTaskBarIcon();
    if (!m_taskBarIcon)
    {
        return false;
    }
    // set new logger (SetActiveTarget returns old logger)
    delete wxLog::SetActiveTarget(new wxLogBalloon(m_taskBarIcon));

    // reload when dictionary file is changed
    m_dictWatcher = new DictionaryFSWatcher();
	m_dictWatcher->Start(DICTIONARY_WATCH_INTERVAL_MS);
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
    Config.Write(SETTING_BOT_ONLINE, m_online);

    if (m_taskBarIcon)
    {
        m_taskBarIcon->RemoveIcon();
        delete m_taskBarIcon;
    }

    // free session info
    FOREACH(SessionMap, it, m_sessions)
    {
        delete it->second;
    }
    m_sessions.clear();

    delete m_locale;

	m_dictWatcher->Stop();
	delete m_dictWatcher;

    return SUCCESS;
}

wxFlybotDLL::~wxFlybotDLL()
{
}