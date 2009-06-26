#pragma once
#include <wx/dynarray.h>
#include "Phrase.h"

const wxString DICTIONARY_FILENAME = wxT("\\Settings\\flydict.ini");
const long DICTIONARY_MAX_PRIORITY = 1000;
const wxChar DICTIONARY_COMMENT_CHAR = wxChar(';');

WX_DECLARE_OBJARRAY(Phrase, ArrayOfPhrases);

class Dictionary
{
	ArrayOfPhrases m_phrases;

	bool ProcessLine(wxString);
public:
	Dictionary(void);

	int Load();
	wxString GetMatchedTemplate(wxString&, ArrayOfPhrases*);

	~Dictionary(void);
};
