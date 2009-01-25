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

#include "SearchManager.h"
#include "UploadManager.h"

#include "ClientManager.h"
#include "ShareManager.h"
#include "SearchResult.h"
#include "ResourceManager.h"
#include "QueueManager.h"

namespace dcpp {

SearchManager::SearchManager() :
	socket(NULL),
	port(0),
	stop(false),
	lastSearch(GET_TICK()) 
{
	TimerManager::getInstance()->addListener(this);
}

SearchManager::~SearchManager() throw() {
	TimerManager::getInstance()->removeListener(this);
	if(socket) {
		stop = true;
		socket->disconnect();
#ifdef _WIN32
		join();
#endif
		delete socket;
	}
}

void SearchManager::search(const string& aName, int64_t aSize, TypeModes aTypeMode /* = TYPE_ANY */, SizeModes aSizeMode /* = SIZE_ATLEAST */, const string& aToken /* = Util::emptyString */, const int* aWindow /* = NULL */) {
	Lock l(cs);
	SearchQueueItem sqi(aSizeMode, aSize, aTypeMode, aName, aWindow, aToken);
	if(aWindow != NULL) {
		bool added = false;
		if(searchQueue.empty()) {
			searchQueue.push_front(sqi);
			added = true;
		} else {
			// Insert before the automatic searches (manual search) 
			for(SearchQueueIter qi = searchQueue.begin(); qi != searchQueue.end(); qi++) {
				if(qi->getWindow() == NULL) {
					searchQueue.insert(qi, sqi);
					added = true;
					break;
				}
			}
		}
		if (!added) {
			searchQueue.push_back(sqi);
		}
	} else {
		// Insert last (automatic search)
		searchQueue.push_back(sqi);
	}
}

void SearchManager::search(StringList& who, const string& aName, int64_t aSize /* = 0 */, TypeModes aTypeMode /* = TYPE_ANY */, SizeModes aSizeMode /* = SIZE_ATLEAST */, const string& aToken /* = Util::emptyString */, const int* aWindow /* = NULL */) {
	Lock l(cs);
	SearchQueueItem sqi(who, aSizeMode, aSize, aTypeMode, aName, aWindow, aToken);
	if(aWindow != NULL) {
		bool added = false;
		if(searchQueue.empty()) {
			searchQueue.push_front(sqi);
			added = true;
		} else {
			// Insert before the automatic searches (manual search) 
			for(SearchQueueIter qi = searchQueue.begin(); qi != searchQueue.end(); qi++) {
				if(qi->getWindow() == NULL) {
					searchQueue.insert(qi, sqi);
					added = true;
					break;
				}
			}
		}
		if (!added) {
			searchQueue.push_back(sqi);
		}
	} else {
		// Insert last (automatic search)
		searchQueue.push_back(sqi);
	}
}

void SearchManager::stopSearch(const int *aWindow) {
	Lock l(cs);
	for(SearchQueueIter qi = searchQueue.begin(); qi != searchQueue.end(); qi++) {
		if(qi->getWindow() == aWindow) {
			searchQueue.erase(qi);
			break;
		}
	}
}

void SearchManager::listen() throw(SocketException) {

	disconnect();

	socket = new Socket();
	socket->create(Socket::TYPE_UDP);
	port = socket->bind(static_cast<uint16_t>(SETTING(UDP_PORT)), SETTING(BIND_ADDRESS));

	start();
}

void SearchManager::disconnect() throw() {
	if(socket != NULL) {
		stop = true;
		queue.shutdown();
		socket->disconnect();
		port = 0;

		join();

		stop = false;
	}
}

#define BUFSIZE 8192
int SearchManager::run() {
	
	boost::scoped_array<uint8_t> buf(new uint8_t[BUFSIZE]);
	int len;

	queue.start();
	while(true) {

		string remoteAddr;
		try {
			while( (len = socket->read(&buf[0], BUFSIZE, remoteAddr)) != 0) {
				onData(&buf[0], len, remoteAddr);
			}
		} catch(const SocketException& e) {
			dcdebug("SearchManager::run Error: %s\n", e.getError().c_str());
		}
		if(stop) {
			return 0;
		}

		try {
			socket->disconnect();
			socket->create(Socket::TYPE_UDP);
			socket->bind(port, SETTING(BIND_ADDRESS));
		} catch(const SocketException& e) {
			// Oops, fatal this time...
			dcdebug("SearchManager::run Stopped listening: %s\n", e.getError().c_str());
			return 1;
		}
	}
	
	return 0;
}

int SearchManager::ResultsQueue::run() {
	string x = Util::emptyString;
	string remoteIp = Util::emptyString;
	stop = false;

	while(true) {
		s.wait();
		if(stop)
			break;

		{
			Lock l(cs);
			if(resultList.empty()) continue;

			x = resultList.front().first;
			remoteIp = resultList.front().second;
			resultList.pop_front();
		}

		if(x.compare(0, 4, "$SR ") == 0) {
			string::size_type i, j;
			// Directories: $SR <nick><0x20><directory><0x20><free slots>/<total slots><0x05><Hubname><0x20>(<Hubip:port>)
			// Files:       $SR <nick><0x20><filename><0x05><filesize><0x20><free slots>/<total slots><0x05><Hubname><0x20>(<Hubip:port>)
			i = 4;
			if( (j = x.find(' ', i)) == string::npos) {
				continue;
			}
			string nick = x.substr(i, j-i);
			i = j + 1;

			// A file has 2 0x05, a directory only one
			size_t cnt = count(x.begin() + j, x.end(), 0x05);
	
			SearchResult::Types type = SearchResult::TYPE_FILE;
			string file;
			int64_t size = 0;

			if(cnt == 1) {
				// We have a directory...find the first space beyond the first 0x05 from the back 
				// (dirs might contain spaces as well...clever protocol, eh?)
				type = SearchResult::TYPE_DIRECTORY;
				// Get past the hubname that might contain spaces
				if((j = x.rfind(0x05)) == string::npos) {
					continue;
				}
				// Find the end of the directory info
				if((j = x.rfind(' ', j-1)) == string::npos) {
					continue;
				}
				if(j < i + 1) {
					continue;
				}	
				file = x.substr(i, j-i) + '\\';
			} else if(cnt == 2) {
				if( (j = x.find((char)5, i)) == string::npos) {
					continue;
				}
				file = x.substr(i, j-i);
				i = j + 1;
				if( (j = x.find(' ', i)) == string::npos) {
					continue;
				}
				size = Util::toInt64(x.substr(i, j-i));
			}	
			i = j + 1;
		
			if( (j = x.find('/', i)) == string::npos) {
				continue;
			}
			uint8_t freeSlots = (uint8_t)Util::toInt(x.substr(i, j-i));
			i = j + 1;
			if( (j = x.find((char)5, i)) == string::npos) {
				continue;
			}
			uint8_t slots = (uint8_t)Util::toInt(x.substr(i, j-i));
			i = j + 1;
			if( (j = x.rfind(" (")) == string::npos) {
				continue;
			}
			string hubName = x.substr(i, j-i);
			i = j + 2;
			if( (j = x.rfind(')')) == string::npos) {
				continue;
			}

			string hubIpPort = x.substr(i, j-i);
			string url = ClientManager::getInstance()->findHub(hubIpPort);

			string encoding = ClientManager::getInstance()->findHubEncoding(url);
			nick = Text::toUtf8(nick, encoding);
			file = Text::toUtf8(file, encoding);
			hubName = Text::toUtf8(hubName, encoding);

			UserPtr user = ClientManager::getInstance()->findUser(nick, url);
			if(!user) {
				// Could happen if hub has multiple URLs / IPs
				user = ClientManager::getInstance()->findLegacyUser(nick);
				if(!user)
					continue;
			}
			ClientManager::getInstance()->setIPUser(remoteIp, user);

			string tth;
			if(hubName.compare(0, 4, "TTH:") == 0) {
				tth = hubName.substr(4);
				StringList names = ClientManager::getInstance()->getHubNames(user->getCID());
				hubName = names.empty() ? STRING(OFFLINE) : Util::toString(names);
			}

			if(tth.empty() && type == SearchResult::TYPE_FILE) {
				continue;
			}


			SearchResultPtr sr(new SearchResult(user, type, slots, freeSlots, size,
				file, hubName, remoteIp, TTHValue(tth), Util::emptyString));
			SearchManager::getInstance()->fire(SearchManagerListener::SR(), sr);
		}
		Thread::sleep(10);
	}
	return 0;
}

void SearchManager::onData(const uint8_t* buf, size_t aLen, const string& remoteIp) {
	string x((char*)buf, aLen);

	if(x.compare(1, 4, "RES ") == 0 && x[x.length() - 1] == 0x0a) {
		AdcCommand c(x.substr(0, x.length()-1));
		if(c.getParameters().empty())
			return;
		string cid = c.getParam(0);
		if(cid.size() != 39)
			return;

		UserPtr user = ClientManager::getInstance()->findUser(CID(cid));
		if(!user)
			return;

		// This should be handled by AdcCommand really...
		c.getParameters().erase(c.getParameters().begin());

		onRES(c, user, remoteIp);

	} if(x.compare(1, 4, "PSR ") == 0 && x[x.length() - 1] == 0x0a) {
		AdcCommand c(x.substr(0, x.length()-1));
		if(c.getParameters().empty())
			return;
		string cid = c.getParam(0);
		if(cid.size() != 39)
			return;

		c.getParameters().erase(c.getParameters().begin());

		uint16_t udpPort = 0;
		uint32_t partialCount = 0;
		string tth;
		string hubIpPort;
		string nick;
		PartsInfo partialInfo;

		for(StringIterC i = c.getParameters().begin(); i != c.getParameters().end(); ++i) {
			const string& str = *i;
			if(str.compare(0, 2, "U4") == 0) {
				udpPort = (uint16_t)Util::toInt(str.substr(2));
			} else if(str.compare(0, 2, "NI") == 0) {
				nick = str.substr(2);
			} else if(str.compare(0, 2, "HI") == 0) {
				hubIpPort = str.substr(2);
			} else if(str.compare(0, 2, "TR") == 0) {
				tth = str.substr(2);
			} else if(str.compare(0, 2, "PC") == 0) {
				partialCount = Util::toUInt32(str.substr(2))*2;
			} else if(str.compare(0, 2, "PI") == 0) {
				string partialInfoBlocks = str.substr(2);
				string::size_type i = 0, j = 0;
				while((j = partialInfoBlocks.find(',', i)) != string::npos) {
					partialInfo.push_back((uint16_t)Util::toInt(partialInfoBlocks.substr(i, j-i)));
					i = j + 1;
				}
			}
		}

		UserPtr user = ClientManager::getInstance()->findUser(CID(cid));
		if(!user) {
			// for NMDC support
			string url = ClientManager::getInstance()->findHub(hubIpPort);
			user = ClientManager::getInstance()->findUser(nick, url);
			if(!user) {
				// Could happen if hub has multiple URLs / IPs
				user = ClientManager::getInstance()->findLegacyUser(nick);
				if(!user) {
					dcdebug("Search result from unknown user");
					return;
				}
			}
		}

		if(partialInfo.size() != partialCount) {
			// what to do now ? just ignore partial search result :-/
			return;
		}

		PartsInfo outPartialInfo;
		QueueItem::PartialSource ps(ClientManager::getInstance()->getMyNMDCNick(user), hubIpPort, remoteIp, udpPort);
		ps.setPartialInfo(partialInfo);

		QueueManager::getInstance()->handlePartialResult(user, TTHValue(tth), ps, outPartialInfo);
		
		if((udpPort > 0) && !outPartialInfo.empty()) {
			sendPSR(remoteIp, udpPort, false, ps.getMyNick(), hubIpPort, tth, outPartialInfo);
		}
	} /*else if(x.compare(1, 4, "SCH ") == 0 && x[x.length() - 1] == 0x0a) {
		try {
			respond(AdcCommand(x.substr(0, x.length()-1)));
		} catch(ParseException& ) {
		}
	}*/ // Needs further DoS investigation
	else {
		queue.addResult(x, remoteIp);
	}
}

void SearchManager::onRES(const AdcCommand& cmd, const UserPtr& from, const string& remoteIp) {
	int freeSlots = -1;
	int64_t size = -1;
	string file;
	string tth;
	string token;

	for(StringIterC i = cmd.getParameters().begin(); i != cmd.getParameters().end(); ++i) {
		const string& str = *i;
			if(str.compare(0, 2, "FN") == 0) {
				file = Util::toNmdcFile(str.substr(2));
			} else if(str.compare(0, 2, "SL") == 0) {
				freeSlots = Util::toInt(str.substr(2));
			} else if(str.compare(0, 2, "SI") == 0) {
				size = Util::toInt64(str.substr(2));
			} else if(str.compare(0, 2, "TR") == 0) {
				tth = str.substr(2);
		} else if(str.compare(0, 2, "TO") == 0) {
			token = str.substr(2);
		}
	}

	if(!file.empty() && freeSlots != -1 && size != -1) {

		StringList names = ClientManager::getInstance()->getHubNames(from->getCID());
		string hubName = names.empty() ? STRING(OFFLINE) : Util::toString(names);
		StringList hubs = ClientManager::getInstance()->getHubs(from->getCID());
		string hub = hubs.empty() ? STRING(OFFLINE) : Util::toString(hubs);

		SearchResult::Types type = (file[file.length() - 1] == '\\' ? SearchResult::TYPE_DIRECTORY : SearchResult::TYPE_FILE);
		if(type == SearchResult::TYPE_FILE && tth.empty())
			return;
		/// @todo Something about the slots
		SearchResultPtr sr(new SearchResult(from, type, 0, (uint8_t)freeSlots, size,
			file, hubName, remoteIp, TTHValue(tth), token));
			fire(SearchManagerListener::SR(), sr);
	}
}

void SearchManager::respond(const AdcCommand& adc, const CID& from) {
	// Filter own searches
	if(from == ClientManager::getInstance()->getMe()->getCID())
		return;

	UserPtr p = ClientManager::getInstance()->findUser(from);
	if(!p)
		return;

	SearchResultList results;
	ShareManager::getInstance()->search(results, adc.getParameters(), 10);

	string token;

	adc.getParam("TO", 0, token);

	if(results.empty())
		return;

	for(SearchResultList::const_iterator i = results.begin(); i != results.end(); ++i) {
		AdcCommand cmd = (*i)->toRES(AdcCommand::TYPE_UDP);
		if(!token.empty())
			cmd.addParam("TO", token);
		ClientManager::getInstance()->send(cmd, from);
	}
}

string SearchManager::clean(const string& aSearchString) {
	static const char* badChars = "$|.[]()-_+";
	string::size_type i = aSearchString.find_first_of(badChars);
	if(i == string::npos)
		return aSearchString;

	string tmp = aSearchString;
	// Remove all strange characters from the search string
	do {
		tmp[i] = ' ';
	} while ( (i = tmp.find_first_of(badChars, i)) != string::npos);

	return tmp;
}

void SearchManager::on(TimerManagerListener::Second, uint64_t aTick) throw() {
	if((getLastSearch() + (SETTING(MINIMUM_SEARCH_INTERVAL)*1000)) < aTick) {
		SearchQueueItem sqi;
		{
			Lock l(cs);
			if(searchQueue.empty()) return;
			sqi = searchQueue.front();
			searchQueue.pop_front();
		}
		
		if(sqi.getHubs().empty()) {
			ClientManager::getInstance()->search(sqi.getSizeMode(), sqi.getSize(), sqi.getTypeMode(), sqi.getTarget(), sqi.getToken());
		} else {
			ClientManager::getInstance()->search(sqi.getHubs(), sqi.getSizeMode(), sqi.getSize(), sqi.getTypeMode(), sqi.getTarget(), sqi.getToken());
		}
		fire(SearchManagerListener::Searching(), &sqi);
		setLastSearch(aTick);
	}
}

int SearchManager::getSearchQueueNumber(const int* aWindow) {
	Lock l(cs);
	if(!searchQueue.empty()){
		int queueNumber = 0;
		for(SearchQueueIterC sqi = searchQueue.begin(); sqi != searchQueue.end(); ++sqi) {
			if(sqi->getWindow() == aWindow) {
				return queueNumber;
			}
			queueNumber++;
		}
	}
	return 0;
}

string SearchManager::getPartsString(const PartsInfo& partsInfo) const {
	string ret;

	for(PartsInfo::const_iterator i = partsInfo.begin(); i < partsInfo.end(); i+=2){
		ret += Util::toString(*i) + "," + Util::toString(*(i+1)) + ",";
	}

	return ret.substr(0, ret.size()-1);
}

void SearchManager::sendPSR(const string& ip, uint16_t port, bool wantResponse, const string& myNick, const string& hubIpPort, const string& tth, const vector<uint16_t>& partialInfo) {
	if(myNick.empty()) return;

	try {
		AdcCommand cmd(AdcCommand::CMD_PSR, AdcCommand::TYPE_UDP);
		cmd.addParam("NI", Text::utf8ToAcp(myNick));
		cmd.addParam("HI", hubIpPort);
		cmd.addParam("U4", Util::toString(wantResponse ? getPort() : 0));
		cmd.addParam("TR", tth);
		cmd.addParam("PC", Util::toString(partialInfo.size() / 2));
		cmd.addParam("PI", getPartsString(partialInfo));
			
		Socket s;
		s.writeTo(Socket::resolve(ip), port, cmd.toString(ClientManager::getInstance()->getMyCID()));
	} catch(...) {
		dcdebug("Partial search caught error\n");		
	}
}

} // namespace dcpp

/**
 * @file
 * $Id: SearchManager.cpp 386 2008-05-10 19:29:01Z BigMuscle $
 */
