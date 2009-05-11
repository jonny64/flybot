#include "wx/wxprec.h"

#include "wx/msw/private.h"
#include "wx/wx.h"
#include "wx/taskbar.h"
#include "ChatBotAPI.h"

// debug memory allocation enhancement (see next tip)
#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif