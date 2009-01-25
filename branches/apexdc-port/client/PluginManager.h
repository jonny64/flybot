/* 
 * Copyright (C) 2003 Twink, spm7@waikato.ac.nz
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

/*
 * Updated @ 2007-2008, by Crise
 */

#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include "Singleton.h"
#include "SettingsManager.h"
#include "TimerManager.h"
#include "ClientManager.h"
#include "CriticalSection.h"
#include "PluginStructure.h"

namespace dcpp {

// Implementation of the Callback interface, plugins call functions in this
class PluginCallBack : public PluginCallBackInterface
{
public:
	HWND getMainWindow();
	void debugMessage(const string& messsage);
	void logMessage(const string& message);
	void runTimer(bool state, PluginStructure* plugin);
	void sendHubs(bool state, PluginStructure* plugin);
	void release() const { delete this; };

	// String conversions
	string fromUtf8(const string& str, string& tmp);
	string toUtf8(const string& str, string& tmp);

	// Settings
	void setSetting(const string& pluginName, const string& setting, const string& value);
	void setSetting(const string& pluginName, const tstring& setting, const tstring& value);

	string getSetting(const string& pluginName, const string& setting);
	tstring getSetting(const string& pluginName, const tstring& setting);
};

// Holds a loaded plugin
class PluginInfo
{
public:
	typedef PluginInfo* Ptr;
	typedef vector<Ptr> pluginList;

	typedef PluginStructure*	(*PLUGIN_LOAD)();
	typedef double				(*PLUGIN_VER)();
	typedef char*				(*PLUGIN_GUID)();
	typedef void				(*PLUGIN_UNLOAD)();

	PluginInfo(pluginHandle hInst, PLUGIN_LOAD aInit, PLUGIN_UNLOAD aFree) : 
		handle(hInst), newObject(aInit), freeObject(aFree), object(NULL) { };

	~PluginInfo() throw();

	struct MetaData { 
		string name;
		string author;
		string desc;
		double ver;
		int icon;
	};

	void sleep();
	bool awaken(bool bFull = true);
	bool getLoaded() const { return object != NULL; }
	const MetaData& getMetaData() const { return info; }

	PluginStructure* object;

private:
	MetaData info;
	pluginHandle handle;
	PLUGIN_UNLOAD freeObject;
	PLUGIN_LOAD newObject;
};

class PluginManager : public Singleton<PluginManager>, private SettingsManagerListener,
	private ClientManagerListener, private TimerManagerListener
{
public:
	typedef vector<pair<PluginInfo::MetaData, bool>> MetaDataList;

	PluginManager() : mainWnd(NULL) {
		SettingsManager::getInstance()->addListener(this);
		loadPlugins();
	}

	~PluginManager() throw() {
		unloadPlugins();
	}

	void startPlugins(HWND mainWnd);
	void stopPlugins();

	void getPluginList(MetaDataList& aList);
	const PluginInfo::Ptr getPlugin(int index) { Lock l(cs); return plugins[index]; }

	// Functions that call the dll
	tstring onOutgoingChat(ClientInterface *client, const tstring& message);
	tstring onOutgoingPM(Client *client, const string& user, const tstring& message);
	void onIncomingChat(ClientInterface *client, string& message);
	void onIncomingPM(UserInterface *user, string& message);
	void onIncomingHubData(ClientInterface *client, const string& data);
	bool onIncomingConnectionData(ConnectionInterface *uc, const string& data);
	bool onOutgoingConnectionData(ConnectionInterface *uc, const string& data);

	void debugMessage(const string& messsage) {
		COMMAND_DEBUG("PLUGINS: " + messsage, DebugManager::CLIENT_IN, "localhost");
	}

	void removeSetting(string pluginName, string setting) {
		if(settings.find(pluginName) != settings.end() &&
			settings[pluginName].find(setting) != settings[pluginName].end())
			settings[pluginName].erase(settings[pluginName].find(setting));
	}

	void setSetting(string pluginName, string setting, string value) {
		settings[pluginName][setting] = value;
	}

	string getSetting(string pluginName, string setting) {
		if(settings.find(pluginName) != settings.end() &&
			settings[pluginName].find(setting) != settings[pluginName].end())
			return settings[pluginName][setting];
		return Util::emptyString;
	}

	// Listeners
	void setTimer(bool state, PluginStructure* plugin) {
		Lock l(cs);
		if(state) {
			if(find(timerList.begin(), timerList.end(), plugin) == timerList.end()) {
				if(timerList.size() == 0) {
					TimerManager::getInstance()->addListener(this);
				}
				timerList.push_back(plugin);
			}
		} else {
			listenerMap::iterator i = timerList.begin();
			if((i = find(timerList.begin(), timerList.end(), plugin)) != timerList.end()) {
				timerList.erase(i);
				if(timerList.size() == 0) {
					TimerManager::getInstance()->removeListener(this);
				}
			}
		}
	}

	void setHubs(bool state, PluginStructure* plugin) {
		Lock l(cs);
		if(state) {
			if(find(hubsList.begin(), hubsList.end(), plugin) == hubsList.end()) {
				if(hubsList.size() == 0) {
					ClientManager::getInstance()->addListener(this);
				}
				hubsList.push_back(plugin);
			}
		} else {
			listenerMap::iterator i = hubsList.begin();
			if((i = find(hubsList.begin(), hubsList.end(), plugin)) != hubsList.end()) {
				hubsList.erase(i);
				if(hubsList.size() == 0) {
					ClientManager::getInstance()->removeListener(this);
				}
			}
		}
	}

	GETSET(HWND, mainWnd, MainWindow)

private:
	typedef map<string, map<string, string>> settingsMap;
	typedef vector<PluginStructure*> listenerMap;

	string validateName(string name) {
		string::size_type i;
		while((i = name.find(' ')) != string::npos) 
			name.erase(i, 1);
		return name;
	}

	// SettingsManagerListener
	void on(SettingsManagerListener::Load, SimpleXML& xml) throw();
	void on(SettingsManagerListener::Save, SimpleXML& xml) throw();

	// Listeners
	void on(ClientManagerListener::ClientConnected, const Client* aClient) throw();
	void on(ClientManagerListener::ClientDisconnected, const Client* aClient) throw();
	void on(TimerManagerListener::Second, uint64_t /*ticks*/) throw();

	void load(SimpleXML& aXml);
	void save(SimpleXML& aXml);
	void unloadPlugins();
	void loadPlugins();
	void loadPlugin(const tstring& fileName);
	bool isLoaded(const string& guid);

	PluginInfo::pluginList plugins;
	settingsMap settings;
	listenerMap timerList;
	listenerMap hubsList;
	CriticalSection cs;
};

}

#endif //PLUGIN_MANAGER_H