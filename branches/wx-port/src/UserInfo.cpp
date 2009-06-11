#include "stdwx.h"
#include <wx/tokenzr.h>
#include "UserInfo.h"

UserInfo::UserInfo(WCHAR* userinfo)
{
	wxString varname, value;
	wxString info(userinfo);

	// split string by | char
	wxStringTokenizer tokenizer(info, wxT("|"));
	while (tokenizer.HasMoreTokens() ) {
		wxString token = tokenizer.GetNextToken();		

		// split value and varname
		// FIXME: carefully investigate empty var case
		varname = token.SubString( 0, token.Index(wxT('=')) - 1 );
		value = token.SubString( token.Index(wxT('=')) + 1, token.Length() );
		m_vars[varname] = value;
	}
}

UserInfo::UserInfo(void)
{
}

wxString& UserInfo::operator[](const wxString& key)
{
	return m_vars[key];
}

UserInfo::~UserInfo(void)
{
}
