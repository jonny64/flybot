#pragma once
#include <wx/wxprec.h>
#include <wx/msw/private.h>
#include <wx/thread.h>
#include <wx/wx.h>
#include <wx/tokenzr.h>
#include <wx/taskbar.h>
#include <wx/config.h>


// memory allocation enhancement
#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif

const int SUCCESS = 0;
const int IO_FAILURE = -1;

int random(int max);


#define FOREACH(type, it, container) \
    for (type::iterator (it) = (container).begin(); (it) != (container).end(); ++(it))

#define FOREACH_CONST(type, it, container) \
    for (type::const_iterator (it) = (container).begin(); (it) != (container).end(); ++(it))