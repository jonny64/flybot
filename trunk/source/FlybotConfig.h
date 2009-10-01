#pragma once
#include "stdwx.h"
#include <wx/fileconf.h>

const wxString SETTING_USE_BALLOONS = wxT("EnableBalloons");
const wxString SETTING_SLOT_TIMEOUT = wxT("SlotTimeout");
const wxString SETTING_ANSWER_DELAY = wxT("AnswerDelay");
const wxString SETTING_BOT_ONLINE = wxT("Enabled");
const wxString CONFIG_FILENAME = wxT("\\Settings\\flybot.ini");;

class FlybotConfig :
    public wxFileConfig
{
    list<int> DoReadIntList(const wxString &path);
    void DoWriteIntList(const wxString &path, const list<int> &lst);
    void ReadSlotTimeouts();
    void ReadAnswerDelays();
public:
    FlybotConfig(void);
    
    list<int> SlotTimeouts;
    list<int> AnswerDelays;
    bool BalloonsEnabled();
    int GetSelectedSlotTimeout();
    int GetSelectedAnswerDelay();

    int GetSelectedSlotTimeoutId();
    int GetSelectedAnswerDelayId();
    void SetSelectedSlotTimeoutId(int id);
    void SetSelectedAnswerDelayId(int id);

    ~FlybotConfig(void);
};
