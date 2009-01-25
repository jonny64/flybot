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

#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include "DCPlusPlus.h"

#include "HttpConnection.h"
#include "Streams.h"

namespace dcpp {

class AutoUpdater;

class UpdaterListener {
public:
	virtual ~UpdaterListener() { }
	template<int I>	struct X { enum { TYPE = I };  };

	typedef X<0> Failed;
	typedef X<1> Complete;
	typedef X<3> StatusUpdate;

	virtual void on(Failed, AutoUpdater*, const string&) throw() { }
	virtual void on(Complete, AutoUpdater*, const string&) throw() { }
	virtual void on(StatusUpdate, AutoUpdater*) throw() { }
};

class AutoUpdater : public Speaker<UpdaterListener>, private boost::noncopyable, private HttpConnectionListener
{
public:
	AutoUpdater() : hc(NULL), file(NULL), pos(0), updating(false) { }
	~AutoUpdater() throw();

	void startData(const string& aUrl, const string& aExeName);
	int64_t getPos() const { return pos; }
	int64_t getTotal() const { return hc->getSize(); }
	double getPercent() const { return (hc->getSize() > 0) ? (double)pos*100.0/(double)hc->getSize() : 0; }
	string getExeName() const { return exename; }
	bool isUpdating() const { return updating; }

private:
	HttpConnection* hc;
	OutputStream* file;
	int64_t pos;
	string exename;
	bool updating;

	void endData(const string& aLine, bool success = false);

	// HttpConnectionListener
	void on(HttpConnectionListener::Complete, HttpConnection* conn, const string& aLine) throw() { conn->removeListener(this); endData(aLine, true); }
	void on(HttpConnectionListener::Failed, HttpConnection* conn, const string& aLine) throw() { conn->removeListener(this); endData(aLine); }
	void on(HttpConnectionListener::Data, HttpConnection* conn, const uint8_t* buf, size_t len) throw();

};

}

#endif // AUTOUPDATER_H