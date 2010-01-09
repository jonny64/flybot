#pragma once
#include "stdwx.h"
#include <wx/fileconf.h>
#include <vector>
using std::vector;

const wxString SETTING_USE_BALLOONS = wxT("EnableBalloons");
const wxString SETTING_SLOT_TIMEOUT = wxT("SlotTimeout");
const wxString SETTING_ANSWER_DELAY = wxT("AnswerDelay");
const wxString SETTING_BOT_ONLINE = wxT("Enabled");
const wxString CONFIG_FILENAME = wxT("flybot.ini");;

class FlybotConfig :
        public wxFileConfig
{
    vector<int> DoReadIntList(const wxString &path);
    void DoWriteIntList(const wxString &path, const vector<int> &lst);
    void ReadSlotTimeouts();
    void ReadAnswerDelays();
    
    vector<int> m_slotTimeouts;
    vector<int> m_answerDelays;
public:
    FlybotConfig();
    
    bool BalloonsEnabled();
    
    int GetSlotTimeoutId();
    int GetAnswerDelayId();
    
    vector<int> GetSlotTimeouts();
    vector<int> GetAnswerDelays();
    
    ~FlybotConfig(void);
};
