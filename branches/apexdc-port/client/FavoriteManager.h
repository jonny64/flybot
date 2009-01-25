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

#ifndef DCPLUSPLUS_DCPP_FAVORITE_MANAGER_H
#define DCPLUSPLUS_DCPP_FAVORITE_MANAGER_H

#include "SettingsManager.h"

#include "CriticalSection.h"
#include "HttpConnection.h"
#include "User.h"
#include "UserCommand.h"
#include "FavoriteUser.h"
#include "Singleton.h"
#include "ClientManagerListener.h"
#include "FavoriteManagerListener.h"
#include "ClientManager.h"
#include "StringTokenizer.h"

namespace dcpp {
	
class HubEntry {
public:
	typedef vector<HubEntry> List;
	
	HubEntry(const string& aName, const string& aServer, const string& aDescription, const string& aUsers) throw() : 
	name(aName), server(aServer), description(aDescription), country(Util::emptyString), 
	rating(Util::emptyString), reliability(0.0), shared(0), minShare(0), users(Util::toInt(aUsers)), minSlots(0), maxHubs(0), maxUsers(0) { }

	HubEntry(const string& aName, const string& aServer, const string& aDescription, const string& aUsers, const string& aCountry,
		const string& aShared, const string& aMinShare, const string& aMinSlots, const string& aMaxHubs, const string& aMaxUsers,
		const string& aReliability, const string& aRating) : name(aName), server(aServer), description(aDescription), country(aCountry), 
		rating(aRating), reliability((float)(Util::toFloat(aReliability) / 100.0)), shared(Util::toInt64(aShared)), minShare(Util::toInt64(aMinShare)),
		users(Util::toInt(aUsers)), minSlots(Util::toInt(aMinSlots)), maxHubs(Util::toInt(aMaxHubs)), maxUsers(Util::toInt(aMaxUsers)) 
	{

	}

	HubEntry() throw() { }
	HubEntry(const HubEntry& rhs) throw() : name(rhs.name), server(rhs.server), description(rhs.description), country(rhs.country), 
		rating(rhs.rating), reliability(rhs.reliability), shared(rhs.shared), minShare(rhs.minShare), users(rhs.users), minSlots(rhs.minSlots),
		maxHubs(rhs.maxHubs), maxUsers(rhs.maxUsers) { }

	~HubEntry() throw() { }

	GETSET(string, name, Name);
	GETSET(string, server, Server);
	GETSET(string, description, Description);
	GETSET(string, country, Country);
	GETSET(string, rating, Rating);
	GETSET(float, reliability, Reliability);
	GETSET(int64_t, shared, Shared);
	GETSET(int64_t, minShare, MinShare);
	GETSET(int, users, Users);
	GETSET(int, minSlots, MinSlots);
	GETSET(int, maxHubs, MaxHubs)
	GETSET(int, maxUsers, MaxUsers);
};

class FavoriteHubEntry {
public:
	typedef FavoriteHubEntry* Ptr;
	typedef vector<Ptr> List;
	typedef List::const_iterator Iter;

	FavoriteHubEntry() throw() : connect(false), encoding(Text::systemCharset), chatusersplit(0), stealth(false), userliststate(true), mode(0), ip(Util::emptyString),
		hideShare(false), showJoins(false), exclChecks(false), noAdlSearch(false), logChat(false), miniTab(false), autoOpenOpChat(false) { }
	FavoriteHubEntry(const HubEntry& rhs) throw() : name(rhs.getName()), server(rhs.getServer()), encoding(Text::systemCharset),
		description(rhs.getDescription()), connect(false), chatusersplit(0), stealth(false), userliststate(true), mode(0), ip(Util::emptyString),
		hideShare(false), showJoins(false), exclChecks(false), noAdlSearch(false), logChat(false), miniTab(false), autoOpenOpChat(false) { }
	FavoriteHubEntry(const FavoriteHubEntry& rhs) throw() : userdescription(rhs.userdescription), name(rhs.getName()),
		server(rhs.getServer()), description(rhs.getDescription()), password(rhs.getPassword()), connect(rhs.getConnect()), 
		nick(rhs.nick), chatusersplit(rhs.chatusersplit), stealth(rhs.stealth),
		userliststate(rhs.userliststate), mode(rhs.mode), ip(rhs.ip), encoding(rhs.getEncoding()),
		hubChats(rhs.hubChats), email(rhs.email), awaymsg(rhs.awaymsg), hideShare(rhs.hideShare), showJoins(rhs.showJoins),
		exclChecks(rhs.exclChecks), noAdlSearch(rhs.noAdlSearch), logChat(rhs.logChat), miniTab(rhs.miniTab),
		autoOpenOpChat(rhs.autoOpenOpChat), protectedPrefixes(rhs.protectedPrefixes) { }
	~FavoriteHubEntry() throw() {
		for(Action::Iter i = action.begin(); i != action.end(); ++i) {
			delete i->second;
		}
	}
	
