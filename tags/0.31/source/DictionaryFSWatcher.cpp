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

//called every DICTIONARY_WATCH_INTERVAL_MS milliseconds
void DictionaryFSWatcher::Notify()
{
    wxFileName dictionary(FlybotAPI.ConfigPath + DICTIONARY_FILENAME);
    wxDateTime mtime = dictionary.GetModificationTime();
    
    static wxDateTime oldTimestamp = mtime;
    if (oldTimestamp != mtime)
    {
        wxGetApp().ReloadDictionary();
        oldTimestamp = mtime;
    }
}