/* 
* Copyright (C) 2001-2007 Jacek Sieka, arnetheduck on gmail point com
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

#ifndef USERINFOBASE_H
#define USERINFOBASE_H

#include "forward.h"

//#include "../client/SettingsManager.h"

class UserInfoBase {
public:
	UserInfoBase() { }
	
	void getList();
	void browseList();
	void getUserResponses();
	void checkList();
	void doReport();
	void matchQueue();
	void pm();
	void grant();
	void grantHour();
	void grantDay();
	void grantWeek();
	void ungrant();
	void addFav();
	void removeAll();
	void connectFav();
	
	virtual const UserPtr& getUser() const = 0;

	static uint8_t getImage(const Identity& u) {
		uint8_t image = 12;

		if(SETTING(OLD_ICONS_MODE) != 0 && !SETTING(USERLIST_IMAGE).empty())
			return getOldImage(u, SETTING(OLD_ICONS_MODE));

		if(u.isOp()) {
			image = 0;
		} else if(u.getUser()->isSet(User::FIREBALL)) {
			image = 1;
		} else if(u.getUser()->isSet(User::SERVER)) {
			image = 2;
		} else {
			string conn = u.getConnection();
		
			if(	(conn == "28.8Kbps") ||
				(conn == "33.6Kbps") ||
				(conn == "56Kbps") ||
				(conn == "Modem") ||
				(conn == "ISDN")) {
				image = 6;
			} else if(	(conn == "Satellite") ||
						(conn == "Microwave") ||
						(conn == "Wireless")) {
				image = 8;
			} else if(	(conn == "DSL") ||
						(conn == "Cable")) {
				image = 9;
			} else if(	(strncmp(conn.c_str(), "LAN", 3) == 0)) {
				image = 11;
			} else if( (strncmp(conn.c_str(), "NetLimiter", 10) == 0)) {
				image = 3;
			} else {
				double us = Util::toDouble(conn);
				if(us == 0.005) {
					image = 5;
				} else if(us >= 10) {
					image = 10;
				} else if(us > 0.1) {
					image = 7;
				} else if(us >= 0.01) {
					image = 4;
				}
			}
		}
		if(u.getUser()->isSet(User::AWAY)) {
			image+=13;
		}
		if(u.getUser()->isSet(User::DCPLUSPLUS)) {
			image+=26;
		}

		if(!u.isTcpActive()) {
			// Users we can't connect to...
			image+=52;
		}		

		return image;
	}

	static uint8_t getOldImage(const Identity& u, const int& m) {
		uint8_t image = 10;

		if(m == 3) {
			image = u.isOp() ? 1 : 0;

			if(u.getUser()->isSet(User::DCPLUSPLUS))
				image += 2;
			if(!u.isTcpActive()) {
				// Users we can't connect to...
				image += 4;
			}

			return image;
		}

		string conn = u.getConnection();
		if(	(conn == "28.8Kbps") ||
			(conn == "33.6Kbps") ||
			(conn == "56Kbps") ||
			(conn == "Modem")) {
			image = 1;
		} else
		if( conn == "ISDN") {
			image = 2;
		} else
		if(	(conn == "Satellite") ||
			(conn == "Microwave")) {
			image = 3;
		} else
		if( conn == "Wireless") {
			image = 4;
		} else
		if(	(conn == "DSL")) {
			image = 5;
		} else
		if( conn == "Cable") {
			image = 6;
		} else
		if(	(strncmp(conn.c_str(), "LAN", 3) == 0)) {
			image = 7;
		} else
		if(((strncmp(conn.c_str(), "NetLimiter", 10) == 0) || (strncmp(conn.c_str(), "DU Super Controler", 18) == 0) || (strncmp(conn.c_str(), "Bandwidth Controller", 20) == 0)) && (m == 1)) {
			image = 11;
		} else
		if( (conn == "0.005")) {
			image = (m == 1) ? 13 : 1;
		} else {
			double us = Util::toDouble(conn);
			if(us >= 10) {
				image = (m == 1) ? 15 : 7;
			} else 
			if(us > 0.1) {
				image = (m == 1) ? 14 : 5;
			} else
			if(us >= 0.01) {
				image = (m == 1) ? 12 : 2;
			}
		}

		if(u.isOp()) {
			image = 0;
		} else if(u.getUser()->isSet(User::FIREBALL)) {
			image = 9;
		} else if(u.getUser()->isSet(User::SERVER)) {
			image = 8;
		}
	
		if(u.getUser()->isSet(User::AWAY)) {
			image += (m == 1) ? 16 : 11;
		}
		if(u.getUser()->isSet(User::DCPLUSPLUS)) {
			image += (m == 1) ? 32 : 22;
		}

		if(!u.isTcpActive()) {
			// Users we can't connect to...
			image += (m == 1) ? 64 : 44;
		}

		if(u.isBot() && (m == 1)) {
			image = 128;
			if(u.getUser()->isSet(User::AWAY))
				image += 1;
		}

		return image;
	}
};

#endif