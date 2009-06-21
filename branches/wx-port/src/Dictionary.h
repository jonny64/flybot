#pragma once
#include <wx/dynarray.h>
#include "Phrase.h"

const wxString DICTIONARY_FILENAME = wxT("\\Settings\\flydict.ini");

WX_DECLARE_OBJARRAY(Phrase, ArrayOfPhrases);

class Dictionary
{
	ArrayOfPhrases m_phrases;

	int ProcessLine(wxString);
public:
	Dictionary(void);

	int Load();

	~Dictionary(void);
};
