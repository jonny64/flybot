#pragma once
#include "stdwx.h"
#include "wx/timer.h"
// TODO: make use of wxFileSystemWatcher (wxWidgets 2.9.1)

const int DICTIONARY_WATCH_INTERVAL_MS = 250;
// Periodically checks dictionary file mtime. Reload it when needed
class DictionaryFSWatcher : public wxTimer
{
public:
    DictionaryFSWatcher(void);
    ~DictionaryFSWatcher(void);
    
    void Notify();
};
