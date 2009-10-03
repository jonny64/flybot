#include "stdwx.h"
#include "Dictionary.h"
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/stdpaths.h>
#include <wx/regex.h>
#include "wxLogBalloon.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayOfPhrases);

Dictionary::Dictionary(void)
{
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

bool Dictionary::ProcessLine(const wxString &line, wxString *errorMessage)
{
    if (line.empty() || line.StartsWith(&DICTIONARY_COMMENT_CHAR))
    {
        return true;
    }

    // split string by / char.
    wxArrayString row = wxSplit(line, DICTIONARY_SEPARATION_CHAR, DICTIONARY_ESCAPE_CHAR);

    const int MIN_PARAMS_PER_LINE = 3;
    if (row.Count() < MIN_PARAMS_PER_LINE)
    {
        *errorMessage = _("too few parameters");
        return false;
    }
    
    // construct phrase
    Phrase phrase = {0};
    if (!ToPriority(row[0], &phrase.Priority))
    {
        *errorMessage = wxString::Format(_("priority should be in range 1..%d"), DICTIONARY_MAX_PRIORITY);
        return false;
    }

    // TODO: embed compiled regexp into phrase struct (this may result in faster phrase match)
    phrase.MatchExpr = row[1];
    if (!wxRegEx(phrase.MatchExpr).IsValid())
    {
        *errorMessage = wxString::Format(_("invalid regular expression:\n%s"), phrase.MatchExpr);
        return false;
    }
    phrase.Answer = row[2];
    if (row.Count() > MIN_PARAMS_PER_LINE)
    {
        phrase.Flags = row[3];
    }

    // add newly constructed phrase to collection
    phrase.MatchExpr.empty()? m_emptyPhrases.Add(phrase) : m_phrases.Add(phrase);
    
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
            wxLogWarning(wxString::Format(_("Dictionary, line %d:"), row), 
                errorMessage);
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

    Phrase selectedPhrase = {0};
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
