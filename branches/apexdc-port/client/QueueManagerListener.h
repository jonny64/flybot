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

#ifndef DCPLUSPLUS_DCPP_QUEUE_MANAGER_LISTENER_H
#define DCPLUSPLUS_DCPP_QUEUE_MANAGER_LISTENER_H

#include "forward.h"

namespace dcpp {

class QueueManagerListener {
public:
	virtual ~QueueManagerListener() { }
	template<int I>	struct X { enum { TYPE = I };  };

	typedef X<0> Added;
	typedef X<1> Finished;
	typedef X<2> Removed;
	typedef X<3> Moved;
	typedef X<4> SourcesUpdated;
	typedef X<5> StatusUpdated;
	typedef X<6> PartialList;

	virtual void on(Added, const QueueItem*) throw() { }
	virtual void on(Finished, const QueueItem*, const string&, const Download*) throw() { }
	virtual void on(Removed, const QueueItem*) throw() { }
	virtual void on(Moved, const QueueItem*, const string&) throw() { }
	virtual void on(SourcesUpdated, const QueueItem*) throw() { }
	virtual void on(StatusUpdated, const QueueItem*) throw() { }
	virtual void on(PartialList, const UserPtr&, const string&) throw() { }
};

} // namespace dcpp

#endif // !defined(QUEUE_MANAGER_LISTENER_H)

/**
 * @file
 * $Id: QueueManagerListener.h 373 2008-02-06 17:23:49Z bigmuscle $
 */
