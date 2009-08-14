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

#include "Util.h"
#include "File.h"

#include "SettingsManager.h"
#include "ResourceManager.h"
#include "StringTokenizer.h"
#include "SettingsManager.h"
#include "version.h"
#include "File.h"
#include "SimpleXML.h"

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/utsname.h>
#include <ctype.h>
#endif
#include <locale.h>

#include "CID.h"

#include "FastAlloc.h"

namespace dcpp {

#ifndef _DEBUG
FastCriticalSection FastAllocBase::cs;
#endif

long Util::mUptimeSeconds = 0;
string Util::emptyString;
wstring Util::emptyStringW;
tstring Util::emptyStringT;

bool Util::away = false;
string Util::awayMsg;
time_t Util::awayTime;

Util::CountryList Util::countries;
string Util::configPath;
string Util::systemPath;
string Util::dataPath;

static void sgenrand(unsigned long seed);

extern "C" void bz_internal_error(int errcode) { 
	dcdebug("bzip2 internal error: %d\n", errcode); 
}

#if defined(_WIN32) && (_MSC_VER == 1400 || _MSC_VER == 1500)
void WINAPI invalidParameterHandler(const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t) {
	//do nothing, this exist because vs2k5 crt needs it not to crash on errors.
}
#endif

bool nlfound = false;
BOOL CALLBACK GetWOkna(HWND handle, LPARAM) {
	TCHAR buf[256];
	buf[0] = NULL;
	if (!handle) {
		nlfound = false;
		return TRUE;// Not a window
	}
	SendMessageTimeout(handle, WM_GETTEXT, 255, (LPARAM)buf, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, NULL);
	buf[255] = NULL;

	if(buf[0] != NULL) {
		if(_tcsnicmp(buf, _T("NetLimiter"), 10) == 0/* || _tcsnicmp(buf, _T("DU Super Controler"), 18) == 0*/) {
			nlfound = true;
			return false;
		}
	}

	nlfound = false;
	return true;
}

int Util::getNetLimiterLimit() {
	int NetLimiter_UploadLimit = -1;
	int NetLimiter_UploadOn = 0;

	if(GetModuleHandle(_T("nl_lsp.dll")) == 0) return -1;

	try {
		TCHAR AppData[256];
		GetEnvironmentVariable(_T("APPDATA"), AppData, 255);

		File f(Text::fromT(AppData) + "\\LockTime\\NetLimiter\\history\\apphist.dat", File::RW, File::OPEN);

		const size_t BUF_SIZE = 800;

		TCHAR appName[MAX_PATH+1];
		DWORD x = GetModuleFileName(NULL, appName, MAX_PATH);
		string cesta = Text::fromT(tstring(appName, x)) + "/";

		char buf[BUF_SIZE];
		uint32_t len;
		char* w2 = _strdup(cesta.c_str());

		for(;;) {
			size_t n = BUF_SIZE;
			len = f.read(buf, n);
			string txt = Util::emptyString;
			for(uint32_t i = 0; i < len; ++i) {
				if (buf[i]== 0) 
				txt += "/"; else
				txt += buf[i];
			}
			char* w1 = _strdup(txt.c_str());

			if(::strstr(_strupr(w1),_strupr(w2)) != NULL) {
				char buf1[256];
				char buf2[256];

				snprintf(buf1, sizeof(buf1), "%X", uint8_t(buf[5]));
				string a1 = buf1;

				snprintf(buf2, sizeof(buf2), "%X", uint8_t(buf[6]));
				string a2 = buf2;

				char* limit_hex = _strdup(("0x" + a2 + a1).c_str());

				NetLimiter_UploadLimit = 0;

				sscanf(limit_hex,"%x",&NetLimiter_UploadLimit);
				NetLimiter_UploadLimit /= 4;
				delete limit_hex;

				NetLimiter_UploadOn = uint8_t(txt[16]);
				buf[255] = 0;

				if(NetLimiter_UploadOn == 1) {
					EnumWindows(GetWOkna,NULL);
					if(!nlfound) {
						NetLimiter_UploadLimit = -1;
						NetLimiter_UploadOn = 0;
					}
				} else {
					NetLimiter_UploadLimit = -1;
					NetLimiter_UploadOn = 0;
				}
				delete w1;
				break;
			}

			delete w1;

			if(len < BUF_SIZE)
				break;
		}
	
		f.close();
		delete w2;
	} catch(...) {
	}

	return NetLimiter_UploadLimit;
}

void Util::initialize() {
	Text::initialize();

	sgenrand((unsigned long)time(NULL));

#ifdef _WIN32
	TCHAR buf[MAX_PATH+1];
	::GetModuleFileName(NULL, buf, MAX_PATH);
	// System config path is DC++ executable path...
	systemPath = Util::getFilePath(Text::fromT(buf));
	configPath = systemPath + "Settings\\";
	dataPath = systemPath;
	
#else
	systemPath = "/etc/";
	char* home = getenv("HOME");
	configPath = home ? Text::toUtf8(home) + string("/.dc++/") : "/tmp/";
	dataPath = configPath; // dataPath in linux is usually prefix + /share/app_name, so we can't represent it here
#endif

	// Load boot settings
	try {
		SimpleXML boot;
		boot.fromXML(File(systemPath + "dcppboot.xml", File::READ, File::OPEN).read());
		boot.stepIn();

		if(boot.findChild("ConfigPath")) {
			StringMap params;
#ifdef _WIN32
			TCHAR path[MAX_PATH];

			params["APPDATA"] = Text::fromT((::SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path), path));
			params["PERSONAL"] = Text::fromT((::SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path), path));
			configPath = Util::formatParams(boot.getChildData(), params, false);
#endif
		}
	} catch(const Exception& ) {
		// Unable to load boot settings...
	}

	if(!File::isAbsolute(configPath)) {
		configPath = systemPath + configPath;
	}

