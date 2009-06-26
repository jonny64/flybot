#pragma once
#include "wx/wxprec.h"

#include "wx/msw/private.h"
#include "wx/wx.h"
#include <wx/tokenzr.h>

#include "wx/taskbar.h"

// debug memory allocation enhancement
#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif