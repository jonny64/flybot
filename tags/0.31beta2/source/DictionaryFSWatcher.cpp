#include "stdwx.h"
#include "DictionaryFSWatcher.h"

#include "wxFlybotDLL.h"

DECLARE_APP(wxFlybotDLL)

DictionaryFSWatcher::DictionaryFSWatcher(void)
{
}

DictionaryFSWatcher::~DictionaryFSWatcher(void)
{
}

// checks dictionary modification time and reloads it when needed
void DictionaryFSWatcher::Notify()
{
	wxFileName dictionary(FlybotAPI.ConfigPath + DICTIONARY_FILENAME);
    wxDateTime mtime = dictionary.GetModificationTime();
    
    static wxDateTime oldTimestamp = mtime;
	if ( oldTimestamp != mtime )
	{
		wxGetApp().ReloadDictionary();
		oldTimestamp = mtime;
	}
}