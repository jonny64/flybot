#include "stdwx.h"
#include "Dictionary.h"
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/stdpaths.h>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayOfPhrases);

Dictionary::Dictionary(void)
{
	Load();
}

int Dictionary::ProcessLine(wxString line)
{
	Phrase phrase;

	// split string by / char. construc phrase
	wxStringTokenizer tokenizer(line, wxT("/"));
	while (tokenizer.HasMoreTokens() ) 
	{
		wxString value = tokenizer.GetNextToken();
	}

	// add newly constructed phrase to collection
	m_phrases.Add(phrase);
	
	return 0;
}

int Dictionary::Load()
{
	wxString dictionaryFile = wxStandardPaths::Get().GetPluginsDir() + DICTIONARY_FILENAME;
	wxFileInputStream input(dictionaryFile);
	wxTextInputStream text(input);

	m_phrases.Clear();

	wxString line;	
	while(input.IsOk() && !input.Eof() )
	{
		text >> line;
		ProcessLine(line);
	}

	return 0;

}

Dictionary::~Dictionary(void)
{
}
