#pragma once
#include <wx/wxprec.h>
#include <wx/msw/private.h>
#include <wx/thread.h>
#include <wx/wx.h>
#include <wx/tokenzr.h>
#include <wx/taskbar.h>
#include <wx/config.h>
#include <list>
using std::list;


// memory allocation enhancement
#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif

const int SUCCESS = 0;

// TODO: remove after wxWidgets 2.9 release
// http://trac.wxwidgets.org/ticket/8370
wxArrayString wxSplit(const wxString& str, const wxChar sep, const wxChar escape);

// TODO: remove as soon as wxWidgets 2.9 released 
// http://trac.wxwidgets.org/ticket/9810
bool wxLaunchDefaultApplication(const wxString &document);

int random(int max);
