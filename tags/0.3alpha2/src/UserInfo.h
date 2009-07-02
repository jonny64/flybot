#pragma once

const wxString FLYBOT_API_CID = wxT("CID");
const wxString FLYBOT_API_NICK = wxT("NICK");

class UserInfo
{
	WX_DECLARE_STRING_HASH_MAP(wxString, VarMap);
	VarMap m_vars;
public:
	UserInfo(void);
	UserInfo(WCHAR*);

	wxString& operator[](const wxString&);
	bool Favourite();

	~UserInfo(void);
};
