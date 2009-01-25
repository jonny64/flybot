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

#include "stdinc.h"
#include "DCPlusPlus.h"
#include "PluginManager.h"
#include "LogManager.h"
#include "Util.h"

#ifdef _WIN32

#define PLUGIN_EXT "*.dll"

#define LOAD_LIBRARY(filename) ::LoadLibrary(filename)
#define FREE_LIBRARY(lib) ::FreeLibrary(lib)
#define GET_ADDRESS(lib, name) ::GetProcAddress(lib, name)
#define GET_ERROR() Util::translateError(GetLastError())

#else

#include "dlfcn.h"
#define PLUGIN_EXT "*.so"

#define LOAD_LIBRARY(filename) ::dlopen(filename, RTLD_LAZY | RTLD_GLOBAL)
#define FREE_LIBRARY(lib) ::dlclose(lib)
#define GET_ADDRESS(lib, name) ::dlsym(lib, name)
#define GET_ERROR() ::dlerror()

#endif

namespace dcpp {

PluginInfo::~PluginInfo() throw() {
	if(handle != NULL) {
		FREE_LIBRARY(handle);
		handle = NULL;
	}
}

void PluginInfo::sleep() {
	if(object != NULL) {
		object->onUnload();
		object->getCallBack()->release();
		this->freeObject();
		object = NULL;
	}
}

bool PluginInfo::awaken(bool bFull /*= true*/) {
	if(object == NULL) {
		if(newObject != NULL && freeObject != NULL) {
			object = this->newObject();
			object->setCallBack(new PluginCallBack());
			if(bFull) object->onLoad();

			// Update plugin metadata...
			if(info.name.empty()) {
				info.name = object->getName();
				info.author = object->getAuthor();
				info.desc = object->getDescription();
				info.ver = object->getVersion();
				info.icon = object->getIcon();
			}

			return true;
		}
	}

	return false;
}

void PluginManager::unloadPlugins() {
	Lock l(cs);
	for_each(plugins.rbegin(), plugins.rend(), DeleteFunction());
	plugins.clear();
}

void PluginManager::loadPlugins() {
	Lock l(cs);
	StringList libs = File::findFiles(Util::getDataPath() + PATH_SEPARATOR_STR + "Plugins" + PATH_SEPARATOR_STR, PLUGIN_EXT);
	if(libs.size()) {
		for(StringIter i = libs.begin(); i != libs.end(); ++i) {
			loadPlugin(Text::toT(*i));
		}
	}
}

void PluginManager::startPlugins(HWND hWnd) {
	if(plugins.size() && mainWnd == NULL) {
		mainWnd = hWnd;
		for(PluginInfo::pluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
			if((*i)->getLoaded()) (*i)->object->onLoad();
		}
	}
}

void PluginManager::stopPlugins() {
	if(plugins.size()) {
		mainWnd = NULL;
		settings.clear();
		for(PluginInfo::pluginList::reverse_iterator i = plugins.rbegin(); i != plugins.rend(); ++i) {
			(*i)->sleep();
		}
	}

	// we can't call these in destructor, as it is executed too late
	SettingsManager::getInstance()->removeListener(this);
	if(timerList.size()) { timerList.clear(); TimerManager::getInstance()->removeListener(this); }
	if(hubsList.size()) { hubsList.clear(); ClientManager::getInstance()->removeListener(this); }
}

void PluginManager::loadPlugin(const tstring& fileName) {
	pluginHandle hr = LOAD_LIBRARY(fileName.c_str());
	if(!hr) {
		dcdebug("Failed to load: %s", Text::fromT(fileName).c_str());
		return;
	}

	PluginInfo::PLUGIN_VER getVer = reinterpret_cast<PluginInfo::PLUGIN_VER>(GET_ADDRESS(hr, "getVer"));
	PluginInfo::PLUGIN_GUID getGuid = reinterpret_cast<PluginInfo::PLUGIN_GUID>(GET_ADDRESS(hr, "getGuid"));

	if(getVer != NULL && getGuid != NULL) {
		if(getVer() == API_VER) {
			if(!isLoaded(getGuid())) {
				PluginInfo::PLUGIN_LOAD getObject = reinterpret_cast<PluginInfo::PLUGIN_LOAD>(GET_ADDRESS(hr, "getObject"));
				PluginInfo::PLUGIN_UNLOAD freeObject = reinterpret_cast<PluginInfo::PLUGIN_UNLOAD>(GET_ADDRESS(hr, "freeObject"));
				if(getObject != NULL && freeObject != NULL) {
					PluginInfo* p = new PluginInfo(hr, getObject, freeObject);
					if(p->awaken(false)) { plugins.push_back(p); return; }
				}
			}
		}
	}

	FREE_LIBRARY(hr);
}

bool PluginManager::isLoaded(const string& guid) {
	Lock l(cs);
	if(plugins.size()) {
		for(PluginInfo::pluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
			if((*i)->object->getGuid() == guid)
				return true;
		}
	}
	return false;
}

void PluginManager::getPluginList(PluginManager::MetaDataList& aList) {
	Lock l(cs);

	if(plugins.size()) {
		for(PluginInfo::pluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
			aList.push_back(make_pair((*i)->getMetaData(), (*i)->getLoaded()));
		}
	}
}

// Functions that call the dll
tstring PluginManager::onOutgoingChat(ClientInterface *client, const tstring& text) {
	Lock l(cs);
	if(plugins.size()) {
		string tmp = Text::fromT(text);
		for(PluginInfo::pluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
			if(!(*i)->getLoaded()) continue;
			(*i)->object->onHubEnter(client, tmp);
			if(tmp.empty()) return Util::emptyStringT;
		}
		return Text::toT(tmp);
	}
	return text;
}

tstring PluginManager::onOutgoingPM(Client *client, const string& user, const tstring& text) {
	Lock l(cs);
	if(plugins.size()) {
		OnlineUser* ou = client->findUser(user);
		if(ou) {
			string tmp = Text::fromT(text);
			for(PluginInfo::pluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
				if(!(*i)->getLoaded()) continue;
				(*i)->object->onPMEnter(ou, tmp);
				if(tmp.empty()) return Util::emptyStringT;
			}
			return Text::toT(tmp);
		}
	}
	return text;
}

void PluginManager::onIncomingChat(ClientInterface *client, string& text) {
	Lock l(cs);
	if(plugins.size()) {
		for(PluginInfo::pluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
			if(!(*i)->getLoaded()) continue;
			(*i)->object->onChat(client, text);
			if(text.empty()) break;
		}
	}
}

void PluginManager::onIncomingPM(UserInterface *user, string& text) {
	Lock l(cs);
	if(plugins.size()) {
		for(PluginInfo::pluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
			if(!(*i)->getLoaded()) continue;
			(*i)->object->onPM(user, text);
			if(text.empty()) break;
		}
	}
}

void PluginManager::onIncomingHubData(ClientInterface *client, const string& data) {
	Lock l(cs);
	if(plugins.size()) {
		for(PluginInfo::pluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
			if(!(*i)->getLoaded()) continue;
			(*i)->object->onHubData(client, data);
		}
	}
}

bool PluginManager::onIncomingConnectionData(ConnectionInterface *uc, const string& data) {
	Lock l(cs);
	if(plugins.size()) {
		bool ret = false;
		for(PluginInfo::pluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
			if(!(*i)->getLoaded()) continue;
			if((*i)->object->onConnectionDataIn(uc, data))
				ret = true;
		}
		return ret;
	}
	return false;
}

bool PluginManager::onOutgoingConnectionData(ConnectionInterface *uc, const string& data) {
	Lock l(cs);
	if(plugins.size()) {
		bool ret = false;
		for(PluginInfo::pluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
			if(!(*i)->getLoaded()) continue;
			if((*i)->object->onConnectionDataOut(uc, data))
				ret = true;
		}
		return ret;
	}
	return false;
}

// SettingsManagerListener
void PluginManager::on(SettingsManagerListener::Load, SimpleXML& xml) throw() {
	load(xml);
}

void PluginManager::on(SettingsManagerListener::Save, SimpleXML& xml) throw() {
	save(xml);
}

// Listeners
void PluginManager::on(ClientManagerListener::ClientDisconnected, const Client* aClient) throw() {
	Lock l(cs);
	for(listenerMap::const_iterator i = hubsList.begin(); i != hubsList.end(); ++i) {
		if(*i == NULL) continue;
		(*i)->onHubDisconnected(aClient);
	}
}

void PluginManager::on(ClientManagerListener::ClientConnected, const Client* aClient) throw() {
	Lock l(cs);
	for(listenerMap::const_iterator i = hubsList.begin(); i != hubsList.end(); ++i) {
		if(*i == NULL) continue;
		(*i)->onHubConnected(aClient);
	}
}

void PluginManager::on(TimerManagerListener::Second, uint64_t /*ticks*/) throw() {
	Lock l(cs);
	for(listenerMap::const_iterator i = timerList.begin(); i != timerList.end(); ++i) {
		if(*i == NULL) continue;
		(*i)->onTimer();
	}
}

void PluginManager::load(SimpleXML& aXml) {
	Lock l(cs);

	if(aXml.findChild("Plugins")) {
		aXml.stepIn();
		for(PluginInfo::pluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
			(*i)->object->setDefaults();
			while(aXml.findChild("Plugin")) {
				const string pluginGuid = (*i)->object->getGuid();
				if(aXml.getChildAttrib("Guid") == pluginGuid) {
					StringList settings = aXml.getChildAttribs();
					for(StringIter j = settings.begin(); j != settings.end(); ++j) {
						string value = aXml.getChildAttrib(*j, getSetting(pluginGuid, *j));
						if(value == pluginGuid) continue;
						setSetting(pluginGuid, (*j), value);				
					}
					if(getSetting(pluginGuid, "State") == "0") (*i)->sleep();
				}
			}
			aXml.resetCurrentChild();
		}
		aXml.stepOut();
	} else {
		aXml.resetCurrentChild(); // to allow upgrading from pre 1.0.0
	}
 }

void PluginManager::save(SimpleXML& aXml) {
	Lock l(cs);

	aXml.addTag("Plugins");
	aXml.stepIn();
	for(settingsMap::const_iterator i = settings.begin(); i != settings.end(); ++i){
		aXml.addTag("Plugin");
		aXml.addChildAttrib("Guid", i->first);
		for(map<string, string>::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			aXml.addChildAttrib(validateName(j->first), j->second);			
		}
	}
	aXml.stepOut();
}

// Implementation of the Callback interface, plugins call functions in this 
HWND PluginCallBack::getMainWindow() {
	return PluginManager::getInstance()->getMainWindow();
}

void PluginCallBack::debugMessage(const string& message) {
	PluginManager::getInstance()->debugMessage(message);
}

void PluginCallBack::logMessage(const string& message) {
	LogManager::getInstance()->message(message);
}

void PluginCallBack::runTimer(bool state, PluginStructure* plugin) {
	PluginManager::getInstance()->setTimer(state, plugin);
}

void PluginCallBack::sendHubs(bool state, PluginStructure* plugin) {
	PluginManager::getInstance()->setHubs(state, plugin);
}

// String conversions
string PluginCallBack::fromUtf8(const string& str, string& tmp) {
	return Text::fromUtf8(str, Text::systemCharset, tmp);
}

string PluginCallBack::toUtf8(const string& str, string& tmp) {
	return Text::toUtf8(str, Text::systemCharset, tmp);
}

// Settings
void PluginCallBack::setSetting(const string& pluginName, const string& setting, const string& value) {
	if(!value.empty()) {
		PluginManager::getInstance()->setSetting(pluginName, setting, value);
	} else {
		PluginManager::getInstance()->removeSetting(pluginName, setting);
	}
}

void PluginCallBack::setSetting(const string& pluginName, const tstring& setting, const tstring& value) {
	setSetting(pluginName, Text::fromT(setting), Text::fromT(value));
}

string PluginCallBack::getSetting(const string& pluginName, const string& setting) {
	return PluginManager::getInstance()->getSetting(pluginName, setting);
}

tstring PluginCallBack::getSetting(const string& pluginName, const tstring& setting) {
	return Text::toT(getSetting(pluginName, Text::fromT(setting)));
}

}