#include "stdwx.h"
#include "wxFlybotDLL.h"
#include <wx/stdpaths.h>

FlybotConfig::FlybotConfig():
    wxFileConfig(
        wxT("flybot"),
        wxT("astro64m"), 
        FlybotAPI.ConfigPath + CONFIG_FILENAME, 
        wxT(""), 
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

vector<int> FlybotConfig::DoReadIntList(const wxString &path)
{
    vector<int> result;
    wxFileConfig *conf = &wxGetApp().Config;

    // read settings
    int val = 0;
    int i = 0;
    while (conf->Read(
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
    wxFileConfig *conf = &wxGetApp().Config;

    int index = 0;
    FOREACH_CONST(vector<int>, it, lst)
    {
        conf->Write(wxString::Format(wxT("%s/value%d"), path, index), *it);
        index++;
    }
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
