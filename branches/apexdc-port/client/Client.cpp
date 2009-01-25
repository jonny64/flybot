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

#include "stdinc.h"
#include "DCPlusPlus.h"

#include "Client.h"

#include "BufferedSocket.h"

#include "FavoriteManager.h"
#include "TimerManager.h"
#include "ResourceManager.h"
#include "ClientManager.h"
#include "UploadManager.h"
#include "PluginManager.h"

namespace dcpp {

Client::Counts Client::counts;

Client::Client(const string& hubURL, char separator_, bool secure_) : 
	myIdentity(ClientManager::getInstance()->getMe(), 0),
	reconnDelay(120), lastActivity(GET_TICK()), registered(false), autoReconnect(false),
	encoding(const_cast<string*>(&Text::systemCharset)), state(STATE_DISCONNECTED), sock(0),
	hubUrl(hubURL), port(0), separator(separator_),
	secure(secure_), countType(COUNT_UNCOUNTED), availableBytes(0)
{
	string file;
	Util::decodeUrl(hubURL, address, port, file);

	TimerManager::getInstance()->addListener(this);
}

Client::~Client() throw() {
	dcassert(!sock);
	
	// In case we were deleted before we Failed
	FavoriteManager::getInstance()->removeUserCommand(getHubUrl());
	TimerManager::getInstance()->removeListener(this);
	updateCounts(true);
}

void Client::reconnect() {
	disconnect(true);
	setAutoReconnect(true);
	setReconnDelay(0);
}

void Client::shutdown() {
	if(sock) {
		BufferedSocket::putSocket(sock);
		sock = 0;
	}
}

void Client::reloadSettings(bool updateNick) {
	const FavoriteHubEntry* hub = FavoriteManager::getInstance()->getFavoriteHubEntry(getHubUrl());
	if(hub) {
		if(updateNick) {
			setCurrentNick(checkNick(hub->getNick(true)));
		}

		string speedDescription = Util::emptyString;
		if(!hub->getStealth()) {
			if(BOOLSETTING(SHOW_DESCRIPTION_SLOTS))
				speedDescription += "[" + Util::toString(UploadManager::getInstance()->getFreeSlots()) + "]";
			if(BOOLSETTING(SHOW_DESCRIPTION_LIMIT) && SETTING(THROTTLE_ENABLE) && SETTING(MAX_UPLOAD_SPEED_LIMIT) != 0)
				speedDescription += "[L:" + Util::toString(SETTING(MAX_UPLOAD_SPEED_LIMIT)) + "KB]";
		}
		if(!hub->getUserDescription().empty()) {
			setCurrentDescription(speedDescription + hub->getUserDescription());
		} else {
			setCurrentDescription(speedDescription + SETTING(DESCRIPTION));
		}
		if(!hub->getEmail().empty()) {
			setCurrentEmail(hub->getEmail());
		} else {
			setCurrentEmail(SETTING(EMAIL));
		}
		if(!hub->getName().empty())
			setLocalHubName(hub->getName());
		if(!hub->getDescription().empty())
			setLocalHubDescription(hub->getDescription());
		if(!hub->getPassword().empty())
			setPassword(hub->getPassword());
		setStealth(hub->getStealth());
		setFavIp(hub->getIP());
		setHideShare(hub->getHideShare());
		setExclChecks(hub->getExclChecks());
		setAutoOpenOpChat(hub->getAutoOpenOpChat());
		setLogChat(hub->getLogChat() || BOOLSETTING(LOG_MAIN_CHAT));
		setOpChat(hub->getHubChats().find(";") ? hub->getHubChats().substr(0, hub->getHubChats().find(";")) : hub->getHubChats());
		setHubChats(Text::toT(hub->getHubChats()));
		setProtectedPrefixes(hub->getProtectedPrefixes().empty() ? SETTING(PROT_USERS) : (SETTING(PROT_USERS).empty() ? hub->getProtectedPrefixes() : SETTING(PROT_USERS) + ";" + hub->getProtectedPrefixes()));
	} else {
		if(updateNick) {
			setCurrentNick(checkNick(SETTING(NICK)));
		}
		setCurrentDescription(SETTING(DESCRIPTION));
		setCurrentEmail(SETTING(EMAIL));
		setStealth(true);
		setFavIp(Util::emptyString);
		setHideShare(false);
		setExclChecks(false);
		setAutoOpenOpChat(false);
		setLogChat(BOOLSETTING(LOG_MAIN_CHAT));
		setOpChat(Util::emptyString);
		setHubChats(Util::emptyStringT);
		setProtectedPrefixes(SETTING(PROT_USERS));
	}

	if(getCurrentDescription().length() > 35 && getStealth())
		setCurrentDescription(getCurrentDescription().substr(0, 35));
}

bool Client::isActive() const {
	return ClientManager::getInstance()->getMode(hubUrl) != SettingsManager::INCOMING_FIREWALL_PASSIVE;
}

void Client::connect() {
	if(sock)
		BufferedSocket::putSocket(sock);

	availableBytes = 0;

	setAutoReconnect(true);
	setReconnDelay(120 + Util::rand(0, 60));
	reloadSettings(true);
	setRegistered(false);
	setMyIdentity(Identity(ClientManager::getInstance()->getMe(), 0));
	setHubIdentity(Identity());

	state = STATE_CONNECTING;

	try {
		sock = BufferedSocket::getSocket(separator);
		sock->addListener(this);
		sock->connect(address, port, secure, BOOLSETTING(ALLOW_UNTRUSTED_HUBS), true);
	} catch(const Exception& e) {
		if(sock) {
			BufferedSocket::putSocket(sock);
			sock = 0;
		}
		fire(ClientListener::Failed(), this, e.getError());
	}
	updateActivity();
}

void Client::send(const char* aMessage, size_t aLen) {
	dcassert(sock);
	if(!sock)
		return;
	updateActivity();
	sock->write(aMessage, aLen);
	COMMAND_DEBUG(aMessage, DebugManager::HUB_OUT, getIpPort());
}

void Client::on(Connected) throw() {
	updateActivity(); 
	ip = sock->getIp();
	localIp = sock->getLocalIp();
	fire(ClientListener::Connected(), this);
	state = STATE_PROTOCOL;
}

void Client::on(Failed, const string& aLine) throw() {
	updateActivity(); 
	state = STATE_DISCONNECTED;
	FavoriteManager::getInstance()->removeUserCommand(getHubUrl());
	sock->removeListener(this);
	fire(ClientListener::Failed(), this, aLine);
}

void Client::disconnect(bool graceLess) {
	if(sock) 
		sock->disconnect(graceLess);
}

void Client::updateCounts(bool aRemove) {
	// We always remove the count and then add the correct one if requested...
	if(countType == COUNT_NORMAL) {
		Thread::safeDec(counts.normal);
	} else if(countType == COUNT_REGISTERED) {
		Thread::safeDec(counts.registered);
	} else if(countType == COUNT_OP) {
		Thread::safeDec(counts.op);
	}

	countType = COUNT_UNCOUNTED;

	if(!aRemove) {
		if(getMyIdentity().isOp()) {
			Thread::safeInc(counts.op);
			countType = COUNT_OP;
		} else if(getMyIdentity().isRegistered()) {
			Thread::safeInc(counts.registered);
			countType = COUNT_REGISTERED;
		} else {
			Thread::safeInc(counts.normal);
			countType = COUNT_NORMAL;
		}
	}
}

string Client::getLocalIp() const {
	// Favorite hub Ip
	if(!getFavIp().empty())
		return Socket::resolve(getFavIp());

	// Best case - the server detected it
	if((!BOOLSETTING(NO_IP_OVERRIDE) || SETTING(EXTERNAL_IP).empty()) && !getMyIdentity().getIp().empty()) {
		return getMyIdentity().getIp();
	}

	if(!SETTING(EXTERNAL_IP).empty()) {
		return Socket::resolve(SETTING(EXTERNAL_IP));
	}

	if(localIp.empty()) {
		return Util::getLocalIp();
	}

	return localIp;
}

void Client::on(Line, const string& aLine) throw() {
	updateActivity();
	COMMAND_DEBUG(aLine, DebugManager::HUB_IN, getIpPort());
	PluginManager::getInstance()->onIncomingHubData(this, aLine);
}

void Client::on(Second, uint64_t aTick) throw() {
	if(state == STATE_DISCONNECTED && getAutoReconnect() && (aTick > (getLastActivity() + getReconnDelay() * 1000)) ) {
		// Try to reconnect...
		connect();
	}
}

} // namespace dcpp

/**
 * @file
 * $Id: Client.cpp 386 2008-05-10 19:29:01Z BigMuscle $
 */