	const string& getNick(bool useDefault = true) const { 
		return (!nick.empty() || !useDefault) ? nick : SETTING(NICK);
	}

	void setNick(const string& aNick) { nick = aNick; }

	GETSET(string, userdescription, UserDescription);
	GETSET(string, awaymsg, AwayMsg);
	GETSET(string, email, Email);
	GETSET(string, name, Name);
	GETSET(string, server, Server);
	GETSET(string, description, Description);
	GETSET(string, password, Password);
	GETSET(string, headerOrder, HeaderOrder);
	GETSET(string, headerWidths, HeaderWidths);
	GETSET(string, headerVisible, HeaderVisible);
	GETSET(bool, connect, Connect);
	GETSET(string, encoding, Encoding);
	GETSET(int, chatusersplit, ChatUserSplit);
	GETSET(bool, stealth, Stealth);
	GETSET(bool, userliststate, UserListState);
	GETSET(bool, hideShare, HideShare); // Hide Share Mod
	GETSET(bool, showJoins, ShowJoins); // Show joins
	GETSET(bool, exclChecks, ExclChecks); // Excl. from client checking
	GETSET(bool, noAdlSearch, NoAdlSearch); // Don't perform ADLSearches in this hub
	GETSET(bool, logChat, LogChat); // Log chat
	GETSET(bool, miniTab, MiniTab); // Mini Tab
	GETSET(bool, autoOpenOpChat, AutoOpenOpChat); // Auto-open OP Chat 
	GETSET(int, mode, Mode); // 0 = default, 1 = active, 2 = passive
	GETSET(string, ip, IP);
	GETSET(string, hubChats, HubChats);
	GETSET(string, protectedPrefixes, ProtectedPrefixes);

	struct Action {
		typedef Action* Ptr;
		typedef unordered_map<int, Ptr> List;
		typedef List::const_iterator Iter;

		Action(bool aActive, string aRaw = Util::emptyString) 
			throw() : active(aActive) {
				if(!aRaw.empty()) {
					StringTokenizer<string> tok(aRaw, ',');
					StringList l = tok.getTokens();
					for(StringIter j = l.begin(); j != l.end(); ++j){
						if(Util::toInt(*j) != 0)
							raw.push_back(Raw(Util::toInt(*j)));
					}
				}
			};

		GETSET(bool, active, Active);
		
		struct Raw {
			Raw(int aRawId) 
				throw() : rawId(aRawId) { };

			GETSET(int, rawId, RawId);
		};

		typedef vector<Raw> RawList;
		typedef RawList::const_iterator RawIter;

		RawList raw;
	};

	Action::List action;

private:
	string nick;
};

class RecentHubEntry {
public:
	typedef RecentHubEntry* Ptr;
	typedef vector<Ptr> List;
	typedef List::const_iterator Iter;

	~RecentHubEntry() throw() { }	
	
	GETSET(string, name, Name);
	GETSET(string, server, Server);
	GETSET(string, description, Description);
	GETSET(string, users, Users);
	GETSET(string, shared, Shared);	
};

class PreviewApplication {
public:
	typedef PreviewApplication* Ptr;
	typedef vector<Ptr> List;
	typedef List::const_iterator Iter;

	PreviewApplication() throw() {}
	PreviewApplication(string n, string a, string r, string e) : name(n), application(a), arguments(r), extension(e) {};
	~PreviewApplication() throw() { }	

