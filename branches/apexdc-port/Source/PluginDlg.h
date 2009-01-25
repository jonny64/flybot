/*
 * Copyright (C) 2007-2008 Crise, crise@mail.berlios.de
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#if !defined(PLUGIN_DLG_H)
#define PLUGIN_DLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>
#include <atlwin.h>
#include "resource.h"

class PluginDlg : public CDialogImpl<PluginDlg>
{
public:
	enum { IDD = IDD_PLUGINDLG };

	PluginDlg(Plugin *p) : plugin(p) { }
	~PluginDlg() { }

	BEGIN_MSG_MAP(PluginDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
		// fill values here
		SetDlgItemText(IDC_SUFFIX, plugin->getSetting(_T("SendSuffix")).c_str());

		CenterWindow(GetParent());
		return TRUE;
	}
		
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		if(wID == IDOK) {
			// save values here
			TCHAR buf[256];
			GetDlgItemText(IDC_SUFFIX, buf, 256);
			plugin->setSetting(_T("SendSuffix"), buf);
		}

		EndDialog(wID);
		return 0;
	}

private:
	Plugin* plugin;
	PluginDlg(const PluginDlg&) { }
};

#endif // !defined(PLUGIN_DLG_H)