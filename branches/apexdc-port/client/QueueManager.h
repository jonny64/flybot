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

#if !defined(QUEUE_MANAGER_H)
#define QUEUE_MANAGER_H

#include "TimerManager.h"
#include "ClientManager.h"

#include "CriticalSection.h"
#include "Exception.h"
#include "User.h"
#include "File.h"
#include "QueueItem.h"
#include "Singleton.h"
#include "DirectoryListing.h"
#include "MerkleTree.h"

#include "QueueManagerListener.h"
#include "SearchManagerListener.h"
#include "ClientManagerListener.h"
#include "LogManager.h"

namespace dcpp {

STANDARD_EXCEPTION(QueueException);

class UserConnection;

class DirectoryItem {
public:
	typedef DirectoryItem* Ptr;
	typedef unordered_multimap<UserPtr, Ptr, User::Hash> DirectoryMap;
	typedef DirectoryMap::const_iterator DirectoryIter;
	typedef pair<DirectoryIter, DirectoryIter> DirectoryPair;
	
	typedef vector<Ptr> List;
	typedef List::const_iterator Iter;

	DirectoryItem() : priority(QueueItem::DEFAULT) { }
	DirectoryItem(const UserPtr& aUser, const string& aName, const string& aTarget, 
		QueueItem::Priority p) : name(aName), target(aTarget), priority(p), user(aUser) { }
	~DirectoryItem() { }
	
	UserPtr& getUser() { return user; }
	void setUser(const UserPtr& aUser) { user = aUser; }
	