	GETSET(string, name, Name);
	GETSET(string, application, Application);
	GETSET(string, arguments, Arguments);
	GETSET(string, extension, Extension);
};

class SimpleXML;

/**
 * Public hub list, favorites (hub&user). Assumed to be called only by UI thread.
 */
class FavoriteManager : public Speaker<FavoriteManagerListener>, private HttpConnectionListener, public Singleton<FavoriteManager>,
	private SettingsManagerListener, private ClientManagerListener
{
public:
// Public Hubs
	enum HubTypes {
		TYPE_NORMAL,
		TYPE_BZIP2
	};
	StringList getHubLists();
	void setHubList(int aHubList);
	int getSelectedHubList() { return lastServer; }
	void refresh(bool forceDownload = false);
	HubTypes getHubListType() { return listType; }
	HubEntryList getPublicHubs() {
		Lock l(cs);
		return publicListMatrix[publicListServer];
	}
	bool isDownloading() { return (useHttp && running); }

// Favorite Users
	typedef unordered_map<CID, FavoriteUser> FavoriteMap;
	FavoriteMap getFavoriteUsers() { Lock l(cs); return users; }
	PreviewApplication::List& getPreviewApps() { return previewApplications; }

	void addFavoriteUser(const UserPtr& aUser);
	bool isFavoriteUser(const UserPtr& aUser) const { Lock l(cs); return users.find(aUser->getCID()) != users.end(); }
	void removeFavoriteUser(const UserPtr& aUser);

	bool hasSlot(const UserPtr& aUser) const;
	void setSuperUser(const UserPtr& aUser, bool superUser);
	void setUserDescription(const UserPtr& aUser, const string& description);
	void setAutoGrant(const UserPtr& aUser, bool grant);
	void userUpdated(const OnlineUser& info);
	time_t getLastSeen(const UserPtr& aUser) const;
	std::string getUserURL(const UserPtr& aUser) const;
	
// Favorite Hubs
	FavoriteHubEntryList& getFavoriteHubs() { return favoriteHubs; }

	void addFavorite(const FavoriteHubEntry& aEntry);
	void removeFavorite(const FavoriteHubEntry* entry);
	bool isFavoriteHub(const std::string& aUrl);
	FavoriteHubEntry* getFavoriteHubEntry(const string& aServer) const;

// Favorite Directories
	struct FavoriteDirectory {
		string dir, ext, name;
	};
	typedef vector<FavoriteDirectory> FavDirList;
	typedef FavDirList::iterator FavDirIter;
	bool addFavoriteDir(const string& aDirectory, const string& aName, const string& aExt);
	bool removeFavoriteDir(const string& aName);
	bool renameFavoriteDir(const string& aName, const string& anotherName);
	bool updateFavoriteDir(const string& aName, const FavoriteDirectory& dir);
	string getDownloadDirectory(const string& ext);
	FavDirList getFavoriteDirs() { return favoriteDirs; }

// Recent Hubs
	RecentHubEntry::List& getRecentHubs() { return recentHubs; };

	void addRecent(const RecentHubEntry& aEntry);
	void removeRecent(const RecentHubEntry* entry);
	void updateRecent(const RecentHubEntry* entry);

	RecentHubEntry* getRecentHubEntry(const string& aServer) {
		for(RecentHubEntry::Iter i = recentHubs.begin(); i != recentHubs.end(); ++i) {
			RecentHubEntry* r = *i;
			if(Util::stricmp(r->getServer(), aServer) == 0) {
				return r;
			}
		}
		return NULL;
	}

	PreviewApplication* addPreviewApp(string name, string application, string arguments, string extension){
		PreviewApplication* pa = new PreviewApplication(name, application, arguments, extension);
		previewApplications.push_back(pa);
		return pa;
	}

	PreviewApplication* removePreviewApp(unsigned int index){
		if(previewApplications.size() > index)
			previewApplications.erase(previewApplications.begin() + index);	
		return NULL;
	}

	PreviewApplication* getPreviewApp(unsigned int index, PreviewApplication &pa){
		if(previewApplications.size() > index)
			pa = *previewApplications[index];	
		return NULL;
	}
	
	PreviewApplication* updatePreviewApp(int index, PreviewApplication &pa){
		*previewApplications[index] = pa;
		return NULL;
	}

	void removeallRecent() {
		recentHubs.clear();
		recentsave();
	}

	bool getActiveAction(FavoriteHubEntry* entry, int actionId);
	void setActiveAction(FavoriteHubEntry* entry, int actionId, bool active);
	bool getActiveRaw(FavoriteHubEntry* entry, int actionId, int rawId);
	void setActiveRaw(FavoriteHubEntry* entry, int actionId, int rawId, bool active);

	string getAwayMessage(const string& aServer);

// User Commands
	UserCommand addUserCommand(int type, int ctx, Flags::MaskType flags, const string& name, const string& command, const string& hub);
	bool getUserCommand(int cid, UserCommand& uc);
	int findUserCommand(const string& aName, const string& aUrl);
	bool moveUserCommand(int cid, int pos);
	void updateUserCommand(const UserCommand& uc);
	void removeUserCommand(int cid);
	void removeUserCommand(const string& srv);
	void removeHubUserCommands(int ctx, const string& hub);

	UserCommand::List getUserCommands() { Lock l(cs); return userCommands; }
	UserCommand::List getUserCommands(int ctx, const StringList& hub, bool& op);

	void load();
	void save();
	void recentsave();
	
private:
	FavoriteHubEntryList favoriteHubs;
	FavDirList favoriteDirs;
	RecentHubEntry::List recentHubs;
	PreviewApplication::List previewApplications;
	UserCommand::List userCommands;
	int lastId;

	FavoriteMap users;

	mutable CriticalSection cs;

	// Public Hubs
	typedef unordered_map<string, HubEntryList> PubListMap;
	PubListMap publicListMatrix;
	string publicListServer;
	bool useHttp, running;
	HttpConnection* c;
	int lastServer;
	HubTypes listType;
	string downloadBuf;
	
	/** Used during loading to prevent saving. */
	bool dontSave;

	friend class Singleton<FavoriteManager>;
	
	FavoriteManager();
	~FavoriteManager() throw();
	
	FavoriteHubEntryList::const_iterator getFavoriteHub(const string& aServer);
	void loadXmlList(const string& xml);

	RecentHubEntry::Iter getRecentHub(const string& aServer) const {
		for(RecentHubEntry::Iter i = recentHubs.begin(); i != recentHubs.end(); ++i) {
			if(Util::stricmp((*i)->getServer(), aServer) == 0) {
				return i;
			}
		}
		return recentHubs.end();
	}

	// ClientManagerListener
	void on(UserUpdated, const OnlineUser& user) throw();
	void on(UserConnected, const UserPtr& user) throw();
	void on(UserDisconnected, const UserPtr& user) throw();

	// HttpConnectionListener
	void on(Data, HttpConnection*, const uint8_t*, size_t) throw();
	void on(Failed, HttpConnection*, const string&) throw();
	void on(Complete, HttpConnection*, const string&) throw();
	void on(Redirected, HttpConnection*, const string&) throw();
	void on(TypeNormal, HttpConnection*) throw();
	void on(TypeBZ2, HttpConnection*) throw();

	void onHttpFinished(bool fromHttp) throw();

	// SettingsManagerListener
	void on(SettingsManagerListener::Load, SimpleXML& xml) throw() {
		load(xml);
		recentload(xml);
		previewload(xml);
	}

	void on(SettingsManagerListener::Save, SimpleXML& xml) throw() {
		previewsave(xml);
	}

	void load(SimpleXML& aXml);
	void recentload(SimpleXML& aXml);
	void previewload(SimpleXML& aXml);
	void previewsave(SimpleXML& aXml);
	
	string getConfigFile() { return Util::getConfigPath() + "Favorites.xml"; }
};

} // namespace dcpp

#endif // !defined(FAVORITE_MANAGER_H)

/**
 * @file
 * $Id: FavoriteManager.h 382 2008-03-09 10:40:22Z BigMuscle $
 */
