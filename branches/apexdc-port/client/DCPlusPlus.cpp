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

#include "stdinc.h"
#include "DCPlusPlus.h"

#include "ConnectionManager.h"
#include "DownloadManager.h"
#include "UploadManager.h"
#include "CryptoManager.h"
#include "ShareManager.h"
#include "SearchManager.h"
#include "QueueManager.h"
#include "ClientManager.h"
#include "HashManager.h"
#include "LogManager.h"
#include "FavoriteManager.h"
#include "SettingsManager.h"
#include "FinishedManager.h"
#include "ADLSearch.h"

#include "StringTokenizer.h"

#include "DebugManager.h"
#include "ClientProfileManager.h"
#include "WebServerManager.h"
#include "IgnoreManager.h"
#include "HistoryManager.h"
#include "RawManager.h"
#include "PluginManager.h"
#include "IpGuard.h"
#include "../windows/PopupManager.h"
#include "../windows/ToolbarManager.h"
#include "../windows/ExCImage.h"

/*
#ifdef _STLP_DEBUG
void __stl_debug_terminate() {
	int* x = 0;
	*x = 0;
}
#endif
*/

namespace dcpp {

void startup(void (*f)(void*, const tstring&), void* p) {
	// "Dedicated to the near-memory of Nev. Let's start remembering people while they're still alive."
	// Nev's great contribution to dc++
	while(1) break;


#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	Util::initialize();

	ResourceManager::newInstance();
	SettingsManager::newInstance();

	PluginManager::newInstance();
	LogManager::newInstance();
	TimerManager::newInstance();
	HashManager::newInstance();
	CryptoManager::newInstance();
	SearchManager::newInstance();
	ClientManager::newInstance();
	ConnectionManager::newInstance();
	DownloadManager::newInstance();
	UploadManager::newInstance();
	ShareManager::newInstance();
	FavoriteManager::newInstance();
	QueueManager::newInstance();
	FinishedManager::newInstance();
	RawManager::newInstance();
	ADLSearchManager::newInstance();
	DebugManager::newInstance();
	ClientProfileManager::newInstance();	
	PopupManager::newInstance();
	IgnoreManager::newInstance();
	ToolbarManager::newInstance();
	HistoryManager::newInstance();
	ResourceLoader::newInstance();
	IpGuard::newInstance();

	SettingsManager::getInstance()->load();	

	if(!SETTING(LANGUAGE_FILE).empty()) {
		ResourceManager::getInstance()->loadLanguage(SETTING(LANGUAGE_FILE));
	}

	FavoriteManager::getInstance()->load();
	CryptoManager::getInstance()->loadCertificates();
	ClientProfileManager::getInstance()->load();	
	WebServerManager::newInstance();

	if(f != NULL)
		(*f)(p, TSTRING(HASH_DATABASE));
	HashManager::getInstance()->startup();
	if(f != NULL)
		(*f)(p, TSTRING(SHARED_FILES));
	ShareManager::getInstance()->refresh(true, false, true);
	if(f != NULL)
		(*f)(p, TSTRING(DOWNLOAD_QUEUE));
	QueueManager::getInstance()->loadQueue();
}

void shutdown() {
	TimerManager::getInstance()->shutdown();
	HashManager::getInstance()->shutdown();
	ConnectionManager::getInstance()->shutdown();

	BufferedSocket::waitShutdown();

	QueueManager::getInstance()->saveQueue();
	SettingsManager::getInstance()->save();
	PluginManager::getInstance()->stopPlugins();

	WebServerManager::deleteInstance();
	ClientProfileManager::deleteInstance();
	IpGuard::deleteInstance();
	ResourceLoader::deleteInstance();
	HistoryManager::deleteInstance();
	ToolbarManager::deleteInstance();
	IgnoreManager::deleteInstance();
	PopupManager::deleteInstance();
	ADLSearchManager::deleteInstance();
	RawManager::deleteInstance();
	FinishedManager::deleteInstance();
	ShareManager::deleteInstance();
	CryptoManager::deleteInstance();
	DownloadManager::deleteInstance();
	UploadManager::deleteInstance();
	QueueManager::deleteInstance();
	ConnectionManager::deleteInstance();
	SearchManager::deleteInstance();
	FavoriteManager::deleteInstance();
	ClientManager::deleteInstance();
	HashManager::deleteInstance();
	LogManager::deleteInstance();
	SettingsManager::deleteInstance();
	TimerManager::deleteInstance();
	DebugManager::deleteInstance();
	ResourceManager::deleteInstance();
	PluginManager::deleteInstance();

#ifdef _WIN32	
	::WSACleanup();
#endif
}

} // namespace dcpp

/**
 * @file
 * $Id: DCPlusPlus.cpp 385 2008-04-26 13:05:09Z BigMuscle $
 */