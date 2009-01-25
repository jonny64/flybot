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

#include "AutoUpdater.h"
#include "ZipFile.h"

namespace dcpp {

AutoUpdater::~AutoUpdater() throw() {
	if(hc != NULL) {
		hc->removeListeners();
		delete hc;
	} if(file != NULL) {
		try {
			file->flush();
			delete file;
			file = NULL;
		} catch(const FileException&) { }
	}
}

void AutoUpdater::on(HttpConnectionListener::Data, HttpConnection* conn, const uint8_t* buf, size_t len) throw() {
	if(file == NULL) {
		File* f = NULL;
		try {
			f = new File(Util::getSystemPath() + "Update.zip", File::WRITE, File::OPEN | File::CREATE | File::SHARED);
			if(f->getSize() == 0 && conn->getSize() != 0) f->setSize(conn->getSize());
		} catch(const FileException& e) {
			if(f) { delete f; f = NULL; }
			endData(e.getError());
			return;
		} catch(const Exception& e) {
			if(f) { delete f; f = NULL; }
			endData(e.getError());
			return;
		}

		try {
			if(SETTING(BUFFER_SIZE) > 0) {
				file = new BufferedOutputStream<true>(f);
			}
		} catch(const Exception& e) {
			endData(e.getError());
			return;
		} catch(...) {
			if(file) { delete file; file = NULL; }
			return;			
		}

		// Check that we don't get too many bytes
		file = new LimitedOutputStream<true>(((file != NULL) ? file : f), f->getSize());
	}

	try {
		pos += file->write(buf, len);
	} catch(const Exception& e) {
		endData(e.getError());
	}

	//fire(UpdaterListener::StatusUpdate(), this);
}

void AutoUpdater::startData(const string& aUrl, const string& aExeName) {
	updating = true;
	exename = aExeName;

	hc = new HttpConnection();
	hc->addListener(this);
	hc->downloadFile(aUrl);
}

void AutoUpdater::endData(const string& aLine /*= Util::emptyString*/, bool success /*= false*/) {
	if(file != NULL) {
		try {
			file->flush();
			delete file;
			file = NULL;
		} catch(const FileException& e) {
			updating = false;
			fire(UpdaterListener::Failed(), this, e.getError());
			return;
		}
	}

	if(aLine == STRING(TOO_MUCH_DATA)) {
		hc->removeListener(this);
		delete hc;
		hc = NULL;
	}

	if(success && Util::fileExists(Util::getSystemPath() + "Update.zip")) {
		try {
			File::renameFile(exename, Util::getSystemPath() + "ApexDC.exe.bak");
			if(Util::fileExists(Util::getDataPath() + "ApexDC.pdb")) {
				File::deleteFile(Util::getDataPath() + "ApexDC.pdb");
			}
		} catch(FileException& e) {
			updating = false;
			File::deleteFile(Util::getSystemPath() + "Update.zip");
			fire(UpdaterListener::Failed(), this, e.getError());
			return;
		}

		// Unzip the update
		try {
			ZipFile zip(Util::getSystemPath() + "Update.zip");
			if(zip.GoToFirstFile()) {
				do {
					zip.OpenCurrentFile();
					if(zip.GetCurrentFileName().find(".exe") != string::npos) {
						zip.ReadCurrentFile(exename);
					} else {
						zip.ReadCurrentFile(Util::getSystemPath() + zip.GetCurrentFileName());
					}
					zip.CloseCurrentFile();
				} while(zip.GoToNextFile());
			}
		} catch(ZipFileException& e) {
			updating = false;
			File::deleteFile(Util::getSystemPath() + "Update.zip");
			fire(UpdaterListener::Failed(), this, e.getError());
			return;
		}

		File::deleteFile(Util::getSystemPath() + "Update.zip");
		fire(UpdaterListener::Complete(), this, aLine);
		return;
	}

	updating = false;
	fire(UpdaterListener::Failed(), this, aLine);
}

}