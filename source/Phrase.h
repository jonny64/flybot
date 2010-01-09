#pragma once

const long DICTIONARY_MAX_PRIORITY = 1000;
const wxChar DICTIONARY_COMMENT_CHAR = wxChar(';');
const wxChar DICTIONARY_CLOSE_CHAR = wxChar('c');
const wxChar DICTIONARY_IGNORE_CHAR = wxChar('i');
const wxChar DICTIONARY_SLOT_CHAR = wxChar('s');
const wxChar DICTIONARY_LOG_CHAR = wxChar('l');
const wxChar DICTIONARY_SEPARATION_CHAR = wxChar('/');
const wxChar DICTIONARY_ESCAPE_CHAR = wxChar('#');

class Phrase
{
    bool m_valid;
public:
    Phrase(const wxString &line);
    Phrase() : MatchExpr(wxEmptyString), Answer(wxEmptyString), Flags(wxEmptyString), ErrorMsg(wxEmptyString) {}
    
    int Priority;
    wxString MatchExpr;
    wxString Answer;
    wxString Flags;
    wxString ErrorMsg;
    
    bool Empty() const;
    bool Valid() const;
    bool Matches(const wxString &PM) const;
    wxString ToString() const;
};
