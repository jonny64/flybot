#include "stdwx.h"
#include "wxFlybotDLL.h"
#include <wx/stdpaths.h>

FlybotConfig::FlybotConfig(void):
    wxFileConfig(
        wxT("flybot"),
        wxT("astro64m"), 
        wxStandardPaths::Get().GetPluginsDir() + CONFIG_FILENAME, 
        wxEmptyString, 
        wxCONFIG_USE_LOCAL_FILE
        )
{
    ReadSlotTimeouts();
    ReadAnswerDelays();
}

bool FlybotConfig::BalloonsEnabled()
{
    bool useBalloons = true;  // use baloon notifications by default
    Read(SETTING_USE_BALLOONS, &useBalloons, true);
    return useBalloons;
}

wxArrayString FlybotConfig::DoReadStringList(const wxString &path)
{
    wxArrayString result;
    wxString buf;
    buf.Printf(wxT("%s/value%d"), path, 1);

    wxString val;
    int i = 0;
    while ((wxFileConfig *)this->Read(
            wxString::Format(wxT("%s/value%d"), path, i),
            &val
            )
        )
    {
        result.Add(val);
        i++;
    }
    return result;
}

vector<int> FlybotConfig::DoReadIntList(const wxString &path)
{
    vector<int> result;
    
    int val = 0;
    int i = 0;
    while ((wxFileConfig *)this->Read(
            wxString::Format(wxT("%s/value%d"), path, i),
            &val
            )
        )
    {
        result.push_back(val);
        i++;
    }

    return result;
}

void FlybotConfig::DoWriteIntList(const wxString &path, const vector<int> &lst)
{
    int index = 0;
    FOREACH_CONST(vector<int>, it, lst)
    {
        (wxFileConfig *)this->Write(wxString::Format(wxT("%s/value%d"), path, index), (int)*it);
        index++;
    }
}

void FlybotConfig::AddNickHistory(const wxString &nick)
{
    wxString val;
    int i = 0;
    while ((wxFileConfig *)this->Read(
            wxString::Format(wxT("%s/value%d"), SETTING_NICK_HISTORY, i),
            &val
            )
        )
    {
        i++;
    }

    wxString path = wxString::Format(wxT("%s/value%d"), SETTING_NICK_HISTORY, i);
    Write(path, nick);
}

wxArrayString FlybotConfig::GetNickHistory()
{
    return DoReadStringList(SETTING_NICK_HISTORY);
}

void FlybotConfig::ReadSlotTimeouts()
{
    m_slotTimeouts = DoReadIntList(SETTING_SLOT_TIMEOUT);
    
    if (m_slotTimeouts.empty())
    {
        // default slot timeouts
        int defaults[] = {600, 3600, 3600*5, 86400};
        for (int i = 0; i < ARRAYSIZE(defaults); i++)
        {
            m_slotTimeouts.push_back(defaults[i]);
        }
        DoWriteIntList(SETTING_SLOT_TIMEOUT, m_slotTimeouts);
    }
}

void FlybotConfig::ReadAnswerDelays()
{
    m_answerDelays = DoReadIntList(SETTING_ANSWER_DELAY);
    
    if (m_answerDelays.empty())
    {
        // set defaults
        int defaults[] = {0, 6, 40};
        for (int i = 0; i < ARRAYSIZE(defaults); i++)
        {
            m_answerDelays.push_back(defaults[i]);
        }
        DoWriteIntList(SETTING_ANSWER_DELAY, m_answerDelays);
    }
}

vector<int> FlybotConfig::GetSlotTimeouts()
{
    return m_slotTimeouts;
}

vector<int> FlybotConfig::GetAnswerDelays()
{
    return m_answerDelays;
}

int FlybotConfig::GetAnswerDelayId()
{
    int answerDelayId = 0;
    Read(SETTING_ANSWER_DELAY, &answerDelayId );
    return answerDelayId;
}

int FlybotConfig::GetSlotTimeoutId()
{
    int timeoutId = 0;
    Read(SETTING_SLOT_TIMEOUT, &timeoutId);
    return timeoutId;
}

FlybotConfig::~FlybotConfig(void)
{
}