#if _MSC_VER == 1400 || _MSC_VER == 1500
	_set_invalid_parameter_handler(reinterpret_cast<_invalid_parameter_handler>(invalidParameterHandler));
#endif

	try {
		// This product includes GeoIP data created by MaxMind, available from http://maxmind.com/
		// Updates at http://www.maxmind.com/app/geoip_country
		string file = Util::getConfigPath() + "GeoIpCountryWhois.csv";
		string data = File(file, File::READ, File::OPEN).read();

		const char* start = data.c_str();
		string::size_type linestart = 0;
		string::size_type comma1 = 0;
		string::size_type comma2 = 0;
		string::size_type comma3 = 0;
		string::size_type comma4 = 0;
		string::size_type lineend = 0;
		CountryIter last = countries.end();
		uint32_t startIP = 0;
		uint32_t endIP = 0, endIPprev = 0;

		for(;;) {
			comma1 = data.find(',', linestart);
			if(comma1 == string::npos) break;
			comma2 = data.find(',', comma1 + 1);
			if(comma2 == string::npos) break;
			comma3 = data.find(',', comma2 + 1);
			if(comma3 == string::npos) break;
			comma4 = data.find(',', comma3 + 1);
			if(comma4 == string::npos) break;
			lineend = data.find('\n', comma4);
			if(lineend == string::npos) break;

			startIP = Util::toUInt32(start + comma2 + 2);
			endIP = Util::toUInt32(start + comma3 + 2);
			uint16_t* country = (uint16_t*)(start + comma4 + 2);
			if((startIP-1) != endIPprev)
				last = countries.insert(last, make_pair((startIP-1), (uint16_t)16191));
			last = countries.insert(last, make_pair(endIP, *country));

			endIPprev = endIP;
			linestart = lineend + 1;
		}
	} catch(const FileException&) {
	}
	File::ensureDirectory(Util::getConfigPath());
}

#ifdef _WIN32
static const char badChars[] = { 
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
		17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
		31, '<', '>', '/', '"', '|', '?', '*', 0
};
#else

static const char badChars[] = { 
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
	31, '<', '>', '\\', '"', '|', '?', '*', 0
};
#endif

/**
 * Replaces all strange characters in a file with '_'
 * @todo Check for invalid names such as nul and aux...
 */
string Util::validateFileName(string tmp) {
	string::size_type i = 0;

	// First, eliminate forbidden chars
	while( (i = tmp.find_first_of(badChars, i)) != string::npos) {
		tmp[i] = '_';
		i++;
	}

	// Then, eliminate all ':' that are not the second letter ("c:\...")
	i = 0;
	while( (i = tmp.find(':', i)) != string::npos) {
		if(i == 1) {
			i++;
			continue;
		}
		tmp[i] = '_';	
		i++;
	}

	// Remove the .\ that doesn't serve any purpose
	i = 0;
	while( (i = tmp.find("\\.\\", i)) != string::npos) {
		tmp.erase(i+1, 2);
	}
	i = 0;
	while( (i = tmp.find("/./", i)) != string::npos) {
		tmp.erase(i+1, 2);
	}

	// Remove any double \\ that are not at the beginning of the path...
	i = 1;
	while( (i = tmp.find("\\\\", i)) != string::npos) {
		tmp.erase(i+1, 1);
	}
	i = 1;
	while( (i = tmp.find("//", i)) != string::npos) {
		tmp.erase(i+1, 1);
	}

	// And last, but not least, the infamous ..\! ...
	i = 0;
	while( ((i = tmp.find("\\..\\", i)) != string::npos) ) {
		tmp[i + 1] = '_';
		tmp[i + 2] = '_';
		tmp[i + 3] = '_';
		i += 2;
	}
	i = 0;
	while( ((i = tmp.find("/../", i)) != string::npos) ) {
		tmp[i + 1] = '_';
		tmp[i + 2] = '_';
		tmp[i + 3] = '_';
		i += 2;
	}

	// Dots at the end of path names aren't popular
	i = 0;
	while( ((i = tmp.find(".\\", i)) != string::npos) ) {
		if(i != 0)
			tmp[i] = '_';
		i += 1;
	}
	i = 0;
	while( ((i = tmp.find("./", i)) != string::npos) ) {
		if(i != 0)
			tmp[i] = '_';
		i += 1;
	}


	return tmp;
}

