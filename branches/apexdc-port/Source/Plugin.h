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

#ifndef PLUGIN_H
#define PLUGIN_H

#ifdef _WIN32
# define snprintf _snprintf
# define snwprintf _snwprintf
#endif

#include "version.h"
#include <Singleton.h>
#include <PluginStructure.h>

using namespace std;

class Plugin : public PluginStructure, public dcpp::Singleton<Plugin>
{
public:
	Plugin() { }
	~Plugin() throw() { }

	bool onLoad();
	void onUnload();
	void onHubConnected(const ClientInterface* client);
	void onHubEnter(ClientInterface* client, string& text);
	void setDefaults();
	bool onConfig(HWND hWnd);

	const string getName() const { return PLUGIN_NAME; };
	const string getAuthor() const { return PLUGIN_AUTHOR; };
	const string getDescription() const { return PLUGIN_DESC; }
	const string getGuid() const { return PLUGIN_GUID; }
	double getVersion() { return PLUGIN_VERSION; };

private:
	static string fromDouble(double d);

	friend class dcpp::Singleton<Plugin>;
	static Plugin* instance;
};

#endif // PLUGIN_H