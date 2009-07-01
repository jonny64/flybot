#include "stdwx.h"
#include "wxFlybotDLL.h"
#include "FlybotConfig.h"

FlybotConfig::FlybotConfig(void): wxConfig(wxT("flybot"))
{
	ReadSlotTimeouts();
}

bool FlybotConfig::BalloonsEnabled()
{
	bool useBalloons = true;
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
		// set defaults
		const int COUNT_DEFAULT_TIMEOUTS = 6;
		int defaults[] = {60, 300, 600, 3600, 86400, 86400*7};
		for (int i = 0; i < COUNT_DEFAULT_TIMEOUTS; i++)
		{
			SlotTimeouts.push_back(defaults[i]);
		}
		DoWriteIntList(SETTING_SLOT_TIMEOUT, SlotTimeouts);
	}
}

int FlybotConfig::SelectedSlotTimeoutId()
{
	int selectedTimeoutId = 0;
	wxGetApp().Config.Read(SETTING_SLOT_TIMEOUT, selectedTimeoutId);
	return selectedTimeoutId;
}

FlybotConfig::~FlybotConfig(void)
{
}