string Util::cleanPathChars(string aNick) {
	string::size_type i = 0;

	while( (i = aNick.find_first_of("/.\\", i)) != string::npos) {
		aNick[i] = '_';
	}
	return aNick;
}

string Util::getShortTimeString() {
	char buf[255];
	time_t _tt = time(NULL);
	tm* _tm = localtime(&_tt);
	if(_tm == NULL) {
		strcpy(buf, "xx:xx");
	} else {
		strftime(buf, 254, SETTING(TIME_STAMPS_FORMAT).c_str(), _tm);
	}
	return Text::toUtf8(buf);
}

/**
 * Decodes a URL the best it can...
 * Default ports:
 * http:// -> port 80
 * dchub:// -> port 411
 */
void Util::decodeUrl(const string& url, string& aServer, uint16_t& aPort, string& aFile) {
	// First, check for a protocol: xxxx://
	string::size_type i = 0, j, k;
	
	aServer = emptyString;
	aFile = emptyString;

	if( (j=url.find("://", i)) != string::npos) {
		// Protocol found
		string protocol = url.substr(0, j);
		i = j + 3;

		if(protocol == "http") {
			aPort = 80;
		} else if(protocol == "dchub") {
			aPort = 411;
		}
	}

	if( (j=url.find('/', i)) != string::npos) {
		// We have a filename...
		aFile = url.substr(j);
	}

	if( (k=url.find(':', i)) != string::npos) {
		// Port
		if(j == string::npos) {
			aPort = static_cast<uint16_t>(Util::toInt(url.substr(k+1)));
		} else if(k < j) {
			aPort = static_cast<uint16_t>(Util::toInt(url.substr(k+1, j-k-1)));
		}
	} else {
		k = j;
	}

	if(k == string::npos) {
		aServer = url.substr(i);
		if(i==0) aPort = 411;
	} else
		aServer = url.substr(i, k-i);
}

void Util::setAway(bool aAway) {
	away = aAway;

	SettingsManager::getInstance()->set(SettingsManager::AWAY, aAway);

	if (away)
		awayTime = time(NULL);
}

void Util::setLimiter(bool aLimiter ) {
	SettingsManager::getInstance()->set(SettingsManager::THROTTLE_ENABLE, aLimiter);
}

string Util::getAwayMessage() { 
	Util::increaseUptime();
	time_t currentTime;
	time(&currentTime);
	int currentHour = localtime(&currentTime)->tm_hour;

	if(BOOLSETTING(AWAY_TIME_THROTTLE) && ((SETTING(AWAY_START) < SETTING(AWAY_END) && 
			currentHour >= SETTING(AWAY_START) && currentHour < SETTING(AWAY_END)) ||
		(SETTING(AWAY_START) > SETTING(AWAY_END) &&
			(currentHour >= SETTING(AWAY_START) || currentHour < SETTING(AWAY_END))))) {
		return (formatTime(awayMsg.empty() ? SETTING(SECONDARY_AWAY_MESSAGE) : awayMsg, awayTime));
	} else {
		return (formatTime(awayMsg.empty() ? SETTING(DEFAULT_AWAY_MESSAGE) : awayMsg, awayTime));
	}
}

string Util::formatBytes(int64_t aBytes) {
	char buf[64];
	if(aBytes < 1024) {
		snprintf(buf, sizeof(buf), "%d %s", (int)(aBytes&0xffffffff), CSTRING(B));
	} else if(aBytes < 1048576) {
		snprintf(buf, sizeof(buf), "%.02f %s", (double)aBytes/(1024.0), CSTRING(KB));
	} else if(aBytes < 1073741824) {
		snprintf(buf, sizeof(buf), "%.02f %s", (double)aBytes/(1048576.0), CSTRING(MB));
	} else if(aBytes < (int64_t)1099511627776) {
		snprintf(buf, sizeof(buf), "%.02f %s", (double)aBytes/(1073741824.0), CSTRING(GB));
	} else if(aBytes < (int64_t)1125899906842624) {
		snprintf(buf, sizeof(buf), "%.02f %s", (double)aBytes/(1099511627776.0), CSTRING(TB));
	} else if(aBytes < (int64_t)1152921504606846976)  {
		snprintf(buf, sizeof(buf), "%.02f %s", (double)aBytes/(1125899906842624.0), CSTRING(PB));
	} else {
		snprintf(buf, sizeof(buf), "%.02f %s", (double)aBytes/(1152921504606846976.0), CSTRING(EB));
	}

	return buf;
}

