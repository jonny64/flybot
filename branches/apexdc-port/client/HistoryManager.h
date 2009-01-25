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

#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include "DCPlusPlus.h"

#include "Singleton.h"
#include "SettingsManager.h"
#include "FinishedManager.h"
#include "SimpleXML.h"

namespace dcpp {

class HistoryManager: public Singleton<HistoryManager>, private SettingsManagerListener
{
public:
	HistoryManager() { SettingsManager::getInstance()->addListener(this); };
	~HistoryManager() { SettingsManager::getInstance()->removeListener(this); };

	// Search history
	void setSearchHistory(const TStringList& list);
	void clearSearchHistory();
	bool addSearchToHistory(const tstring& search);
	TStringList getSearchHistory() const;

private:
	// Search history
	void loadSearchHistory(SimpleXML& aXml);
	void saveSearchHistory(SimpleXML& aXml);
	TStringList	searchHistory;

	// Transfer histories
	void loadDownloadHistory();
	void loadUploadHistory();
	void saveDownloadHistory();
	void saveUploadHistory();

	// SettingsManagerListener
	void on(SettingsManagerListener::Load, SimpleXML& xml) throw();
	void on(SettingsManagerListener::Save, SimpleXML& xml) throw();

	// Generic
	mutable CriticalSection cs;

};

}

#endif // HISTORYMANAGER_H
