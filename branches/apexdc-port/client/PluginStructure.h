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

#ifndef PLUGIN_STRUCTURE_H
#define PLUGIN_STRUCTURE_H

#define API_VER 0.15 // Version of the plugin api (must change every time this file has changed)

#if !defined(_STLPORT_VERSION)
# error STLport is a prerequisite for plugin developement.
#elif defined(_STLP_NO_IOSTREAMS)
# error You must use compiled STLPort else you can't use optimized node allocator.
#elif defined(_STLPORT_VERSION) && (_STLPORT_VERSION < 0x520)
# error Too old version of STLport is being used, remove this at your own risk.
#endif

#ifdef _WIN32

#define EXPORT __declspec(dllexport)
typedef HMODULE pluginHandle;

#else

#define EXPORT __attribute__ ((visibility("default")))
typedef void* pluginHandle;
typedef int HWND; // nothing of this sort exists on linux out of the box

#endif

// Forward declarations
class ClientInterface;
class PluginStructure;

// Callback interface used by Plugins to interact with ApexDC
class PluginCallBackInterface 
{
public:
	virtual HWND getMainWindow() = 0;																					// Gets the main Application window
	virtual void debugMessage(const std::string& /*message*/) = 0;													// Show a debug message
	virtual void logMessage(const std::string& /*message*/) = 0;													// Log message (system log)
	virtual void runTimer(bool /*state*/, PluginStructure* /*plugin*/) = 0;											// Enables timer functions for this plugin
	virtual void sendHubs(bool /*state*/, PluginStructure* /*plugin*/) = 0;											// Sends hubs to this plugin
	virtual void release() const = 0;																				// Release the interface

	// String conversions, call the ones in PluginStructure
	virtual std::string fromUtf8(const std::string& /*str*/, std::string& /*tmp*/) = 0;
	virtual std::string toUtf8(const std::string& /*str*/, std::string& /*tmp*/) = 0;

	// Settings, call the ones in PluginStructure
	virtual void setSetting(const std::string& /*pluginName*/, const std::string& /*setting*/, const std::string& /*value*/) = 0;
	virtual void setSetting(const std::string& /*pluginName*/, const std::wstring& /*setting*/, const std::wstring& /*value*/) = 0;

	virtual std::string getSetting(const std::string& /*pluginName*/, const std::string& /*setting*/) = 0;
	virtual std::wstring getSetting(const std::string& /*pluginName*/, const std::wstring& /*setting*/) = 0;
};

// Interface for a user
class UserInterface
{
public:
	virtual const std::string getNick() const = 0;
	virtual bool isHidden() const = 0;
	virtual bool isOp() const = 0;
	virtual std::string get(const char* name) const = 0;
	virtual void sendMessage(const std::string& message, bool thirdPerson = false) = 0;
	virtual ClientInterface* getUserClient() = 0;
};

// Interface for client (aka hub)
class ClientInterface
{
public:
	virtual void hubMessage(const std::string& message, bool thirdPerson = false) = 0;
	virtual void addClientLine(const std::string& message, int type = 0) = 0;
	virtual void sendUserCmd(const std::string& userCmd) = 0;
	virtual bool isOp() const = 0;
	virtual const std::string& getHubUrl() const = 0;
	virtual std::string getIpPort() const = 0;
	virtual std::string getField(const char* name) const = 0;
	virtual std::string getMyField(const char* name) const = 0;
	virtual UserInterface* getUser(const std::string& nick) const = 0;
};

// Interface for client<->client connections
class ConnectionInterface
{
public:
	virtual const std::string& getRemoteIp() const = 0;
	virtual unsigned short getPort() const = 0;
	virtual const std::string& getDirectionString() const = 0;
	virtual void send(const std::string& data) = 0;
	virtual void disconnect(bool graceless = false) = 0;
	virtual void reconnect() = 0;
	virtual void removeConnection() = 0;
};

// Basic Structure for all plugins
class PluginStructure 
{
public:
	PluginStructure() : callBack(NULL) { }
	virtual ~PluginStructure() throw() { }

	// addClientLine types
	enum {
		CLIENT_MESSAGE,																								// Plain text, with general text style
		STATUS_MESSAGE,																								// Message in status bar
		SYSTEM_MESSAGE,																								// Uses system message formatting
		CHEAT_MESSAGE																								// Uses cheat message formatting
	};

	// Functions that have to be implemented
	virtual const std::string getName() const = 0;																	// Returns a unique name for the plugin
	virtual const std::string getAuthor() const = 0;																// Returns authors name
	virtual const std::string getDescription() const = 0;															// Returns a short description for plugin
	virtual const std::string getGuid() const = 0;																	// Returns UUID/GUID of the plugin
	virtual double getVersion() = 0;																				// Returns the plugin version

	// Overload these functions to use the feature
	virtual bool onLoad() { return true; }																			// Called when function loaded
	virtual bool onConfig(HWND /*hWnd*/) { return false; }															// Plugin specific configuration
	virtual void onUnload() { }																						// Called when plugin is about to be unloaded
	virtual void onTimer() { }																						// Timer function, called once per second
	virtual void setDefaults() { }																					// Setup the default settings in this
	virtual int getIcon() { return 27; }																			// Stock icon for plugin (uses settings bitmap)

	virtual void onChat(ClientInterface* /*client*/, std::string& /*text*/) { }										// Incoming chat from hubs
	virtual void onPM(UserInterface* /*user*/, std::string& /*text*/) { }											// Incoming private message from user
	virtual void onHubEnter(ClientInterface* /*client*/, std::string& /*text*/) { }									// Called with text about to be sent to hub or a /command
	virtual void onPMEnter(UserInterface* /*user*/, std::string& /*text*/) { }										// Called with text about to be sent as pm or a /command
	virtual void onHubData(ClientInterface* /*client*/, const std::string& /*data*/) { }							// Incoming hub protocol messages
	virtual bool onConnectionDataIn(ConnectionInterface* /*uc*/, const std::string& /*data*/) { return false; }		// Incoming user connection messages
	virtual bool onConnectionDataOut(ConnectionInterface* /*uc*/, const std::string& /*data*/) { return false; }	// Outgoing user connection messages

	virtual void onHubConnected(const ClientInterface* /*client*/) { }												// Hub has just been connected
	virtual void onHubDisconnected(const ClientInterface* /*client*/) { }											// Hub has just been disconnected

	// Don't touch these.
	void setCallBack(PluginCallBackInterface *cb) {
		callBack = cb;
	}

	PluginCallBackInterface* getCallBack() const {
		return callBack;
	}

	// String conversions
	std::string fromUtf8(const std::string& str) {
		std::string tmp;
		return callBack->fromUtf8(str, tmp);
	}

	std::string toUtf8(const std::string& str) {
		std::string tmp;
		return callBack->toUtf8(str, tmp);
	}

	// Settings
	void setSetting(const std::string& name, const std::string& value) {
		callBack->setSetting(getGuid(), name, value);
	}

	void setSetting(const std::wstring& name, const std::wstring& value) {
		callBack->setSetting(getGuid(), name, value);
	}

	// Retrieve settings
	std::string getSetting(const std::string& name) {
		return callBack->getSetting(getGuid(), name);
	}

	std::wstring getSetting(const std::wstring& name) {
		return callBack->getSetting(getGuid(), name);
	}

private:
	PluginCallBackInterface *callBack; // Pointer to a callback structure
};

#endif //PLUGIN_STRUCTURE_H