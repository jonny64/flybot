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
		$Date: 2005/07/02 05:15:58 $
		$Revision: 1.8 $
*/

/*
	Updated for DC++ base code @ 2008, by Crise
*/

#ifndef __ZIPFILE_H__
#define __ZIPFILE_H__

#include "DCPlusPlus.h"
#include "Exception.h"
#include "File.h"

#include "../miniunzip/unzip.h"

namespace dcpp {

class ZipFileException : public Exception 
{
public:
	ZipFileException(const string &func) : Exception(func) { }
	ZipFileException(const string &func, int msg) : Exception(func + ": " + TranslateError(msg)) { }
	ZipFileException(const string &func, const string &msg) : Exception(func + ": " + msg) { }
	virtual ~ZipFileException() throw() { }

private:
	static string TranslateError(int e);
};

class ZipFile : private boost::noncopyable 
{
public:
	ZipFile() : fp(NULL) { }
	ZipFile(const string &file) throw(ZipFileException);
	~ZipFile() throw(ZipFileException);

	void Open(const string &file) throw(ZipFileException);
	void Close() throw(ZipFileException);

	bool IsOpen() const;

	bool GoToFirstFile();
	bool GoToNextFile();

	void OpenCurrentFile() throw(ZipFileException);
	void CloseCurrentFile() throw(ZipFileException);

	string GetCurrentFileName() throw(ZipFileException);
	pair<BYTE*,size_t> ReadCurrentFile() throw(ZipFileException);
	void ReadCurrentFile(const string &path) throw(ZipFileException);
	void ReadFiles(StringMap& files) throw(ZipFileException);

private:
	unzFile fp;
};

} // namespace dcpp

#endif
