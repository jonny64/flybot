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

#ifndef DCPLUSPLUS_DCPP_USER_CONNECTION_H
#define DCPLUSPLUS_DCPP_USER_CONNECTION_H

#include "forward.h"
#include "TimerManager.h"
#include "UserConnectionListener.h"
#include "BufferedSocketListener.h"
#include "BufferedSocket.h"
#include "CriticalSection.h"
#include "File.h"
#include "User.h"
#include "AdcCommand.h"
#include "MerkleTree.h"
#include "DebugManager.h"
#include "ClientManager.h"
#include "PluginManager.h"
#include "QueueManager.h"
#include "version.h"

namespace dcpp {

#ifdef CAM
	static const string PGLL = "$TEGListLen";
	static const string PLIL = "$SILtLen";
	static const string PMNI = "$NYMick";
	static const string PMAX = "$XAMedOut";
	static const string PSUP = "$PUSports";
	static const string PFIL = "$LIFeLength";
	static const string PGET = "$TEG";
	static const string PSND = "$NESd";
	static const string PCNC = "$NACceled";
	static const string PDIR = "$RIDection";
#else
	static const string PGLL = "$GetListLen";
	static const string PLIL = "$ListLen";
	static const string PMNI = "$MyNick";
	static const string PMAX = "$MaxedOut";
	static const string PSUP = "$Supports";
	static const string PFIL = "$FileLength";
	static const string PGET = "$Get";
	static const string PSND = "$Send";
	static const string PCNC = "$Canceled";
	static const string PDIR = "$Direction";
#endif

class UserConnection : public Speaker<UserConnectionListener>, 
	private BufferedSocketListener, public Flags, private CommandHandler<UserConnection>,
	private boost::noncopyable, public ConnectionInterface
{
public:
	friend class ConnectionManager;
	
	static const string FEATURE_GET_ZBLOCK;
	static const string FEATURE_MINISLOTS;
	static const string FEATURE_XML_BZLIST;
	static const string FEATURE_ADCGET;
	static const string FEATURE_ZLIB_GET;
	static const string FEATURE_TTHL;
	static const string FEATURE_TTHF;
	static const string FEATURE_ADC_BAS0;
	static const string FEATURE_ADC_BASE;
	static const string FEATURE_ADC_BZIP;
	static const string FEATURE_ADC_TIGR;

	static const string FILE_NOT_AVAILABLE;
	
	enum Modes {	
		MODE_COMMAND = BufferedSocket::MODE_LINE,
		MODE_DATA = BufferedSocket::MODE_DATA
	};

	enum Flags {
		FLAG_NMDC					= 0x01,
		FLAG_OP						= 0x02,
		FLAG_UPLOAD					= 0x04,
		FLAG_DOWNLOAD				= 0x08,
		FLAG_INCOMING				= 0x10,
		FLAG_ASSOCIATED				= 0x20,
		FLAG_HASSLOT				= 0x40,
		FLAG_HASEXTRASLOT			= 0x80,
		FLAG_SUPPORTS_MINISLOTS		= 0x100,
		FLAG_SUPPORTS_XML_BZLIST	= 0x200,
		FLAG_SUPPORTS_ADCGET		= 0x400,
		FLAG_SUPPORTS_ZLIB_GET		= 0x800,
		FLAG_SUPPORTS_TTHL			= 0x1000,
		FLAG_SUPPORTS_TTHF			= 0x2000,
		FLAG_STEALTH				= 0x4000,
		FLAG_SECURE					= 0x8000
	};
	
	enum States {
		// ConnectionManager
		STATE_UNCONNECTED,
		STATE_CONNECT,

		// Handshake
		STATE_SUPNICK,		// ADC: SUP, Nmdc: $Nick
		STATE_INF,
		STATE_LOCK,
		STATE_DIRECTION,
		STATE_KEY,

		// UploadManager
		STATE_GET,			// Waiting for GET
		STATE_SEND,			// Waiting for $Send

		// DownloadManager
		STATE_SND,	// Waiting for SND
		STATE_IDLE, // No more downloads for the moment

		// Up & down
		STATE_RUNNING,		// Transmitting data

	};

	short getNumber() const { return (short)((((size_t)this)>>2) & 0x7fff); }

	// NMDC stuff
	void myNick(const string& aNick) { send(PMNI + " " + Text::fromUtf8(aNick, *encoding) + '|'); }
	void lock(const string& aLock, const string& aPk) { send ("$Lock " + aLock + " Pk=" + aPk + '|'); }
	void key(const string& aKey) { send("$Key " + aKey + '|'); }
	void direction(const string& aDirection, int aNumber) { send(PDIR + " " + aDirection + " " + Util::toString(aNumber) + '|'); }
	void fileLength(const string& aLength) { send(PFIL + " " + aLength + '|'); }
	void error(const string& aError) { send("$Error " + aError + '|'); }
	void listLen(const string& aLength) { send(PLIL + " " + aLength + '|'); }
	
	void maxedOut(int qPos = -1) {
		bool sendPos = !isSet(UserConnection::FLAG_STEALTH) && qPos >= 0;

		if(isSet(FLAG_NMDC)) {
			send(PMAX + (sendPos ? (" " + Util::toString(qPos)) : Util::emptyString) + "|");
		} else {
			AdcCommand cmd(AdcCommand::SEV_RECOVERABLE, AdcCommand::ERROR_SLOTS_FULL, "Slots full");
			if(sendPos) {
				cmd.addParam("QP", Util::toString(qPos));
			}
			send(cmd);
		}
	}
	
	
	void fileNotAvail(const std::string& msg = FILE_NOT_AVAILABLE) { isSet(FLAG_NMDC) ? send("$Error " + msg + "|") : send(AdcCommand(AdcCommand::SEV_RECOVERABLE, AdcCommand::ERROR_FILE_NOT_AVAILABLE, msg)); }
	void supports(const StringList& feat);
	void getListLen() { send(PGLL + "|"); }

	// ADC Stuff
	void sup(const StringList& features);
	void inf(bool withToken);
	void get(const string& aType, const string& aName, const int64_t aStart, const int64_t aBytes) {  send(AdcCommand(AdcCommand::CMD_GET).addParam(aType).addParam(aName).addParam(Util::toString(aStart)).addParam(Util::toString(aBytes))); }
	void snd(const string& aType, const string& aName, const int64_t aStart, const int64_t aBytes) {  send(AdcCommand(AdcCommand::CMD_SND).addParam(aType).addParam(aName).addParam(Util::toString(aStart)).addParam(Util::toString(aBytes))); }
	void send(const AdcCommand& c) { send(c.toString(0, isSet(FLAG_NMDC))); }

	void setDataMode(int64_t aBytes = -1) { dcassert(socket); socket->setDataMode(aBytes); }
	void setLineMode(size_t rollback) { dcassert(socket); socket->setLineMode(rollback); }

	void connect(const string& aServer, uint16_t aPort) throw(SocketException, ThreadException);
	void accept(const Socket& aServer) throw(SocketException, ThreadException);

	void updated() { if(socket) socket->updated(); }

	void disconnect(bool graceless = false) { if(socket) socket->disconnect(graceless); }
	void transmitFile(InputStream* f) { socket->transmitFile(f); }

	const string& getDirectionString() const {
		dcassert(isSet(FLAG_UPLOAD) ^ isSet(FLAG_DOWNLOAD));
		return isSet(FLAG_UPLOAD) ? UPLOAD : DOWNLOAD;
	}

	const UserPtr& getUser() const { return user; }
	UserPtr& getUser() { return user; }
	bool isSecure() const { return socket && socket->isSecure(); }
	bool isTrusted() const { return socket && socket->isTrusted(); }

	const string& getRemoteIp() const { if(socket) return socket->getIp(); else return Util::emptyString; }
	Download* getDownload() { dcassert(isSet(FLAG_DOWNLOAD)); return download; }
	uint16_t getPort() const { if(socket) return socket->getPort(); else return 0; }
	void setDownload(Download* d) { dcassert(isSet(FLAG_DOWNLOAD)); download = d; }
	Upload* getUpload() { dcassert(isSet(FLAG_UPLOAD)); return upload; }
	void setUpload(Upload* u) { dcassert(isSet(FLAG_UPLOAD)); upload = u; }

	void reconnect() {
		disconnect();
		Thread::sleep(100);
		ClientManager::getInstance()->connect(user, Util::toString(Util::rand()));
	}
	
	void handle(AdcCommand::SUP t, const AdcCommand& c) { fire(t, this, c); }
	void handle(AdcCommand::INF t, const AdcCommand& c) { fire(t, this, c); }
	void handle(AdcCommand::GET t, const AdcCommand& c) { fire(t, this, c); }
	void handle(AdcCommand::SND t, const AdcCommand& c) { fire(t, this, c);	}
	void handle(AdcCommand::STA t, const AdcCommand& c) { fire(t, this, c);	}
	void handle(AdcCommand::RES t, const AdcCommand& c) { fire(t, this, c); }
	void handle(AdcCommand::GFI t, const AdcCommand& c) { fire(t, this, c);	}

	// Ignore any other ADC commands for now
	template<typename T> void handle(T , const AdcCommand& ) { }

	int64_t getChunkSize() const { return chunkSize; }
	void updateChunkSize(int64_t leafSize, int64_t lastChunk, uint64_t ticks);
	
	GETSET(string, hubUrl, HubUrl);
	GETSET(string, token, Token);
	GETSET(int64_t, speed, Speed);
	GETSET(uint64_t, lastActivity, LastActivity);
	GETSET(States, state, State);

	GETSET(string*, encoding, Encoding);
	
	BufferedSocket const* getSocket() { return socket; } 
	void garbageCommand() { 
		string tmp;
		tmp.reserve(20);
		for(int i = 0; i < 20; i++) {
			tmp.append(1, (char)Util::rand('a', 'z'));
		}
		send("$"+tmp+"|");
	}

private:
	int64_t chunkSize;
	BufferedSocket* socket;
	UserPtr user;

	static const string UPLOAD, DOWNLOAD;
	
	union {
		Download* download;
		Upload* upload;
	};

	// We only want ConnectionManager to create this...
	UserConnection(bool secure_) throw() : encoding(const_cast<string*>(&Text::systemCharset)), state(STATE_UNCONNECTED),
		lastActivity(0), speed(0), chunkSize(0), socket(0), download(NULL) {
		if(secure_) {
			setFlag(FLAG_SECURE);
		}
	}

	~UserConnection() throw() {
		BufferedSocket::putSocket(socket);
		dcassert(!download);
	}

	friend struct DeleteFunction;

	void setUser(const UserPtr& aUser);

	void onLine(const string& aLine) throw();
	
	void send(const string& aString) {
		lastActivity = GET_TICK();
		if(PluginManager::getInstance()->onOutgoingConnectionData(this, aString))
			return;
		COMMAND_DEBUG(aString, DebugManager::CLIENT_OUT, getRemoteIp());
		socket->write(aString);
	}

	void on(Connected) throw();
	void on(Line, const string&) throw();
	void on(Data, uint8_t* data, size_t len) throw();
	void on(BytesSent, size_t bytes, size_t actual) throw() ;
	void on(ModeChange) throw();
	void on(TransmitDone) throw();
	void on(Failed, const string&) throw();
	void on(Updated) throw();

	/* Plugins */
	void removeConnection() { QueueManager::getInstance()->removeSource(this->getUser(), QueueItem::Source::FLAG_REMOVED); }
};

} // namespace dcpp

#endif // !defined(USER_CONNECTION_H)

/**
 * @file
 * $Id: UserConnection.h 385 2008-04-26 13:05:09Z BigMuscle $
 */