	GETSET(string, name, Name);
	GETSET(string, target, Target);
	GETSET(QueueItem::Priority, priority, Priority);
private:
	UserPtr user;
};

class ConnectionQueueItem;
class QueueLoader;

class QueueManager : public Singleton<QueueManager>, public Speaker<QueueManagerListener>, private TimerManagerListener, 
	private SearchManagerListener, private ClientManagerListener
{
public:
	/** Add a file to the queue. */
	void add(const string& aTarget, int64_t aSize, const TTHValue& root, UserPtr aUser,
		Flags::MaskType aFlags = 0, bool addBad = true) throw(QueueException, FileException);
		/** Add a user's filelist to the queue. */
	void addList(const UserPtr& aUser, Flags::MaskType aFlags, const string& aInitialDir = Util::emptyString) throw(QueueException, FileException);
	/** Queue a partial file list download */
	void addPfs(const UserPtr& aUser, const string& aDir) throw(QueueException);

	void addTestSUR(UserPtr aUser, bool checkList = false) throw(QueueException, FileException) {
		StringList nicks = ClientManager::getInstance()->getNicks(*aUser);
		string nick = nicks.empty() ? Util::emptyString : Util::cleanPathChars(nicks[0]) + ".";
		string target = Util::getConfigPath() + "TestSURs\\TestSUR" + nick + aUser->getCID().toBase32();

		add(target, -1, TTHValue(), aUser, (Flags::MaskType)((checkList ? QueueItem::FLAG_CHECK_FILE_LIST : 0) | QueueItem::FLAG_TESTSUR));
	}

	void removeTestSUR(UserPtr aUser) {
		try {
			StringList nicks = ClientManager::getInstance()->getNicks(*aUser);
			string nick = nicks.empty() ? Util::emptyString : Util::cleanPathChars(nicks[0]) + ".";
			string target = Util::getConfigPath() + "TestSURs\\TestSUR" + nick + aUser->getCID().toBase32();

			remove(target);
		} catch(...) {
			// exception
		}
		return;
	}

	void removeFileListCheck(UserPtr aUser) {
		Lock l(cs);
		for(QueueItem::StringIter i = fileQueue.getQueue().begin(); i != fileQueue.getQueue().end(); ++i) {
			if(i->second->isSource(aUser) && i->second->isSet(QueueItem::FLAG_CHECK_FILE_LIST)) {
				remove(i->second->getTarget());
				return;
			}
		}
		return;
	}

	void removeOfflineChecks() {
		Lock l(cs);
		const QueueItem::StringMap& queue = fileQueue.getQueue();
		if(queue.size() > 1) {
			for(QueueItem::StringIter i = queue.begin(); i != queue.end(); ++i) {
				if(i->second->isSet(QueueItem::FLAG_TESTSUR) || i->second->isSet(QueueItem::FLAG_CHECK_FILE_LIST)) {
					if(i->second->countOnlineUsers() == 0) {
						remove(i->second->getTarget());
						// reset the iterator
						i = queue.begin();
					}
				}
			}
		}
	}

	bool isTestSURinQueue(UserPtr aUser) {
		Lock l(cs);
		StringList nicks = ClientManager::getInstance()->getNicks(*aUser);
		string nick = nicks.empty() ? Util::emptyString : Util::cleanPathChars(nicks[0]) + ".";

		for(QueueItem::StringIter i = fileQueue.getQueue().begin(); i != fileQueue.getQueue().end(); ++i) {
			if(i->second->getTargetFileName().find("TestSUR" + nick + aUser->getCID().toBase32()) != string::npos) {
				return true;
			}
		}

		return false;
	}

	/** Readd a source that was removed */
	void readd(const string& target, const UserPtr& aUser) throw(QueueException);
	/** Add a directory to the queue (downloads filelist and matches the directory). */
	void addDirectory(const string& aDir, const UserPtr& aUser, const string& aTarget, QueueItem::Priority p = QueueItem::DEFAULT) throw();
	
	int matchListing(const DirectoryListing& dl) throw();

	bool getTTH(const string& name, TTHValue& tth) const throw();

	/** Move the target location of a queued item. Running items are silently ignored */
	void move(const string& aSource, const string& aTarget) throw();

	void remove(const string& aTarget) throw();
	void removeSource(const string& aTarget, const UserPtr& aUser, Flags::MaskType reason, bool removeConn = true) throw();
	void removeSource(const UserPtr& aUser, Flags::MaskType reason) throw();

	void setPriority(const string& aTarget, QueueItem::Priority p) throw();
	void setAutoPriority(const string& aTarget, bool ap) throw();

	void getTargets(const TTHValue& tth, StringList& sl);
	const QueueItem::StringMap& lockQueue() throw() { cs.enter(); return fileQueue.getQueue(); } ;
	void unlockQueue() throw() { cs.leave(); }

	QueueItem::SourceList getSources(const QueueItem* qi) const { Lock l(cs); return qi->getSources(); }
	QueueItem::SourceList getBadSources(const QueueItem* qi) const { Lock l(cs); return qi->getBadSources(); }
	size_t getSourcesCount(const QueueItem* qi) const { Lock l(cs); return qi->getSources().size(); }
	vector<Segment> getChunksVisualisation(const QueueItem* qi, int type) const { Lock l(cs); return qi->getChunksVisualisation(type); }

	bool getQueueInfo(const UserPtr& aUser, string& aTarget, int64_t& aSize, int& aFlags) throw();
	Download* getDownload(UserConnection& aSource, string& aMessage) throw();
	void putDownload(Download* aDownload, bool finished, bool reportFinish = true) throw();
	void setFile(Download* download);
	
	/** @return The highest priority download the user has, PAUSED may also mean no downloads */
	QueueItem::Priority hasDownload(const UserPtr& aUser) throw();
	
	void loadQueue() throw();
	void saveQueue() throw();

	bool handlePartialSearch(const TTHValue& tth, PartsInfo& _outPartsInfo);
	bool handlePartialResult(const UserPtr& aUser, const TTHValue& tth, const QueueItem::PartialSource& partialSource, PartsInfo& outPartialInfo);
	
	bool dropSource(Download* d);

	const QueueItem::List getRunningFiles() const throw() {
		QueueItem::List ql;
		for(QueueItem::StringIter i = fileQueue.getQueue().begin(); i != fileQueue.getQueue().end(); ++i) {
			QueueItem* q = i->second;
			if(q->isRunning()) {
				ql.push_back(q);
			}
		}
		return ql;
	}

	bool getTargetByRoot(const TTHValue& tth, string& target, string& tempTarget) {
		Lock l(cs);
		QueueItem::List ql;
		fileQueue.find(ql, tth);

		if(ql.empty()) return false;

		target = ql.front()->getTarget();
		tempTarget = ql.front()->getTempTarget();
		return true;
	}

	bool isChunkDownloaded(const TTHValue& tth, int64_t startPos, int64_t& bytes, string& tempTarget, int64_t& size) {
		Lock l(cs);
		QueueItem::List ql;
		fileQueue.find(ql, tth);

		if(ql.empty()) return false;

		QueueItem* qi = ql.front();

		tempTarget = qi->getTempTarget();
		size = qi->getSize();

		return qi->isChunkDownloaded(startPos, bytes);
	}
	
	GETSET(uint64_t, lastSave, LastSave);
	GETSET(string, queueFile, QueueFile);

	enum { MOVER_LIMIT = 10*1024*1024 };
	class FileMover : public Thread {
	public:
		FileMover() : active(false) { }
		virtual ~FileMover() { join(); }

		void moveFile(const string& source, const string& target);
		virtual int run();
	private:
		typedef pair<string, string> FilePair;
		typedef vector<FilePair> FileList;
		typedef FileList::iterator FileIter;

		bool active;

		FileList files;
		CriticalSection cs;
	} mover;

	typedef unordered_map<CID, string> PfsQueue;
	typedef PfsQueue::iterator PfsIter;
	typedef vector<pair<QueueItem::SourceConstIter, const QueueItem*> > PFSSourceList;

	/** All queue items by target */
	class FileQueue {
	public:
		FileQueue() { }
		~FileQueue() {
			for(QueueItem::StringIter i = queue.begin(); i != queue.end(); ++i)
				i->second->dec();
			}
		void add(QueueItem* qi);
		QueueItem* add(const string& aTarget, int64_t aSize, Flags::MaskType aFlags, QueueItem::Priority p, 
			const string& aTempTarget, time_t aAdded, const TTHValue& root)
			throw(QueueException, FileException);

		QueueItem* find(const string& target) const;
		void find(QueueItem::List& sl, int64_t aSize, const string& ext);
		uint8_t getMaxSegments(int64_t filesize) const;
		void find(StringList& sl, int64_t aSize, const string& ext);
		void find(QueueItem::List& ql, const TTHValue& tth);

		// find some PFS sources to exchange parts info
		void findPFSSources(PFSSourceList&);

		QueueItem* findAutoSearch(deque<string>& recent) const;
		size_t getSize() const { return queue.size(); }
		const QueueItem::StringMap& getQueue() const { return queue; }
		void move(QueueItem* qi, const string& aTarget);
		void remove(QueueItem* qi);
	private:
		QueueItem::StringMap queue;
	};

	/** QueueItems by target */
	FileQueue fileQueue;

private:
	/** All queue items indexed by user (this is a cache for the FileQueue really...) */
	class UserQueue {
	public:
		void add(QueueItem* qi);
		void add(QueueItem* qi, const UserPtr& aUser);
		QueueItem* getNext(const UserPtr& aUser, QueueItem::Priority minPrio = QueueItem::LOWEST, int64_t wantedSize = 0, int64_t lastSpeed = 0, bool allowRemove = false);
		QueueItem* getRunning(const UserPtr& aUser);
		void addDownload(QueueItem* qi, Download* d);
		void removeDownload(QueueItem* qi, const UserPtr& d);
		const QueueItem::UserListMap& getList(int p) const { return userQueue[p]; }
		void remove(QueueItem* qi, bool removeRunning = true);
		void remove(QueueItem* qi, const UserPtr& aUser, bool removeRunning = true);
		void setPriority(QueueItem* qi, QueueItem::Priority p);
		
		const QueueItem::UserMap& getRunning() const { return running; }

		string getLastError() { 
			string tmp = lastError;
			lastError = Util::emptyString;
			return tmp;
		}

	private:
		/** QueueItems by priority and user (this is where the download order is determined) */
		QueueItem::UserListMap userQueue[QueueItem::LAST];
		/** Currently running downloads, a QueueItem is always either here or in the userQueue */
		QueueItem::UserMap running;
		/** Last error message to sent to TransferView */
		string lastError;
	};

	friend class QueueLoader;
	friend class Singleton<QueueManager>;
	
	QueueManager();
	~QueueManager() throw();
	
	mutable CriticalSection cs;
	
	/** Partial file list queue */
	PfsQueue pfsQueue;
	/** QueueItems by user */
	UserQueue userQueue;
	/** Directories queued for downloading */
	DirectoryItem::DirectoryMap directories;
	/** Recent searches list, to avoid searching for the same thing too often */
	deque<string> recent;
	/** The queue needs to be saved */
	bool dirty;
	/** Next search */
	uint64_t nextSearch;
	/** map for storing initial dir for file lists */
	StringMap dirMap;
	/** Sanity check for the target filename */
	static string checkTarget(const string& aTarget, int64_t aSize, Flags::MaskType& flags) throw(QueueException, FileException);
	/** Add a source to an existing queue item */
	bool addSource(QueueItem* qi, UserPtr aUser, Flags::MaskType addBad) throw(QueueException, FileException);

	void processList(const string& name, UserPtr& user, int flags);

	void load(const SimpleXML& aXml);
	void moveFile(const string& source, const string& target);

	void setDirty() {
		if(!dirty) {
			dirty = true;
			lastSave = GET_TICK();
		}
	}

	// TimerManagerListener
	void on(TimerManagerListener::Second, uint64_t aTick) throw();
	void on(TimerManagerListener::Minute, uint64_t aTick) throw();
	
	// SearchManagerListener
	void on(SearchManagerListener::SR, const SearchResultPtr&) throw();

	// ClientManagerListener
	void on(ClientManagerListener::UserConnected, const UserPtr& aUser) throw();
	void on(ClientManagerListener::UserDisconnected, const UserPtr& aUser) throw();
};

} // namespace dcpp

#endif // !defined(QUEUE_MANAGER_H)

/**
 * @file
 * $Id: QueueManager.h 386 2008-05-10 19:29:01Z BigMuscle $
 */