wstring Util::formatBytesW(int64_t aBytes) {
	wchar_t buf[64];
	if(aBytes < 1024) {
		snwprintf(buf, sizeof(buf), L"%d %s", (int)(aBytes&0xffffffff), CWSTRING(B));
	} else if(aBytes < 1048576) {
		snwprintf(buf, sizeof(buf), L"%.02f %s", (double)aBytes/(1024.0), CWSTRING(KB));
	} else if(aBytes < 1073741824) {
		snwprintf(buf, sizeof(buf), L"%.02f %s", (double)aBytes/(1048576.0), CWSTRING(MB));
	} else if(aBytes < (int64_t)1099511627776) {
		snwprintf(buf, sizeof(buf), L"%.02f %s", (double)aBytes/(1073741824.0), CWSTRING(GB));
	} else if(aBytes < (int64_t)1125899906842624) {
		snwprintf(buf, sizeof(buf), L"%.02f %s", (double)aBytes/(1099511627776.0), CWSTRING(TB));
	} else if(aBytes < (int64_t)1152921504606846976)  {
		snwprintf(buf, sizeof(buf), L"%.02f %s", (double)aBytes/(1125899906842624.0), CWSTRING(PB));
	} else {
		snwprintf(buf, sizeof(buf), L"%.02f %s", (double)aBytes/(1152921504606846976.0), CWSTRING(EB));
	}

	return buf;
}

wstring Util::formatExactSize(int64_t aBytes) {
#ifdef _WIN32	
	wchar_t buf[64];
	wchar_t number[64];
	NUMBERFMT nf;
	snwprintf(number, sizeof(number), _T("%I64d"), aBytes);
	wchar_t Dummy[16];
	TCHAR sep[2] = _T(",");
    
	/*No need to read these values from the system because they are not
	used to format the exact size*/
	nf.NumDigits = 0;
	nf.LeadingZero = 0;
	nf.NegativeOrder = 0;
	nf.lpDecimalSep = sep;

	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SGROUPING, Dummy, 16 );
	nf.Grouping = _wtoi(Dummy);
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, Dummy, 16 );
	nf.lpThousandSep = Dummy;

	GetNumberFormat(LOCALE_USER_DEFAULT, 0, number, &nf, buf, 64);
		
	snwprintf(buf, sizeof(buf), _T("%s %s"), buf, CTSTRING(B));
	return buf;
#else
		wchar_t buf[64];
		snwprintf(buf, sizeof(buf), L"%'lld", (long long int)aBytes);
		return tstring(buf) + TSTRING(B);
#endif
}

string Util::getLocalIp() {
	string tmp;
	
	char buf[256];
	gethostname(buf, 255);
	hostent* he = gethostbyname(buf);
	if(he == NULL || he->h_addr_list[0] == 0)
		return Util::emptyString;
	sockaddr_in dest;
	int i = 0;
	
	// We take the first ip as default, but if we can find a better one, use it instead...
	memcpy(&(dest.sin_addr), he->h_addr_list[i++], he->h_length);
	tmp = inet_ntoa(dest.sin_addr);
	if(Util::isPrivateIp(tmp) || strncmp(tmp.c_str(), "169", 3) == 0) {
		while(he->h_addr_list[i]) {
			memcpy(&(dest.sin_addr), he->h_addr_list[i], he->h_length);
			string tmp2 = inet_ntoa(dest.sin_addr);
			if(!Util::isPrivateIp(tmp2) && strncmp(tmp2.c_str(), "169", 3) != 0) {
				tmp = tmp2;
			}
			i++;
		}
	}
	return tmp;
}

bool Util::isPrivateIp(string const& ip) {
	struct in_addr addr;

	addr.s_addr = inet_addr(ip.c_str());

	if (addr.s_addr  != INADDR_NONE) {
		unsigned long haddr = ntohl(addr.s_addr);
		return ((haddr & 0xff000000) == 0x0a000000 || // 10.0.0.0/8
				(haddr & 0xff000000) == 0x7f000000 || // 127.0.0.0/8
				(haddr & 0xffff0000) == 0xa9fe0000 || // 169.254.0.0/16
				(haddr & 0xfff00000) == 0xac100000 || // 172.16.0.0/12
				(haddr & 0xffff0000) == 0xc0a80000);  // 192.168.0.0/16
	}
	return false;
}

