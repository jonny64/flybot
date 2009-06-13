#pragma once
#include "UserInfo.h"

class Session
{
	UserInfo m_userinfo;
public:
	Session(void);
	Session(UserInfo&);

	int Answer(wxString&);

	~Session(void);
};
