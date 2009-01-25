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

#ifndef IGNOREMANAGER_H
#define IGNOREMANAGER_H

#include "DCPlusPlus.h"

#include "Singleton.h"
#include "SettingsManager.h"
#include "SimpleXML.h"
#include "User.h"

namespace dcpp {

class IgnoreManager: public Singleton<IgnoreManager>, private SettingsManagerListener
{
public:
	IgnoreManager() { SettingsManager::getInstance()->addListener(this); }
	~IgnoreManager() { SettingsManager::getInstance()->removeListener(this); }

	// store & remove ignores through/from hubframe
	void storeIgnore(const tstring& aNick);
	void removeIgnore(const tstring& aNick);

	// check if user is ignored
	bool isIgnored(const tstring& aNick);

	// get and put ignorelist (for MiscPage)
	unordered_set<tstring> getIgnoredUsers() { Lock l(cs); return ignoredUsers; }
	void putIgnoredUsers(const unordered_set<tstring> &ignoreList) { Lock l(cs); ignoredUsers = ignoreList; }

private:
	typedef unordered_set<tstring> TStringHash;
	typedef TStringHash::const_iterator TStringHashIterC;
	CriticalSection cs;

	// save & load
	void load(SimpleXML& aXml);
	void save(SimpleXML& aXml);

	// SettingsManagerListener
	virtual void on(SettingsManagerListener::Load, SimpleXML& xml) throw();
	virtual void on(SettingsManagerListener::Save, SimpleXML& xml) throw();

	// contains the ignored nicks and patterns 
	TStringHash ignoredUsers;
};

}

#endif // IGNOREMANAGER_H