#pragma once
#include "UserInfo.h"
#include "Dictionary.h"

class Session
{
	UserInfo m_userinfo;
	Dictionary m_dictionary;
public:
	Session(void);
	Session(UserInfo&);

	int Answer(wxString&);

	~Session(void);
};
