#pragma once

class UserInfo
{
	WX_DECLARE_STRING_HASH_MAP(wxString, VarMap);
	VarMap m_vars;
public:
	UserInfo(void);
	UserInfo(WCHAR*);

	wxString& operator[](const wxString&);

	~UserInfo(void);
};
