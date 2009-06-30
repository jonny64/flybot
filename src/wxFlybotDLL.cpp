#include "stdwx.h"
#include "wxFlybotDLL.h"
#include "wxLogBaloon.h"
#include <wx/utils.h>

IMPLEMENT_APP_NO_MAIN(wxFlybotDLL)

bool wxFlybotDLL::GetEnabledState()
{
	return m_enabled;
}

void wxFlybotDLL::SwitchState()
{
	m_enabled = !m_enabled;
}

bool wxFlybotDLL::OnInit()
{
	m_enabled = true;
	m_taskBarIcon = NULL;
	m_taskBarIcon = new FlybotTaskBarIcon();
	m_taskBarIcon->SetupIcon();

	// set new logger
	delete wxLog::SetActiveTarget(new wxLogBaloon(m_taskBarIcon));
	
	Dict.Load();
	return true;
}

void wxFlybotDLL::ReloadDictionary()
{
	if (SUCCESS == Dict.Load())
	{
		wxLogMessage(_("Dictionary was successfully loaded"));
	}
}


// TODO: remove as soon as wxWidgets 2.9 released http://trac.wxwidgets.org/ticket/9810
static bool wxLaunchDefaultApplication(const wxString &document)
{
	wxString verb = wxT("open"); 
	int result = (int)ShellExecute(NULL, verb, document, NULL, NULL, SW_SHOWDEFAULT); 

	return result > 32;
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

	// FIXME: enclose m_sessions processing in critical section
	// if it is a new PM, create new session
	wxString cid = userinfo[FLYBOT_API_CID];
	if (NULL == m_sessions[cid])
	{
		m_sessions[cid] = new Session(userinfo);
	}

	// answer pm, according to previous replies, etc.
	m_sessions[cid]->Answer(msg);
}

int wxFlybotDLL::OnExit()
{
	m_taskBarIcon->RemoveIcon();
	delete m_taskBarIcon;

	// free session info
	SessionMap::iterator it;
	for( it = m_sessions.begin(); it != m_sessions.end(); ++it )
	{
		delete it->second;
	}
	m_sessions.clear();

	return 0;
}

wxFlybotDLL::~wxFlybotDLL()
{
}