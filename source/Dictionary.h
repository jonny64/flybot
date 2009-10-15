#pragma once
#include <wx/dynarray.h>
#include "Phrase.h"

const wxString DICTIONARY_FILENAME = wxT("flydict.ini");
const long DICTIONARY_MAX_PRIORITY = 1000;
const wxChar DICTIONARY_COMMENT_CHAR = wxChar(';');
const wxChar DICTIONARY_CLOSE_CHAR = wxChar('c');
const wxChar DICTIONARY_IGNORE_CHAR = wxChar('i');
const wxChar DICTIONARY_SLOT_CHAR = wxChar('s');
const wxChar DICTIONARY_SEPARATION_CHAR = wxChar('/');
const wxChar DICTIONARY_ESCAPE_CHAR = wxChar('#');

WX_DECLARE_OBJARRAY(Phrase, ArrayOfPhrases);

class Dictionary
{
    ArrayOfPhrases m_phrases;
    ArrayOfPhrases m_emptyPhrases;

    bool ProcessLine(const wxString &line, wxString *errorMessage);
public:
    Dictionary(void);

    int Load();
    static wxString GetDictionaryFilename();
    Phrase GetMatchedTemplate(const wxString& msg, ArrayOfPhrases* usedPhrases);

    ~Dictionary(void);
};
