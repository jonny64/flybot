#include "stdwx.h"
#include "Dictionary.h"
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/stdpaths.h>
#include <wx/regex.h>

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
	phrase.MatchExpr.empty()? m_phrases.Add(phrase) : m_emptyPhrases.Add(phrase);
	
	return true;
}

wxString Dictionary::GetDictionaryFilename()
{
	return wxStandardPaths::Get().GetPluginsDir() + DICTIONARY_FILENAME;
}

int Dictionary::Load()
{
	wxFileInputStream input(GetDictionaryFilename());
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

static int random(int max)
{
	srand (time(NULL));
	return int((rand()/RAND_MAX)*max);
}

static Phrase SelectAccordingPriority(ArrayOfPhrases &from)
{
	unsigned int sumPrio = 0;
	for (unsigned int i = 0; i<from.Count(); i++)
	{
		sumPrio += int(DICTIONARY_MAX_PRIORITY/from[i].Priority);
	}
	
	unsigned int msgId =  random(sumPrio);
	
	unsigned int i = 0;
	while (true)
	{
		sumPrio = sumPrio + int(DICTIONARY_MAX_PRIORITY/from[i].Priority);
		if (msgId >= sumPrio)
			i++;
		else
			break;
	}

	return from[i];
}

Phrase Dictionary::GetMatchedTemplate(wxString& msg, ArrayOfPhrases *usedPhrases)
{
	wxString answer = wxT("");

	// find matches
	ArrayOfPhrases candidates;
	Phrase p = {0};
	for (unsigned int i = 0; i < m_phrases.Count(); i++)
	{
		p = m_phrases.Item(i);
		wxRegEx phraseRegEx(p.MatchExpr);
		if (phraseRegEx.Matches(msg))
		{
			candidates.Add(p);
		}
	}
	if (candidates.empty())
	{
		candidates = m_emptyPhrases;
	}
	
	Phrase selectedPhrase = SelectAccordingPriority(candidates);

	// TODO: show selected phrase template to user
	usedPhrases->Add(selectedPhrase);
	return selectedPhrase;
}

Dictionary::~Dictionary(void)
{
}
