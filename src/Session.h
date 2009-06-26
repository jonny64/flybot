#pragma once
#include "UserInfo.h"
#include "Dictionary.h"
#include "FlybotAPI.h"

class Session
{
	UserInfo m_userinfo;
	ArrayOfPhrases m_usedPhrases;
public:
	Session(void);
	Session(UserInfo&);

	int Answer(wxString&);

	~Session(void);
};
