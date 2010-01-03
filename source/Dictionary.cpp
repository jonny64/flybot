#include "stdwx.h"
#include "Dictionary.h"
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include "wxLogBalloon.h"
#include "wxFlybotDLL.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayOfPhrases);

Dictionary::Dictionary(void)
{
}

bool Dictionary::ProcessLine(const wxString &line, wxString *errorMessage)
{
    if (line.empty() || line.StartsWith(&DICTIONARY_COMMENT_CHAR))
    {
        return true;
    }

    Phrase phrase(line);
    if (!phrase.Valid())
    {
        *errorMessage = phrase.ErrorMsg;
        return false;
    }
    
    phrase.MatchExpr.empty()? m_emptyPhrases.Add(phrase) : m_phrases.Add(phrase);
    return true;
}

wxString Dictionary::GetDictionaryFilename()
{
    return FlybotAPI.ConfigPath + DICTIONARY_FILENAME;
}

int Dictionary::Load()
{
    wxString dictionaryFileName = GetDictionaryFilename();
    wxFileInputStream input(dictionaryFileName);
    if (!input.Ok())
    {
        wxLogError(
			MESSAGE_WITH_TITLE(
				_("Cannot open dictionary for reading"), 
				wxString::Format(_("Path: %s"), dictionaryFileName)
            )
		);
        return IO_FAILURE;
    }

    // TODO: support both for Unicode and ANSI encoded dictionary
    // wxTextInputStream text(input) supposed to work fine in both cases, but...
    wxTextInputStream text(input, wxT("\r\n"), wxCSConv(wxFONTENCODING_CP1251));

    m_phrases.Clear();
    m_emptyPhrases.Clear();

    wxString line = wxT("");
    int row = 0;
    int errorsCount = 0;
    while(input.IsOk() && !input.Eof() )
    {
        row++;
        line = text.ReadLine();
        wxString errorMessage = wxT("");
        if (!ProcessLine(line, &errorMessage))
        {
            wxLogWarning(
                wxString::Format(_("Dictionary, line %d:"), row), 
                wxString::Format(wxT("%s\n%s"), line, errorMessage)
                );
            errorsCount++;
        }
    }

    return errorsCount;
}

static Phrase SelectAccordingPriority(ArrayOfPhrases &candidates)
{
    // map priority weight 1, 2, 3 => 1000, 500, 333
    // compute length of priority weight segment: in our example it is 1833
    unsigned int length = 0;
    for (unsigned int i = 0; i<candidates.Count(); i++)
    {
        length += int(DICTIONARY_MAX_PRIORITY/candidates[i].Priority);
    }
    
    // drop a point to segment, e.g. 700
    unsigned int point =  random(length);
    
    // find corresponded phrase
    unsigned int i = 0;
    unsigned int currPoint = int(DICTIONARY_MAX_PRIORITY/candidates[i].Priority);
    while (point >= currPoint)
    {
        i++;
        currPoint = currPoint + int(DICTIONARY_MAX_PRIORITY/candidates[i].Priority);
    }

    wxASSERT(i < candidates.Count());
    return candidates[i];
}

Phrase Dictionary::GetMatchedTemplate(const wxString& msg, ArrayOfPhrases *usedPhrases)
{
    wxString answer = wxEmptyString;

    // find matches
    ArrayOfPhrases candidates;
    for (unsigned int i = 0; i < m_phrases.Count(); i++)
    {
        if (m_phrases.Item(i).Matches(msg))
        {
            candidates.Add(m_phrases.Item(i));
        }
    }

    if (candidates.empty())
    {
        candidates = m_emptyPhrases;
    }

    Phrase selectedPhrase;
    if (!candidates.empty())
    {
        selectedPhrase = SelectAccordingPriority(candidates);
        usedPhrases->Add(selectedPhrase);
    }
    return selectedPhrase;
}

Dictionary::~Dictionary(void)
{
}