typedef const uint8_t* ccp;
static wchar_t utf8ToLC(ccp& str) {
	wchar_t c = 0;
   if(str[0] & 0x80) { 
      if(str[0] & 0x40) { 
         if(str[0] & 0x20) { 
            if(str[1] == 0 || str[2] == 0 ||
				!((((unsigned char)str[1]) & ~0x3f) == 0x80) ||
					!((((unsigned char)str[2]) & ~0x3f) == 0x80))
				{
					str++;
					return 0;
				}
				c = ((wchar_t)(unsigned char)str[0] & 0xf) << 12 |
					((wchar_t)(unsigned char)str[1] & 0x3f) << 6 |
					((wchar_t)(unsigned char)str[2] & 0x3f);
				str += 3;
			} else {
				if(str[1] == 0 ||
					!((((unsigned char)str[1]) & ~0x3f) == 0x80)) 
				{
					str++;
					return 0;
				}
				c = ((wchar_t)(unsigned char)str[0] & 0x1f) << 6 |
					((wchar_t)(unsigned char)str[1] & 0x3f);
				str += 2;
			}
		} else {
			str++;
			return 0;
		}
	} else {
		wchar_t c = Text::asciiToLower((char)str[0]);
		str++;
		return c;
	}

	return Text::toLower(c);
}

string::size_type Util::findSubString(const string& aString, const string& aSubString, string::size_type start) throw() {
	if(aString.length() < start)
		return (string::size_type)string::npos;

	if(aString.length() - start < aSubString.length())
		return (string::size_type)string::npos;

	if(aSubString.empty())
		return 0;

	// Hm, should start measure in characters or in bytes? bytes for now...
	const uint8_t* tx = (const uint8_t*)aString.c_str() + start;
	const uint8_t* px = (const uint8_t*)aSubString.c_str();

	const uint8_t* end = tx + aString.length() - start - aSubString.length() + 1;

	wchar_t wp = utf8ToLC(px);

	while(tx < end) {
		const uint8_t* otx = tx;
		if(wp == utf8ToLC(tx)) {
			const uint8_t* px2 = px;
			const uint8_t* tx2 = tx;

			for(;;) {
				if(*px2 == 0)
					return otx - (uint8_t*)aString.c_str();

				if(utf8ToLC(px2) != utf8ToLC(tx2))
					break;
			}
		}
	}
	return (string::size_type)string::npos;
}

wstring::size_type Util::findSubString(const wstring& aString, const wstring& aSubString, wstring::size_type pos) throw() {
	if(aString.length() < pos)
		return static_cast<wstring::size_type>(wstring::npos);

	if(aString.length() - pos < aSubString.length())
		return static_cast<wstring::size_type>(wstring::npos);

	if(aSubString.empty())
		return 0;

	wstring::size_type j = 0;
	wstring::size_type end = aString.length() - aSubString.length() + 1;

	for(; pos < end; ++pos) {
		if(Text::toLower(aString[pos]) == Text::toLower(aSubString[j])) {
			wstring::size_type tmp = pos+1;
			bool found = true;
			for(++j; j < aSubString.length(); ++j, ++tmp) {
				if(Text::toLower(aString[tmp]) != Text::toLower(aSubString[j])) {
					j = 0;
					found = false;
					break;
				}
			}

			if(found)
				return pos;
		}
	}
	return static_cast<wstring::size_type>(wstring::npos);
}

int Util::stricmp(const char* a, const char* b) {
	while(*a) {
			wchar_t ca = 0, cb = 0;
			int na = Text::utf8ToWc(a, ca);
			int nb = Text::utf8ToWc(b, cb);
			ca = Text::toLower(ca);
			cb = Text::toLower(cb);
			if(ca != cb) {
			return (int)ca - (int)cb;
			}
		a += abs(na);
		b += abs(nb);
	}
	wchar_t ca = 0, cb = 0;
	Text::utf8ToWc(a, ca);
	Text::utf8ToWc(b, cb);

	return (int)Text::toLower(ca) - (int)Text::toLower(cb);
}

int Util::strnicmp(const char* a, const char* b, size_t n) {
	const char* end = a + n;
	while(*a && a < end) {
		wchar_t ca = 0, cb = 0;
		int na = Text::utf8ToWc(a, ca);
		int nb = Text::utf8ToWc(b, cb);
		ca = Text::toLower(ca);
		cb = Text::toLower(cb);
		if(ca != cb) {
			return (int)ca - (int)cb;
		}
		a += abs(na);
		b += abs(nb);
	}
	wchar_t ca = 0, cb = 0;
	Text::utf8ToWc(a, ca);
	Text::utf8ToWc(b, cb);
	return (a >= end) ? 0 : ((int)Text::toLower(ca) - (int)Text::toLower(cb));
}

