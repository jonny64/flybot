#include "stdwx.h"
#include "wxFlybotDLL.h"

IMPLEMENT_APP_NO_MAIN(wxFlybotDLL)

bool wxFlybotDLL::OnInit()
{
	m_taskBarIcon = NULL;
	m_taskBarIcon = new MyTaskBarIcon();

	// TODO: redirect logging to custom BaloonLogger class derived from wxLog;
	// set it as default logger with wxLog::SetActiveTarget()

	m_taskBarIcon->SwitchIcon();
	return true;
}

void wxFlybotDLL::ReloadDictionary()
{
	m_dictionary.Load();
}

void wxFlybotDLL::HandlePM(UserInfo& userinfo, wxString& msg)
{
	// do not process favourites
	if ( wxT("1") == userinfo[wxT("ISFAV")] )
		return;

	// FIXME: enclose m_sessions processing in critical section
	// if it is a new PM, create new session
	wxString cid = userinfo[wxT("CID")];
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
