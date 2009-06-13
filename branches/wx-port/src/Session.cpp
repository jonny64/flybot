#include "stdwx.h"
#include "UserInfo.h"
#include "Session.h"

Session::Session(void)
{
}

Session::Session(UserInfo& userinfo)
{
	m_userinfo = userinfo;
}

int Session::Answer(wxString& msg)
{
	// save incoming msg to history;
	// find matches;
	// if no matches, exit;
	// select one (random) answer from matches, mark it as used;
	// replace special vars in answer;
	// process answer additional flags;
	// send answer after desired time interval
	return 0;
}

Session::~Session(void)
{
}
