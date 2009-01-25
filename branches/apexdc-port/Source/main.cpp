/*
 * Copyright (C) 2007-2008 Crise, crise@mail.berlios.de
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

#include "stdafx.h"
#include "version.h"
#include "Plugin.h"

#ifdef _WIN32
BOOL APIENTRY DllMain(HANDLE /*hModule*/, DWORD /*reason*/, LPVOID /*lpReserved*/) {
	return TRUE;
}
#endif

extern "C" {
	// Returns the api version...
	EXPORT double __cdecl getVer() {
		return API_VER;
	}

	// Returns plugin uuid/guid
	EXPORT char* __cdecl getGuid() {
		return PLUGIN_GUID;
	}

	// Called on plugin load... 
	EXPORT PluginStructure* __cdecl getObject() {
		Plugin::newInstance();
		return Plugin::getInstance();
	}

	// Called on plugin unload...
	EXPORT void __cdecl freeObject() {
		Plugin::deleteInstance();
 	}
}