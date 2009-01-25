/*
	Copyright (C) 2004-2005 Cory Nelson

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation would be
		appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	
	CVS Info :
		$Author: phrostbyte $
		$Date: 2005/06/03 08:57:00 $
		$Revision: 1.6 $
*/

/*
	Updated for DC++ base code @ 2008, by Crise
*/

#include "stdinc.h"
#include "ZipFile.h"
#include "Util.h"

namespace dcpp {

string ZipFileException::TranslateError(int e) {
	switch(e) {
		case UNZ_END_OF_LIST_OF_FILE:		return "end of file list reached";
		case UNZ_EOF:						return "end of file reached";
		case UNZ_PARAMERROR:				return "invalid parameter given";
		case UNZ_BADZIPFILE:				return "bad zip file";
		case UNZ_INTERNALERROR:				return "internal error";
		case UNZ_CRCERROR:					return "crc error, file is corrupt";
		case UNZ_ERRNO:						return strerror(errno);
		default:							return "unknown error (" + Util::translateError(e) + ")";
	}
}

ZipFile::ZipFile(const string &file) throw(ZipFileException) : fp(NULL) {
	this->Open(file);
}

ZipFile::~ZipFile() throw(ZipFileException) {
	this->Close();
}

void ZipFile::Open(const string &file) throw(ZipFileException) {
	this->Close();
	this->fp = unzOpen(file.c_str());
	if(this->fp == NULL) throw ZipFileException("unzOpen");
}

void ZipFile::Close() throw(ZipFileException) {
	if(this->IsOpen()) {
		int ret = unzClose(this->fp);
		if(ret != UNZ_OK) throw ZipFileException("unzClose", ret);
		this->fp=NULL;
	}
}

bool ZipFile::IsOpen() const {
	return (this->fp != NULL);
}

bool ZipFile::GoToFirstFile() {
	return (unzGoToFirstFile(this->fp) == UNZ_OK);
}

bool ZipFile::GoToNextFile() {
	return (unzGoToNextFile(this->fp) == UNZ_OK);
}

void ZipFile::OpenCurrentFile() throw(ZipFileException) {
	int ret = unzOpenCurrentFile(this->fp);
	if(ret != UNZ_OK) throw ZipFileException("unzOpenCurrentFile", ret);
}

void ZipFile::CloseCurrentFile() throw(ZipFileException) {
	int ret = unzCloseCurrentFile(this->fp);
	if(ret != UNZ_OK) throw ZipFileException("unzCloseCurrentFile", ret);
}

string ZipFile::GetCurrentFileName() throw(ZipFileException) {
	char buf[1024];

	int ret = unzGetCurrentFileInfo(this->fp, NULL, buf, sizeof(buf), NULL, 0, NULL, 0);
	if(ret != UNZ_OK) throw ZipFileException("unzGetCurrentFileInfo", ret);

	return buf;
}

pair<BYTE*,size_t> ZipFile::ReadCurrentFile() throw(ZipFileException) {
	unz_file_info info;
	uLong ret = unzGetCurrentFileInfo(this->fp, &info, NULL, 0, NULL, 0, NULL, 0);
	if(ret != UNZ_OK) throw ZipFileException("unzGetCurrentFileInfo", ret);

	BYTE* buf = new BYTE[info.uncompressed_size];

	ret = unzReadCurrentFile(fp, buf, info.uncompressed_size);
	if(ret != info.uncompressed_size) throw ZipFileException("unzReadCurrentFile", ret);

	return make_pair(buf, info.uncompressed_size);
}

void ZipFile::ReadCurrentFile(const string &path) throw(ZipFileException) {
	try {
		pair<BYTE*,size_t> file = this->ReadCurrentFile();

		File f(path, File::WRITE, File::OPEN | File::CREATE | File::TRUNCATE);
		f.setEndPos(0);
		f.write(file.first, file.second);
		f.close();

		delete[] file.first;
	} catch (const Exception& e) {
		throw ZipFileException(e.getError());
	}
}

void ZipFile::ReadFiles(StringMap& files) throw(ZipFileException) {
	try {
		if(this->GoToFirstFile()) {
			do {
				this->OpenCurrentFile();
				pair<BYTE*,size_t> file = this->ReadCurrentFile();
				files[this->GetCurrentFileName()] = string((char*)file.first, file.second);
				delete[] file.first;
				this->CloseCurrentFile();
			} while(this->GoToNextFile());
		}
	} catch (const Exception& e) {
		throw ZipFileException(e.getError());
	}
}

} // namespace dcpp