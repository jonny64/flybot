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

#ifndef USER_MAP_H
#define USER_MAP_H

#include "../client/QueueManager.h"
#include "../client/Client.h"
#include "../client/Thread.h"

namespace dcpp {

template<bool isADC, typename BaseMap>
class UserMap : public BaseMap 
{
public:
	UserMap() : checker(NULL) { };
	~UserMap() { uinitChecks(); };

	void initChecks(Client* client) {
		if((checker == NULL) && !client->getExclChecks()) {
			checker = new ThreadedCheck(client, this);
		}
	}

	void uinitChecks() {
		stopChecking();
		if(checker != NULL) {
			delete checker;
			checker = NULL;
		}
	}

	void startCheckOnConnect() {
		if((checker != NULL) && !checker->isChecking()) {
			checker->setCheckOnConnect(true);
			checker->start();
		}
	}

	bool startChecking() {
		if((checker == NULL) || checker->isChecking()) { return false; }
		checker->start();
		return true;
	}

	void stopChecking() {
		if((checker != NULL) && checker->isChecking()) {
			checker->cancel();
			QueueManager::getInstance()->removeOfflineChecks();
		}
	}

private:

	// ThreadedCheck
	class ThreadedCheck : public Thread
	{
		public:
			ThreadedCheck(Client* aClient, BaseMap* aUsers) : client(aClient), users(aUsers), keepChecking(false), initialSweepComplete(false), checkOnConnect(false), stop(true) { };
			~ThreadedCheck() { cancel(); join(); }

			bool isChecking() { return !stop; }

			void start() {
				stop = false;
				Thread::start();
			}

			void cancel() {
				keepChecking = false;
				checkOnConnect = false;
				stop = true;
			}

			GETSET(bool, checkOnConnect, CheckOnConnect);

		private:
			int run() {
				setThreadPriority(Thread::IDLE);
				if(checkOnConnect && !keepChecking && !stop) { 
					sleep(SETTING(CHECK_DELAY));
					checkOnConnect = false;
				}

				keepChecking = (client->isConnected() && !stop);
				initialSweepComplete = !BOOLSETTING(CHECK_CLIENT_BEFORE_FILELIST);
				int sleepTime = SETTING(SLEEP_TIME);
				bool iterBreak = false;
				uint8_t secs = 0;

				while(keepChecking) {
					if(client->isConnected()) {
						int t = 0;
						int f = 0;
						{
							const QueueItem::StringMap& q = QueueManager::getInstance()->lockQueue();
							for(QueueItem::StringIter i = q.begin(); i != q.end(); ++i) {
								if(i->second->isSet(QueueItem::FLAG_TESTSUR)) {
									t++;
								} else if (i->second->isSet(QueueItem::FLAG_CHECK_FILE_LIST)) {
									f++;
								}
							}
							QueueManager::getInstance()->unlockQueue();
						}

						if(t < SETTING(MAX_TESTSURS)) {
							Lock l(client->cs);
							iterBreak = false;
							for(BaseMap::const_iterator i = users->begin(); i != users->end(); ++i) {
								i->second->inc();
								OnlineUser* ou = i->second;
								if(!client->isConnected() || stop) {
									ou->dec();
									break;
								}
								if(!ou->isHidden() && (!isADC || !ou->getUser()->isSet(User::NO_ADC_1_0_PROTOCOL))) {
									if(ou->isCheckable()) {
										if(ou->shouldTestSUR()) {
											if(!ou->getChecked()) {
												iterBreak = true;
												try {
													QueueManager::getInstance()->addTestSUR(ou->getUser(), false);
													ou->getIdentity().set("TQ", "1");
												} catch(...) {
													dcdebug("Exception adding testsur: %s\n", ou->getIdentity().getNick());
												}
												ou->dec();
												break;
											}
										} else if(!ou->getIdentity().get("TQ").empty()) {
											try {
												if(!QueueManager::getInstance()->isTestSURinQueue(ou->getUser())) {
													iterBreak = true;
													ou->getIdentity().set("TQ", Util::emptyString);
													ou->dec();
													break;
												}
											} catch(...) {
													dcdebug("Exception remove hastestsur: %s\n", ou->getIdentity().getNick());
											}
										}
									}
									if(initialSweepComplete && (f < SETTING(MAX_FILELISTS))) {
										if(ou->shouldCheckFileList()) {
											if(!ou->getChecked(true)) {
												try {
													QueueManager::getInstance()->addList(ou->getUser(), QueueItem::FLAG_CHECK_FILE_LIST);
													ou->getIdentity().set("FQ", "1");
													ou->dec();
													break;
												} catch(...) {
													dcdebug("Exception adding filelist %s\n", ou->getIdentity().getNick());
												}
											}
										}
									}
								}
								ou->dec();
							}
						}
						if(!initialSweepComplete) {
							initialSweepComplete = !iterBreak;
						}
						if(secs >= 30) {
							try {
								QueueManager::getInstance()->removeOfflineChecks();
								secs = 0;
							} catch(...) {
								// oh well, try again in 10 secs.
								secs = 20;
							}
						} else {
							secs++;
						}
						sleep(sleepTime);
					} else {
						sleep(10000);
					}
				}
				stop = true;
				return 0;
			}

			bool initialSweepComplete;
			bool keepChecking;
			bool stop;

			Client* client;
			BaseMap* users;
	} *checker;
};

}

#endif // !defined(USER_MAP_H)