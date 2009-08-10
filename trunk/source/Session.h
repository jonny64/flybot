#pragma once
#include "UserInfo.h"
#include "Dictionary.h"
#include "FlybotAPI.h"

class Session
{
    UserInfo m_userinfo;
    ArrayOfPhrases m_usedPhrases;
    wxArrayString m_replies;

    void ProcessFlags(const Phrase&);
    wxString SubstituteVars(const wxString&);
    wxString GetVariable(const wxString&);
public:
    Session(void);
    Session(UserInfo&);

    int Answer(wxString&);

    ~Session(void);
};
