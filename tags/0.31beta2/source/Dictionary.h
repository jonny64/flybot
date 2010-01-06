#pragma once
#include <wx/dynarray.h>
#include "Phrase.h"

const wxString DICTIONARY_FILENAME = wxT("flydict.ini");

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
