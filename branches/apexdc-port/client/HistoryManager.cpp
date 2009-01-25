/* 
 * Copyright (C) 2006-2008 Crise, crise@mail.berlios.de
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
#include "HistoryManager.h"
#include "ClientManager.h"

#define MAX_ITEMS 20

namespace dcpp {

// Search history
void HistoryManager::loadSearchHistory(SimpleXML& aXml) {
	if(BOOLSETTING(AUTO_COMPLETE_SEARCH)) {
		if(aXml.findChild("SearchHistory")) {
			aXml.stepIn();
			while(aXml.findChild("Search")) {
				addSearchToHistory(Text::toT(aXml.getChildData()));
			}
			aXml.stepOut();
		}
	}
}

void HistoryManager::saveSearchHistory(SimpleXML& aXml) {
	if(BOOLSETTING(AUTO_COMPLETE_SEARCH)) {
		aXml.addTag("SearchHistory");
		aXml.stepIn();
		{
			Lock l(cs);
			for(TStringIter i = searchHistory.begin(); i != searchHistory.end(); ++i) {
				string tmp = Text::fromT(*i);
				aXml.addTag("Search", tmp);
			}
		}
		aXml.stepOut();
	}
}

void HistoryManager::setSearchHistory(const TStringList& list) {
	Lock l(cs);
	searchHistory = list;
}

void HistoryManager::clearSearchHistory() {
	Lock l(cs);
	searchHistory.clear();
}

bool HistoryManager::addSearchToHistory(const tstring& search) {
	if(search.empty())
		return false;

	Lock l(cs);

	if(find(searchHistory.begin(), searchHistory.end(), search) != searchHistory.end())
		return false;

		
	while(searchHistory.size() > static_cast<TStringList::size_type>(SETTING(SEARCH_HISTORY)))
		searchHistory.erase(searchHistory.begin());

	searchHistory.push_back(search);

	return true;
}

TStringList HistoryManager::getSearchHistory() const {
	Lock l(cs);
	return searchHistory;
}

// Transfer histories
void HistoryManager::loadDownloadHistory() {
	if(!BOOLSETTING(KEEP_DL_HISTORY))
		return;

	try {
		SimpleXML aXml;
		aXml.fromXML(File(Util::getConfigPath() + "FinishedDL.xml", File::READ, File::OPEN).read());
		if(aXml.findChild("FinishedDownloads")) {
			aXml.stepIn();
			while(aXml.findChild("Item")) {
				if(aXml.getChildAttrib("CID").length() != 39) continue;
				UserPtr user = ClientManager::getInstance()->getUser(CID(aXml.getChildAttrib("CID")));
				ClientManager::getInstance()->updateNick(user, aXml.getChildAttrib("User"));

				FinishedItem i = FinishedItem(aXml.getChildAttrib("Target"), user,
				aXml.getChildAttrib("Hub"), aXml.getIntChildAttrib("Size"), aXml.getIntChildAttrib("ChunkSize"), aXml.getIntChildAttrib("MilliSeconds"),
				aXml.getIntChildAttrib("Time"));

				FinishedManager::getInstance()->insertHistoryItem(i);
			}
			aXml.resetCurrentChild();
			aXml.stepOut();
		}
	} catch(const Exception& e) {
		dcdebug("HistoryManager::loadDownloadHistory: %s\n", e.getError().c_str());
	}
}

void HistoryManager::loadUploadHistory() {
	if(!BOOLSETTING(KEEP_UL_HISTORY))
		return;

	try {
		SimpleXML aXml;
		aXml.fromXML(File(Util::getConfigPath() + "FinishedUL.xml", File::READ, File::OPEN).read());
		if(aXml.findChild("FinishedUploads")) {
			aXml.stepIn();
			while(aXml.findChild("Item")) {
				if(aXml.getChildAttrib("CID").length() != 39) continue;
				UserPtr user = ClientManager::getInstance()->getUser(CID(aXml.getChildAttrib("CID")));
				ClientManager::getInstance()->updateNick(user, aXml.getChildAttrib("User"));

				FinishedItem i = FinishedItem(aXml.getChildAttrib("Target"), user,
				aXml.getChildAttrib("Hub"), aXml.getIntChildAttrib("Size"), aXml.getIntChildAttrib("ChunkSize"), aXml.getIntChildAttrib("MilliSeconds"),
				aXml.getIntChildAttrib("Time"));
			
				FinishedManager::getInstance()->insertHistoryItem(i, true);
			}
			aXml.resetCurrentChild();
			aXml.stepOut();
		}
	} catch(const Exception& e) {
		dcdebug("HistoryManager::loadUploadHistory: %s\n", e.getError().c_str());
	}
}

void HistoryManager::saveDownloadHistory() {
	if(!BOOLSETTING(KEEP_DL_HISTORY)) {
		File::deleteFile(Util::getConfigPath() + "FinishedDL.xml");
		return;
	}

	SimpleXML aXml;
	aXml.addTag("FinishedDownloads");
	aXml.stepIn();

	const FinishedItemList& fl = FinishedManager::getInstance()->lockList();
	for(FinishedItemList::const_iterator i = fl.begin() + (fl.size() > MAX_ITEMS ? fl.size() - MAX_ITEMS : 0); i != fl.end(); ++i) {
		if((*i)->getUser()) {
			aXml.addTag("Item");
			aXml.addChildAttrib("Target", (*i)->getTarget());
			aXml.addChildAttrib("User", ClientManager::getInstance()->getNicks((*i)->getUser()->getCID())[0]);
			aXml.addChildAttrib("CID", (*i)->getUser()->getCID().toBase32());
			aXml.addChildAttrib("Hub", (*i)->getHub());
			aXml.addChildAttrib("Size", (*i)->getSize());
			aXml.addChildAttrib("ChunkSize", (*i)->getChunkSize());
			aXml.addChildAttrib("MilliSeconds", (*i)->getMilliSeconds());
			aXml.addChildAttrib("Time", (*i)->getTime());
		}
	}
	FinishedManager::getInstance()->unlockList();

	aXml.stepOut();

	try {
		string fname = Util::getConfigPath() + "FinishedDL.xml";

		File f(fname + ".tmp", File::WRITE, File::CREATE | File::TRUNCATE);
		f.write(SimpleXML::utf8Header);
		f.write(aXml.toXML());
		f.close();
		File::deleteFile(fname);
		File::renameFile(fname + ".tmp", fname);
	} catch(const Exception& e) {
		dcdebug("HistoryManager::saveDownloadHistory: %s\n", e.getError().c_str());
	}
}

void HistoryManager::saveUploadHistory() {
	if(!BOOLSETTING(KEEP_UL_HISTORY)) {
		File::deleteFile(Util::getConfigPath() + "FinishedUL.xml");
		return;
	}

	SimpleXML aXml;
	aXml.addTag("FinishedUploads");
	aXml.stepIn();

	const FinishedItemList& fl = FinishedManager::getInstance()->lockList(true);
	for(FinishedItemList::const_iterator i = fl.begin() + (fl.size() > MAX_ITEMS ? fl.size() - MAX_ITEMS : 0); i != fl.end(); ++i) {
		if((*i)->getUser()) {
			aXml.addTag("Item");
			aXml.addChildAttrib("Target", (*i)->getTarget());
			aXml.addChildAttrib("User", ClientManager::getInstance()->getNicks((*i)->getUser()->getCID())[0]);
			aXml.addChildAttrib("CID", (*i)->getUser()->getCID().toBase32());
			aXml.addChildAttrib("Hub", (*i)->getHub());
			aXml.addChildAttrib("Size", (*i)->getSize());
			aXml.addChildAttrib("ChunkSize", (*i)->getChunkSize());
			aXml.addChildAttrib("MilliSeconds", (*i)->getMilliSeconds());
			aXml.addChildAttrib("Time", (*i)->getTime());
		}
	}
	FinishedManager::getInstance()->unlockList();

	aXml.stepOut();

	try {
		string fname = Util::getConfigPath() + "FinishedUL.xml";

		File f(fname + ".tmp", File::WRITE, File::CREATE | File::TRUNCATE);
		f.write(SimpleXML::utf8Header);
		f.write(aXml.toXML());
		f.close();
		File::deleteFile(fname);
		File::renameFile(fname + ".tmp", fname);
	} catch(const Exception& e) {
		dcdebug("HistoryManager::saveUploadHistory: %s\n", e.getError().c_str());
	}
}

// SettingsManagerListener
void HistoryManager::on(SettingsManagerListener::Load, SimpleXML& aXml) {
	loadSearchHistory(aXml);
	loadDownloadHistory();
	loadUploadHistory();
}

void HistoryManager::on(SettingsManagerListener::Save, SimpleXML& aXml) {
	saveSearchHistory(aXml);
	saveDownloadHistory();
	saveUploadHistory();
}

}
