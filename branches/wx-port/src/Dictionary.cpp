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
	// skip comments
	if (!line.empty() && line[0]==wxChar(';'))
	{
		return 0;
	}

	// split string by / char. construc phrase
	wxStringTokenizer tokenizer(line, wxT("/"));
	wxArrayString row;
	while (tokenizer.HasMoreTokens() ) 
	{
		row.Add(tokenizer.GetNextToken());
	}

	const int PARAMS_PER_LINE = 4;
	if (row.Count() < PARAMS_PER_LINE)
	{
		// TODO: warn user
		return -1;
	}
	
	Phrase phrase = {0};
	if (!row[0].ToLong(&phrase.Priority))
	{
		// TODO: warn user
		return -1;
	}
	phrase.MatchExpr = row[1];
	phrase.Answer = row[2];
	phrase.Flags = row[3];

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

wxString Dictionary::GetAnswer(wxString& msg)
{
	m_history.Add(msg);

	wxString answer;
	
	// find matches;
	// select one (random) answer from matches, mark it as used;
	
	return answer;
}

Dictionary::~Dictionary(void)
{
}
