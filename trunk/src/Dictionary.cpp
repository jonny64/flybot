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

static bool ToPriority(const wxString str, int *buf)
{
	long converted = 0;

	bool ok = str.ToLong(&converted, 10);
	bool isPriority = ok && 0 < converted && converted < DICTIONARY_MAX_PRIORITY;
	
	if (isPriority)
	{
		*buf = converted;
	}
	return isPriority;
}

bool Dictionary::ProcessLine(wxString line)
{
	if (line.empty() || line.StartsWith(&DICTIONARY_COMMENT_CHAR))
	{
		return true;
	}

	// split string by / char.
	wxStringTokenizer tokenizer(line, wxT("/"));
	wxArrayString row;
	while (tokenizer.HasMoreTokens() ) 
	{
		row.Add(tokenizer.GetNextToken());
	}

	const int MIN_PARAMS_PER_LINE = 3;
	if (row.Count() < MIN_PARAMS_PER_LINE)
	{
		// TODO: warn user
		return false;
	}
	
	// construct phrase
	Phrase phrase = {0};
	if (!ToPriority(row[0], &phrase.Priority))
	{
		// TODO: warn user
		return false;
	}
	phrase.MatchExpr = row[1];
	phrase.Answer = row[2];
	if (row.Count() > MIN_PARAMS_PER_LINE)
	{
		phrase.Flags = row[3];
	}

	// add newly constructed phrase to collection
	m_phrases.Add(phrase);
	
	return true;
}

int Dictionary::Load()
{
	wxString dictionaryFile = wxStandardPaths::Get().GetPluginsDir() + DICTIONARY_FILENAME;
	wxFileInputStream input(dictionaryFile);
	// TODO: support both for Unicode and ANSI encoded dictionary
	// wxTextInputStream text(input) supposed to work fine in both cases, but...
	wxTextInputStream text(input, wxT("\r\n"), wxCSConv(wxFONTENCODING_CP1251));

	m_phrases.Clear();

	wxString line;	
	while(input.IsOk() && !input.Eof() )
	{
		line = text.ReadLine();
		if (!ProcessLine(line))
		{
			return -1;
		}
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
