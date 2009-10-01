#include "stdwx.h"
#include "UserInfo.h"

UserInfo::UserInfo(WCHAR* userinfo)
{
    UserInfo();
    
    wxString info(userinfo);
    if (info.empty())
        return;

    // split string by | char
    wxArrayString pairs = wxSplit(info, '|', '#');
    for (size_t i = 0; i < pairs.Count(); i++)
    {
        wxArrayString pair = wxSplit(pairs[i], '=', '#');
        if (2 == pair.Count())
        {
            wxString varname = pair[0];
            wxString value = pair[1];
            if (!varname.empty() && !value.empty())
            {
                m_vars[varname] = value;
            }
        }
    }
}

UserInfo::UserInfo(void)
{
    m_vars.clear();
}

wxString UserInfo::operator[](const wxString& key)
{
    if (m_vars[key])
    {
        return m_vars[key];
    }
    return wxT("");
}

UserInfo& UserInfo::operator=(const UserInfo& rhs)
{
    m_vars = VarMap(rhs.m_vars);
    return *this;
}

bool UserInfo::Favourite()
{
    return wxT("1") == m_vars[wxT("ISFAV")];
}

UserInfo::~UserInfo(void)
{
}
