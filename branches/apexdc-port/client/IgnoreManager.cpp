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
#include "IgnoreManager.h"

#include "Util.h"
#include "pme.h"
#include "Wildcards.h"

namespace dcpp {

void IgnoreManager::load(SimpleXML& aXml) {
	if(aXml.findChild("IgnoreList")) {
		aXml.stepIn();
		while(aXml.findChild("User")) {	
			ignoredUsers.insert(Text::toT(aXml.getChildAttrib("Nick")));
		}
		aXml.stepOut();
	}
}

void IgnoreManager::save(SimpleXML& aXml) {
	aXml.addTag("IgnoreList");
	aXml.stepIn();

	for(TStringHashIterC i = ignoredUsers.begin(); i != ignoredUsers.end(); ++i) {
		aXml.addTag("User");
		aXml.addChildAttrib("Nick", Text::fromT(*i));
	}
	aXml.stepOut();
}

void IgnoreManager::storeIgnore(const tstring& aNick) {
	if(!ignoredUsers.count(aNick)) {
		ignoredUsers.insert(aNick);
	}
}

void IgnoreManager::removeIgnore(const tstring& aNick) {
	ignoredUsers.erase(aNick);
}

bool IgnoreManager::isIgnored(const tstring& aNick) {
	bool ret = false;

	if(ignoredUsers.count(aNick))
		ret = true;

	if(BOOLSETTING(IGNORE_USE_REGEXP_OR_WC) && !ret) {
		Lock l(cs);
		for(TStringHashIterC i = ignoredUsers.begin(); i != ignoredUsers.end(); ++i) {
			if(Util::strnicmp(*i, _T("$Re:"), 4) == 0) {
				if((*i).length() > 4) {
					PME regexp((*i).substr(4), _T("gims"));

					if(regexp.match(aNick)) {
						ret = true;
						break;
					}
				}
			} else {
				ret = Wildcard::patternMatch(Text::toLower(aNick), Text::toLower(*i), false);
				if(ret)
					break;
			}
		}
	}

	return ret;
}

// SettingsManagerListener
void IgnoreManager::on(SettingsManagerListener::Load, SimpleXML& aXml) {
	load(aXml);
}

void IgnoreManager::on(SettingsManagerListener::Save, SimpleXML& aXml) {
	save(aXml);
}

}