string Util::encodeURI(const string& aString, bool reverse) {
	// reference: rfc2396
	string tmp = aString;
	if(reverse) {
		string::size_type idx;
		for(idx = 0; idx < tmp.length(); ++idx) {
			if(tmp.length() > idx + 2 && tmp[idx] == '%' && isxdigit(tmp[idx+1]) && isxdigit(tmp[idx+2])) {
				tmp[idx] = fromHexEscape(tmp.substr(idx+1,2));
				tmp.erase(idx+1, 2);
			} else { // reference: rfc1630, magnet-uri draft
				if(tmp[idx] == '+')
					tmp[idx] = ' ';
			}
		}
	} else {
		const string disallowed = ";/?:@&=+$," // reserved
			                      "<>#%\" "    // delimiters
		                          "{}|\\^[]`"; // unwise
		string::size_type idx, loc;
		for(idx = 0; idx < tmp.length(); ++idx) {
			if(tmp[idx] == ' ') {
				tmp[idx] = '+';
			} else {
				if(tmp[idx] <= 0x1F || tmp[idx] >= 0x7f || (loc = disallowed.find_first_of(tmp[idx])) != string::npos) {
					tmp.replace(idx, 1, toHexEscape(tmp[idx]));
					idx+=2;
				}
			}
		}
	}
	return tmp;
}

/**
 * This function takes a string and a set of parameters and transforms them according to
 * a simple formatting rule, similar to strftime. In the message, every parameter should be
 * represented by %[name]. It will then be replaced by the corresponding item in 
 * the params stringmap. After that, the string is passed through strftime with the current
 * date/time and then finally written to the log file. If the parameter is not present at all,
 * it is removed from the string completely...
 */
string Util::formatParams(const string& msg, StringMap& params, bool filter) {
	string result = msg;

	string::size_type i, j, k;
	i = 0;
	while (( j = result.find("%[", i)) != string::npos) {
		if( (result.size() < j + 2) || ((k = result.find(']', j + 2)) == string::npos) ) {
			break;
		}
		string name = result.substr(j + 2, k - j - 2);
		StringMapIter smi = params.find(name);
		if(smi == params.end()) {
			result.erase(j, k-j + 1);
			i = j;
		} else {
			if(smi->second.find_first_of("%\\./") != string::npos) {
				string tmp = smi->second;	// replace all % in params with %% for strftime
				string::size_type m = 0;
				while(( m = tmp.find('%', m)) != string::npos) {
					tmp.replace(m, 1, "%%");
					m+=2;
				}
				if(filter) {
					// Filter chars that produce bad effects on file systems
					m = 0;
					while(( m = tmp.find_first_of("\\./", m)) != string::npos) {
						tmp[m] = '_';
					}
				}
				
				result.replace(j, k-j + 1, tmp);
				i = j + tmp.size();
			} else {
				result.replace(j, k-j + 1, smi->second);
				i = j + smi->second.size();
			}
		}
	}

	result = formatTime(result, time(NULL));
	
	return result;
}

/** Fix for wide formatting bug in wcsftime in the ms c lib for multibyte encodings of unicode in singlebyte locales */
string fixedftime(const string& format, struct tm* t) {
	string ret = format;
	const char codes[] = "aAbBcdHIjmMpSUwWxXyYzZ%";

	char tmp[4];
	tmp[0] = '%';
	tmp[1] = tmp[2] = tmp[3] = 0;

	StringMap sm;
	static const size_t BUF_SIZE = 1024;
	boost::scoped_array<char> buf(new char[BUF_SIZE]);
	for(size_t i = 0; i < strlen(codes); ++i) {
		tmp[1] = codes[i];
		tmp[2] = 0;
		strftime(&buf[0], BUF_SIZE-1, tmp, t);
		sm[tmp] = &buf[0];

		tmp[1] = '#';
		tmp[2] = codes[i];
		strftime(&buf[0], BUF_SIZE-1, tmp, t);
		sm[tmp] = &buf[0]; 
	}

	for(StringMapIter i = sm.begin(); i != sm.end(); ++i) {
		for(string::size_type j = ret.find(i->first); j != string::npos; j = ret.find(i->first, j)) {
			ret.replace(j, i->first.length(), i->second);
			j += i->second.length() - i->first.length();
		}
	}

	return ret;
}

string Util::formatRegExp(const string& msg, StringMap& params) {
	string result = msg;
	string::size_type i, j, k;
	i = 0;
	while (( j = result.find("%[", i)) != string::npos) {
		if( (result.size() < j + 2) || ((k = result.find(']', j + 2)) == string::npos) ) {
			break;
		}
		string name = result.substr(j + 2, k - j - 2);
		StringMapIter smi = params.find(name);
		if(smi != params.end()) {
			result.replace(j, k-j + 1, smi->second);
			i = j + smi->second.size();
		} else {
			i = k + 1;
		}
	}
	return result;
}

