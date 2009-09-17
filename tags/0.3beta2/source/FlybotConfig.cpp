#include "stdwx.h"
#include "wxFlybotDLL.h"

FlybotConfig::FlybotConfig(void): wxConfig(wxT("flybot"))
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

list<int> FlybotConfig::DoReadIntList(const wxString &path)
{
    list<int> result;
    wxConfig *conf = &wxGetApp().Config;

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

void FlybotConfig::DoWriteIntList(const wxString &path, const list<int> &lst)
{
    wxConfig *conf = &wxGetApp().Config;

    int index = 0;
    for (list<int>::const_iterator it = lst.begin(); it != lst.end(); ++it)
    {
        conf->Write(wxString::Format(wxT("%s/value%d"), path, index), *it);
        index++;
    }
}

void FlybotConfig::ReadSlotTimeouts()
{
    SlotTimeouts = DoReadIntList(SETTING_SLOT_TIMEOUT);
    
    if (SlotTimeouts.empty())
    {
        // default slot timeouts
        int defaults[] = {600, 3600, 7200, 86400};
        for (int i = 0; i < ARRAYSIZE(defaults); i++)
        {
            SlotTimeouts.push_back(defaults[i]);
        }
        DoWriteIntList(SETTING_SLOT_TIMEOUT, SlotTimeouts);
    }
}

void FlybotConfig::ReadAnswerDelays()
{
    AnswerDelays = DoReadIntList(SETTING_ANSWER_DELAY);
    
    if (AnswerDelays.empty())
    {
        // set defaults
        int defaults[] = {0, 6, 40};
        for (int i = 0; i < ARRAYSIZE(defaults); i++)
        {
            AnswerDelays.push_back(defaults[i]);
        }
        DoWriteIntList(SETTING_ANSWER_DELAY, AnswerDelays);
    }
}

int FlybotConfig::GetSelectedSlotTimeout()
{
    list<int>::iterator it = SlotTimeouts.begin();
    std::advance(it, GetSelectedSlotTimeoutId());
    return *it;
}

int FlybotConfig::GetSelectedAnswerDelay()
{
    list<int>::iterator it = AnswerDelays.begin();
    std::advance(it, GetSelectedAnswerDelayId());
    return *it;
}

int FlybotConfig::GetSelectedAnswerDelayId()
{
    int selectedAnswerDelayId = 0;
    Read(SETTING_ANSWER_DELAY, &selectedAnswerDelayId );
    return selectedAnswerDelayId;
}

void SetSelectedAnswerDelayId(int id)
{
    wxGetApp().Config.Write(SETTING_ANSWER_DELAY, id);
}

int FlybotConfig::GetSelectedSlotTimeoutId()
{
    int selectedTimeoutId = 0;
    Read(SETTING_SLOT_TIMEOUT, &selectedTimeoutId);
    return selectedTimeoutId;
}

void FlybotConfig::SetSelectedSlotTimeoutId(int id)
{
    Write(SETTING_SLOT_TIMEOUT, id);
}

void FlybotConfig::SetSelectedAnswerDelayId(int id)
{
    Write(SETTING_ANSWER_DELAY, id);
}

FlybotConfig::~FlybotConfig(void)
{
}
