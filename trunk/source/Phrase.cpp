#include "stdwx.h"
#include <wx/regex.h>
#include "Phrase.h"

static bool ToPriority(const wxString &str, int *buf)
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

Phrase::Phrase(const wxString &line)
{
    // check if line is like 'priority/match_and_answer/optional_flags'
    wxString r = wxString::Format(
                     "^(\\d{1,})%c(.*?)(:?%c([cisl]+))?$",
                     DICTIONARY_SEPARATION_CHAR,
                     DICTIONARY_SEPARATION_CHAR
                 );
    wxRegEx reLine(r, wxRE_ADVANCED);
    if (!reLine.Matches(line))
    {
        ErrorMsg = wxString::Format(
                       _("invalid line format..")
                   );
        m_valid = false;
        return;
    }
    wxASSERT(reLine.GetMatchCount() >= 3);
    
    // construct phrase: initialize priority and flags
    wxString priorityString = reLine.GetMatch(line, 1);
    if (!ToPriority(priorityString, &Priority))
    {
        ErrorMsg = wxString::Format(
                       _("priority should be in range 1..%d"),
                       DICTIONARY_MAX_PRIORITY
                   );
        m_valid = false;
        return;
    }
    Flags = reLine.GetMatch(line, 4);
    
    // asssume that anwer does not contain /
    wxString templateAndAnwer = reLine.GetMatch(line, 2);
    r = wxString::Format(
            "(.+)%c([^%c]*)",
            DICTIONARY_SEPARATION_CHAR,
            DICTIONARY_SEPARATION_CHAR
        );
    wxRegEx reSplit(r);
    if (!reSplit.Matches(templateAndAnwer))
    {
        ErrorMsg = wxString::Format(_("missing template or answer"));
        m_valid = false;
        return;
    }
    wxASSERT(reSplit.GetMatchCount() == 3);
    
    // TODO: embed compiled regexp into phrase struct (this may result in faster phrase match)
    // fill phrase regexp and answer fields
    MatchExpr = reSplit.GetMatch(templateAndAnwer, 1);
    if (!wxRegEx(MatchExpr, wxRE_ADVANCED).IsValid())
    {
        ErrorMsg = wxString::Format(
                       _("invalid regular expression:\n%s"),
                       MatchExpr
                   );
        m_valid = false;
        return;
    }
    Answer = reSplit.GetMatch(templateAndAnwer, 2);
    
    m_valid = true;
}


bool Phrase::Matches(const wxString &PM) const
{
    wxRegEx phraseRegEx(MatchExpr, wxRE_ADVANCED);
    return phraseRegEx.Matches(PM);
}

wxString Phrase::ToString() const
{
    return wxString::Format(
               wxT("%i%c%s%c%s%c%s"),
               Priority,
               DICTIONARY_SEPARATION_CHAR,
               MatchExpr,
               DICTIONARY_SEPARATION_CHAR,
               Answer,
               DICTIONARY_SEPARATION_CHAR,
               Flags
           );
}

bool Phrase::Empty() const
{
    return Flags.empty() && Answer.empty();
}

bool Phrase::Valid() const
{
    return m_valid;
}