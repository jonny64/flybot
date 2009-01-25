/*
 * Copyright (C) 2001-2008 Jacek Sieka, arnetheduck on gmail point com
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

#ifndef DCPLUSPLUS_DCPP_CLIENT_H
#define DCPLUSPLUS_DCPP_CLIENT_H

#include "forward.h"

#include "User.h"
#include "Speaker.h"
#include "BufferedSocketListener.h"
#include "TimerManager.h"
#include "ClientListener.h"
#include "DebugManager.h"

namespace dcpp {

/** Yes, this should probably be called a Hub */
class Client : public Speaker<ClientListener>, public BufferedSocketListener, protected TimerManagerListener, public ClientInterface {
public:
	typedef slist<Client*> List;
	typedef List::const_iterator Iter;

	virtual void connect();
	virtual void disconnect(bool graceless);

	virtual void connect(const OnlineUser& user, const string& token) = 0;
	virtual void hubMessage(const string& aMessage, bool thirdPerson = false) = 0;
	virtual void privateMessage(const OnlineUser& user, const string& aMessage, bool thirdPerson = false) = 0;
	virtual void sendUserCmd(const string& aUserCmd) = 0;
	virtual void search(int aSizeMode, int64_t aSize, int aFileType, const string& aString, const string& aToken) = 0;
	virtual void password(const string& pwd) = 0;
	virtual void info(bool force) = 0;
	virtual void cheatMessage(const string& aLine) = 0;

	virtual size_t getUserCount() const = 0;
	int64_t getAvailable() const { return availableBytes; };
	
	virtual void send(const AdcCommand& command) = 0;

	virtual string escape(string const& str) const { return str; }

	bool isConnected() const { return state != STATE_DISCONNECTED; }
	bool isOp() const { return getMyIdentity().isOp(); }

	virtual void refreshUserList(bool) = 0;
	virtual void getUserList(OnlineUser::List& list) const = 0;
	virtual OnlineUser* findUser(const string& aNick) const = 0;
	
	uint16_t getPort() const { return port; }
	const string& getAddress() const { return address; }

	const string& getIp() const { return ip; }
	string getIpPort() const { return getIp() + ':' + Util::toString(port); }
	string getLocalIp() const;

	void updated(const OnlineUser& aUser) { fire(ClientListener::UserUpdated(), this, aUser); }

	static int getTotalCounts() {
		return counts.normal + counts.registered + counts.op;
	}

	static string getCounts() {
		char buf[128];
		return string(buf, snprintf(buf, sizeof(buf), "%ld/%ld/%ld", counts.normal, counts.registered, counts.op));
	}

	
	StringMap& escapeParams(StringMap& sm) {
		for(StringMapIter i = sm.begin(); i != sm.end(); ++i) {
			i->second = escape(i->second);
		}
		return sm;
	}

	void reconnect();
	void shutdown();
	bool isActive() const;

	void send(const string& aMessage) { send(aMessage.c_str(), aMessage.length()); }
	void send(const char* aMessage, size_t aLen);

	string getMyNick() const { return getMyIdentity().getNick(); }
	string getHubName() const { return getHubIdentity().getNick().empty() ? getHubUrl() : getHubIdentity().getNick(); }
	string getHubDescription() const { return getHubIdentity().getDescription(); }

	Identity& getHubIdentity() { return hubIdentity; }

	const string& getHubUrl() const { return hubUrl; }

	GETSET(Identity, myIdentity, MyIdentity);
	GETSET(Identity, hubIdentity, HubIdentity);

	GETSET(string, defpassword, Password);
	
	GETSET(string, currentNick, CurrentNick);
	GETSET(string, currentDescription, CurrentDescription);
	GETSET(string, currentEmail, CurrentEmail);

	GETSET(string, localHubName, LocalHubName);
	GETSET(string, localHubDescription, LocalHubDescription);

	GETSET(string, opChat, OpChat);
	GETSET(string, protectedPrefixes, ProtectedPrefixes);
	GETSET(string, favIp, FavIp);
	
	GETSET(tstring, hubChats, HubChats);

	GETSET(uint64_t, lastActivity, LastActivity);
	GETSET(uint32_t, reconnDelay, ReconnDelay);
	
	GETSET(string*, encoding, Encoding);	
		
	GETSET(bool, registered, Registered);
	GETSET(bool, autoReconnect, AutoReconnect);
	GETSET(bool, stealth, Stealth);
	GETSET(bool, hideShare, HideShare);
	GETSET(bool, exclChecks, ExclChecks);
	GETSET(bool, autoOpenOpChat, AutoOpenOpChat);
	GETSET(bool, logChat, LogChat);

	virtual bool startChecking() = 0;
	virtual void stopChecking() = 0;
	
	mutable CriticalSection cs;

protected:
	friend class ClientManager;
	Client(const string& hubURL, char separator, bool secure_);
	virtual ~Client() throw();
	struct Counts {
		Counts(long n = 0, long r = 0, long o = 0) : normal(n), registered(r), op(o) { }
		volatile long normal;
		volatile long registered;
		volatile long op;
		bool operator !=(const Counts& rhs) { return normal != rhs.normal || registered != rhs.registered || op != rhs.op; }
	};

	enum States {
		STATE_CONNECTING,	///< Waiting for socket to connect
		STATE_PROTOCOL,		///< Protocol setup
		STATE_IDENTIFY,		///< Nick setup
		STATE_VERIFY,		///< Checking password
		STATE_NORMAL,		///< Running
		STATE_DISCONNECTED,	///< Nothing in particular
	} state;

	BufferedSocket* sock;

	static Counts counts;
	Counts lastCounts;

	int64_t availableBytes;

	void updateCounts(bool aRemove);
	void updateActivity() { lastActivity = GET_TICK(); }

	/** Reload details from favmanager or settings */
	void reloadSettings(bool updateNick);

	virtual string checkNick(const string& nick) = 0;

	// TimerManagerListener
	virtual void on(Second, uint64_t aTick) throw();
	// BufferedSocketListener
	virtual void on(Connecting) throw() { fire(ClientListener::Connecting(), this); }
	virtual void on(Connected) throw();
	virtual void on(Line, const string& aLine) throw();
	virtual void on(Failed, const string&) throw();

private:

	enum CountType {
		COUNT_UNCOUNTED,
		COUNT_NORMAL,
		COUNT_REGISTERED,
		COUNT_OP
	};

	Client(const Client&);
	Client& operator=(const Client&);

	CountType countType;
	string hubUrl;
	string address;
	string ip;
	string localIp;
	uint16_t port;
	char separator;
	bool secure;

	/* Plugins */
	void addClientLine(const string& message, int type) { fire(ClientListener::ClientLine(), this, message, type); }
	string getField(const char* name) const { return getHubIdentity().get(name); }
	string getMyField(const char* name) const { return getMyIdentity().get(name); }
	UserInterface* getUser(const string& nick) const { Lock l(cs); return findUser(nick); }
};

} // namespace dcpp

#endif // !defined(CLIENT_H)

/**
 * @file
 * $Id: Client.h 382 2008-03-09 10:40:22Z BigMuscle $
 */
