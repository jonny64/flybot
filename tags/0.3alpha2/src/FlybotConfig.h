#pragma once
#include "stdwx.h"

const wxString SETTING_USE_BALLOONS = wxT("EnableBalloons");
const wxString SETTING_SLOT_TIMEOUT = wxT("SlotTimeout");

class FlybotConfig :
	public wxConfig
{
	list<int> DoReadIntList(const wxString &path);
	void DoWriteIntList(const wxString &path, const list<int> &lst);
	void ReadSlotTimeouts();
public:
	FlybotConfig(void);
	
	list<int> SlotTimeouts;
	bool BalloonsEnabled();
	int GetSelectedSlotTimeout();
	int GetSelectedSlotTimeoutId();
	void SetSelectedSlotTimeoutId(int id);

	~FlybotConfig(void);
};
