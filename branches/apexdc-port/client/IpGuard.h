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

#ifndef IPGUARD_H
#define IPGUARD_H

#include "Socket.h"
#include "Singleton.h"
#include "SettingsManager.h"
#include "SimpleXML.h"

namespace dcpp {

class IpGuard : public Singleton<IpGuard>, private SettingsManagerListener
{
public:
	IpGuard() : noSave(true) {
		SettingsManager::getInstance()->addListener(this);
	}

	~IpGuard() throw() {
		SettingsManager::getInstance()->removeListener(this);
		ranges.clear();
	}

	struct ip {
		uint32_t iIP;

		ip() : iIP(0) { };
		ip(uint32_t iIP) : iIP(iIP) { };
		bool operator < (const ip &ip) const { return this->iIP < ip.iIP; }
		bool operator > (const ip &ip) const { return this->iIP > ip.iIP; }
		bool operator <= (const ip &ip) const { return this->iIP <= ip.iIP; }
		bool operator >= (const ip &ip) const { return this->iIP >= ip.iIP; }
		bool operator == (const ip &ip) const { return this->iIP == ip.iIP; }
		bool operator != (const ip &ip) const { return this->iIP != ip.iIP; }

		ip operator + (uint32_t i) const { return this->iIP + i; }
		ip operator - (uint32_t i) const { return this->iIP - i; }

		string str() const {
			uint32_t tmpIp = htonl(iIP);
			return inet_ntoa(*(in_addr*)&tmpIp);
		}
	};

	struct range {
		ip start, end;
		string name;
		int id;

		range() : id(0) { };
		range(int id, const string &name) : id(id), name(name) { };
		range(int id, const string &name, const ip &start, const ip &end) : id(id), name(name), start(start), end(end) { };

		bool operator < (const range &range) const {
			return (this->start < range.start) | (this->start == range.start && this->end < range.end);
		}
		bool operator > (const range &range) const {
			return (this->start > range.start) | (this->start == range.start && this->end > range.end);
		}
		bool operator == (const range &range) const {
			return (this->start >= range.start && this->end <= range.end);
		}
		bool operator < (const ip &addr) const {
			return (this->end < addr);
		}
		bool operator > (const ip &addr) const {
			return (this->start > addr);
		}

		bool adjacent(const range &rng) const { return (this->end == (rng.start - 1)); }
		bool contains(const ip &addr) const { return this->start <= addr && addr <= this->end; }
		void copy(const string &name, const ip &start, const ip &end) { this->name = name; this->start = start; this->end = end; }
	};

	typedef slist<range> RangeList;
	typedef RangeList::const_iterator RangeIter;

	bool check(const string& aIP, string& reason);
	void check(uint32_t aIP, Socket* socket = NULL) throw(SocketException);

	bool load();
	void addRange(const string& aName, const string& aStart, const string& aEnd) throw(Exception);
	void updateRange(int aId, const string& aName, const string& aStart, const string& aEnd) throw(Exception);
	void removeRange(int aId);

	const RangeList& getRanges() {
		Lock l(cs);
		return ranges;
	}

private:
	bool noSave;
	int lastRange;
	RangeList ranges;
	CriticalSection cs;

	void save();

	static RangeIter range_search(RangeIter itr1, RangeIter itr2, const ip &addr) {
		RangeIter find_itr = lower_bound(itr1, itr2, addr);
		if(find_itr != itr2 && !(*find_itr > addr))
			return find_itr;
		else
			return itr2;
	}

	static bool merger(const range &a, const range &b) {
		if(a.contains(b.start) || a.adjacent(b)) { 
			const_cast<range&>(a).copy(a.name + "; " + b.name, std::min(a.start, b.start), std::max(a.end, b.end));
			return true;
		} else return false;
	}

	// SettingsManagerListener
	void on(SettingsManagerListener::Load, SimpleXML& /*xml*/) throw() {
		if(BOOLSETTING(ENABLE_IPGUARD)) {
			load();
		}
	}

	void on(SettingsManagerListener::Save, SimpleXML& /*xml*/) throw() {
		if(!noSave) {
			save();
		}
	}
};

}

#endif // IPGUARD_H