uint64_t Util::getDirSize(const string &sFullPath) {
	uint64_t total = 0;

	WIN32_FIND_DATA fData;
	HANDLE hFind;
	
	hFind = FindFirstFile(Text::toT(sFullPath + "\\*").c_str(), &fData);

	if(hFind != INVALID_HANDLE_VALUE) {
		do {
			string name = Text::fromT(fData.cFileName);
			if(name == "." || name == "..")
				continue;
			if(name.find('$') != string::npos)
				continue;
			if(!BOOLSETTING(SHARE_HIDDEN) && (fData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
				continue;
			if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				string newName = sFullPath + PATH_SEPARATOR + name;
				if(Util::stricmp(newName + PATH_SEPARATOR, SETTING(TEMP_DOWNLOAD_DIRECTORY)) != 0) {
					total += getDirSize(newName);
				}
			} else {
				// Not a directory, assume it's a file...make sure we're not sharing the settings file...
				if( (Util::stricmp(name.c_str(), "DCPlusPlus.xml") != 0) && 
					(Util::stricmp(name.c_str(), "Favorites.xml") != 0)) {

					total+=(uint64_t)fData.nFileSizeLow | ((uint64_t)fData.nFileSizeHigh)<<32;
				}
			}
		} while(FindNextFile(hFind, &fData));
	}
	
	FindClose(hFind);

	return total;
}

bool Util::validatePath(const string &sPath) {
	if(sPath.empty())
		return false;

	if((sPath.substr(1, 2) == ":\\") || (sPath.substr(0, 2) == "\\\\")) {
		if(GetFileAttributes(Text::toT(sPath).c_str()) & FILE_ATTRIBUTE_DIRECTORY)
			return true;
	}

	return false;
}

bool Util::fileExists(const string &aFile) {
	DWORD attr = GetFileAttributes(Text::toT(aFile).c_str());
	return (attr != 0xFFFFFFFF);
}

string Util::formatTime(const string &msg, const time_t t) {
	if (!msg.empty()) {
		string ret = msg;
		size_t bufsize = msg.size() + 256;
		struct tm* loc = localtime(&t);

		if(!loc) {
			return Util::emptyString;
		}

		// Work it around :P
		string::size_type i = 0;
		while((i = ret.find("%", i)) != string::npos) {
			if(string("aAbBcdHIjmMpSUwWxXyYzZ%").find(ret[i+1]) == string::npos) {
				ret.replace(i, 1, "%%");
			}
			i += 2;
		}

#if _WIN32
		tstring buf(bufsize, 0);

		buf.resize(_tcsftime(&buf[0], buf.size()-1, Text::toT(ret).c_str(), loc));

		if(buf.empty()) {
			return fixedftime(ret, loc);
		}

		return Text::fromT(buf);
#else
		// will this give wide representations for %a and %A?
		// surely win32 can't have a leg up on linux/unixen in this area. - Todd
		string buf(bufsize, 0);

		buf.resize(strftime(&buf[0], bufsize-1, ret.c_str(), loc));
		
		while(buf.empty()) {
			bufsize+=64;
			buf.resize(bufsize);
			buf.resize(strftime(&buf[0], bufsize-1, msg.c_str(), loc));
		}

		return Text::toUtf8(buf);
#endif
	}
	return Util::emptyString;
}

/* Below is a high-speed random number generator with much
   better granularity than the CRT one in msvc...(no, I didn't
   write it...see copyright) */ 
/* Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.
   Any feedback is very welcome. For any question, comments,       
   see http://www.math.keio.ac.jp/matumoto/emt.html or email       
   matumoto@math.keio.ac.jp */       
/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializing the array with a NONZERO seed */
static void sgenrand(unsigned long seed) {
	/* setting initial seeds to mt[N] using         */
	/* the generator Line 25 of Table 1 in          */
	/* [KNUTH 1981, The Art of Computer Programming */
	/*    Vol. 2 (2nd Ed.), pp102]                  */
	mt[0]= seed & 0xffffffff;
	for (mti=1; mti<N; mti++)
		mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}

uint32_t Util::rand() {
	unsigned long y;
	static unsigned long mag01[2]={0x0, MATRIX_A};
	/* mag01[x] = x * MATRIX_A  for x=0,1 */

	if (mti >= N) { /* generate N words at one time */
		int kk;

		if (mti == N+1)   /* if sgenrand() has not been called, */
			sgenrand(4357); /* a default initial seed is used   */

		for (kk=0;kk<N-M;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		for (;kk<N-1;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
		mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

		mti = 0;
	}

	y = mt[mti++];
	y ^= TEMPERING_SHIFT_U(y);
	y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
	y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
	y ^= TEMPERING_SHIFT_L(y);

	return y; 
}

/*	getIpCountry
	This function returns the country(Abbreviation) of an ip
	for exemple: it returns "PT", whitch standards for "Portugal"
	more info: http://www.maxmind.com/app/csv
*/
string Util::getIpCountry (const string& IP) {
	if (BOOLSETTING(GET_USER_COUNTRY)) {
		dcassert(count(IP.begin(), IP.end(), _T('.')) == 3);

		//e.g IP 23.24.25.26 : w=23, x=24, y=25, z=26
		string::size_type a = IP.find(_T('.'));
		string::size_type b = IP.find(_T('.'), a+1);
		string::size_type c = IP.find(_T('.'), b+2);

		uint32_t ipnum = (Util::toUInt32(IP.c_str()) << 24) | 
			(Util::toUInt32(IP.c_str() + a + 1) << 16) | 
			(Util::toUInt32(IP.c_str() + b + 1) << 8) | 
			(Util::toUInt32(IP.c_str() + c + 1) );

		CountryList::const_iterator i = countries.lower_bound(ipnum);

		if(i != countries.end()) {
			return string((char*)&(i->second), 2);
		}
	}

	return Util::emptyString; //if doesn't returned anything already, something is wrong...
}

string Util::formatMessage(const string& nick, const string& message, bool thirdPerson) {
	// let's *not* obey the spec here and add a space after the star. :P
	string tmp = (thirdPerson ? "* " + nick + " " : "<" + nick + "> ") + message;
	
	// Check all '<' and '[' after newlines as they're probably pasts...
	size_t i = 0;
	while( (i = tmp.find('\n', i)) != string::npos) {
		if(i + 1 < tmp.length()) {
			if(tmp[i+1] == '[' || tmp[i+1] == '<') {
				tmp.insert(i+1, "- ");
				i += 2;
			}
		}
		i++;
	}

	return Text::toDOS(tmp);
}

string Util::getTimeString() {
	char buf[64];
	time_t _tt;
	time(&_tt);
	tm* _tm = localtime(&_tt);
	if(_tm == NULL) {
		strcpy(buf, "xx:xx:xx");
	} else {
		strftime(buf, 64, "%X", _tm);
	}
	return buf;
}

string Util::toAdcFile(const string& file) {
	if(file == "files.xml.bz2" || file == "files.xml")
		return file;

	string ret;
	ret.reserve(file.length() + 1);
	ret += '/';
	ret += file;
	for(string::size_type i = 0; i < ret.length(); ++i) {
		if(ret[i] == '\\') {
			ret[i] = '/';
		}
	}
	return ret;
}
string Util::toNmdcFile(const string& file) {
	if(file.empty())
		return Util::emptyString;

	string ret(file.substr(1));
	for(string::size_type i = 0; i < ret.length(); ++i) {
		if(ret[i] == '/') {
			ret[i] = '\\';
		}
	}
	return ret;
}

TCHAR* Util::strstr(const TCHAR *str1, const TCHAR *str2, int *pnIdxFound) {
	TCHAR *s1, *s2;
	TCHAR *cp = (TCHAR *)str1;
	if (!*str2)
		return (TCHAR *)str1;
	int nIdx = 0;
	while (*cp) {
		s1 = cp;
		s2 = (TCHAR *) str2;
                while(*s1 && *s2 && !(*s1-*s2))
                        s1++, s2++;
		if (!*s2) {
			if (pnIdxFound != NULL)
				*pnIdxFound = nIdx;
			return cp;
		}
		cp++;
		nIdx++;
	}
	if (pnIdxFound != NULL)
		*pnIdxFound = -1;
	return NULL;
}

/* natural sorting */ 

int Util::DefaultSort(const wchar_t *a, const wchar_t *b, bool noCase /*=  true*/) {
	if(BOOLSETTING(NAT_SORT)) {
		int v1, v2;
		while(*a != 0 && *b != 0) {
			v1 = 0; v2 = 0;
			bool t1 = isNumeric(*a);
			bool t2 = isNumeric(*b);
			if(t1 != t2) return (t1) ? -1 : 1;

			if(!t1 && noCase) {
				if(Text::toLower(*a) != Text::toLower(*b))
					return ((int)Text::toLower(*a)) - ((int)Text::toLower(*b));
				a++; b++;
			} else if(!t1) {
				if(*a != *b)
					return ((int)*a) - ((int)*b);
				a++; b++;
			} else {
			    while(isNumeric(*a)) {
			       v1 *= 10;
			       v1 += *a - '0';
			       a++;
			    }

	            while(isNumeric(*b)) {
		           v2 *= 10;
		           v2 += *b - '0';
		           b++;
		        }

				if(v1 != v2)
					return (v1 < v2) ? -1 : 1;
			}			
		}

		return noCase ? (((int)Text::toLower(*a)) - ((int)Text::toLower(*b))) : (((int)*a) - ((int)*b));
	} else {
		return noCase ? lstrcmpi(a, b) : lstrcmp(a, b);
	}
}

} // namespace dcpp

/**
 * @file
 * $Id: Util.cpp 382 2008-03-09 10:40:22Z BigMuscle $
 */