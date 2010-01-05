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


void DictionaryFSWatcher::Notify()
{
	static __time64_t oldTimestamp = 0;
	struct	__stat64 fileinfo;
	wxString dictionaryFullPath = FlybotAPI.ConfigPath + DICTIONARY_FILENAME;
	
	if (0 == _stat64(dictionaryFullPath.mb_str(wxConvUTF8), &fileinfo) )
	{
		if (fileinfo.st_mtime != oldTimestamp)
		{
			// dictionary file changed by user
			wxGetApp().ReloadDictionary();
			oldTimestamp = fileinfo.st_mtime;
		}
	}
}