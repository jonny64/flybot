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

#include "stdafx.h"
#include "Plugin.h"
#include "PluginDlg.h"

Plugin* Plugin::instance = NULL;

// Little greeting from us :)
bool Plugin::onLoad() {
	getCallBack()->logMessage("*** " + this->getName() + " loaded! (see /pluginhelp for more info)");
	getCallBack()->sendHubs(true, this); // request data about hubs connected to
	return true;
}

void Plugin::onUnload() {
	getCallBack()->sendHubs(false, this); // cancel above request
}

void Plugin::onHubConnected(const ClientInterface* client) {
	getCallBack()->logMessage("*** " + client->getHubUrl() + " connected!");
}

void Plugin::onHubEnter(ClientInterface* client, string& text) {
	if(text[0] == '/') {
		string cmd, param;
		string::size_type i = text.find(" ");
		if(i != string::npos) {
			param = text.substr(i+1);
			cmd = text.substr(1, i-1);
		} else {
			cmd = text.substr(1);
		}

		if(stricmp(cmd.c_str(), "pluginhelp") == 0) {
			client->addClientLine("\t\t\t -= PLUGIN HELP =- \t\t\t\r\n\t /plugininfo \t\t\t Prints info about the sample plugin\r\n\t /send <text> \t\t\t Chat message test\r\n", PluginStructure::SYSTEM_MESSAGE);
			text = "";
		} else if(stricmp(cmd.c_str(), "plugininfo") == 0) {
			client->addClientLine("\t\t\t -= PLUGIN INFO =- \t\t\t\r\n\t Name: \t\t\t\t" + this->getName() + "\r\n\t Author: \t\t\t" + this->getAuthor() + "\r\n\t Version: \t\t\t" + Plugin::fromDouble(this->getVersion()) + "\r\n\t Description: \t\t\t" + this->getDescription() + "\r\n\t GUID/UUID: \t\t\t" + this->getGuid() + "\r\n", PluginStructure::SYSTEM_MESSAGE);
			text = "";
		} else if(stricmp(cmd.c_str(), "send") == 0) {
			if(!param.empty()) {
				text = param + " " + this->getSetting("SendSuffix");
			} else {
				client->addClientLine("You must supply a parameter!", PluginStructure::SYSTEM_MESSAGE);
				text = "";
			}
		}
	}
}

// Configuration dialog
// if you do not implent this, or return false results in 'no configuration' message
bool Plugin::onConfig(HWND hWnd) {
	PluginDlg dlg(this);
	dlg.DoModal(hWnd);
	return true;
}

// Settings defaults
void Plugin::setDefaults() {
	this->setSetting("SendSuffix", "<ApexDC++ Plugins Test>");	// /send commad suffix default
}

// Utility functions (private)
string Plugin::fromDouble(double d) {
	char buf[16];
	snprintf(buf, sizeof(buf), "%0.2f", d);
	return buf;
}