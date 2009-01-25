#ifndef DCPLUSPLUS_DCPP_DOWNLOAD_H_
#define DCPLUSPLUS_DCPP_DOWNLOAD_H_

#include "forward.h"
#include "Transfer.h"
#include "MerkleTree.h"
#include "Flags.h"
#include "Streams.h"
#include "QueueItem.h"

namespace dcpp {

/**
 * Comes as an argument in the DownloadManagerListener functions.
 * Use it to retrieve information about the ongoing transfer.
 */
class Download : public Transfer, public Flags {
public:

	enum {
		FLAG_ZDOWNLOAD			= 0x01,
		FLAG_CHUNKED			= 0x02,
		FLAG_TTH_CHECK			= 0x04,
		FLAG_TESTSUR			= 0x08,
		FLAG_CHECK_FILE_LIST	= 0x10,
		FLAG_SLOWUSER			= 0x20,
		FLAG_XML_BZ_LIST		= 0x40,
		FLAG_PARTIAL			= 0x80,
		FLAG_OVERLAP			= 0x100
	};

	Download(UserConnection& conn, const string& pfsDir) throw();
	Download(UserConnection& conn, QueueItem& qi) throw();

	void getParams(const UserConnection& aSource, StringMap& params);

	~Download();

	/** @return Target filename without path. */
	string getTargetFileName() const {
		return Util::getFileName(getPath());
	}

	/** @internal */
	string getDownloadTarget() const {
		const string& tgt = (getTempTarget().empty() ? getPath() : getTempTarget());
		return tgt;
	}

	/** @internal */
	TigerTree& getTigerTree() { return tt; }
	string& getPFS() { return pfs; }
	
	const TigerTree& getTigerTree() const { return tt; }
	const string& getPFS() const { return pfs; }

	/** @internal */
	AdcCommand getCommand(bool zlib) const;

	GETSET(string, tempTarget, TempTarget);
	GETSET(uint64_t, lastTick, LastTick);
	GETSET(OutputStream*, file, File);
	GETSET(bool, treeValid, TreeValid);
private:
	Download(const Download&);
	Download& operator=(const Download&);

	TigerTree tt;
	string pfs;
};

} // namespace dcpp

#endif /*DOWNLOAD_H_*/
