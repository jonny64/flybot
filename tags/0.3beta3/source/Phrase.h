#pragma once

struct Phrase
{
    int Priority;
    wxString MatchExpr;
    wxString Answer;
    wxString Flags;

    bool empty()
    {
        return Flags.empty() && Answer.empty();
    }
};
