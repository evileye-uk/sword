/******************************************************************************
 *  rawstr.cpp   - code for class 'RawStr'- a module that reads raw text
 *				files:  ot and nt using indexs ??.bks ??.cps ??.vss
 *				and provides lookup and parsing functions based on
 *				class StrKey
 */


#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#endif

#include <stdlib.h>
#include <utilfuns.h>
#include <rawstr.h>
#include <sysdata.h>

SWORD_NAMESPACE_START

/******************************************************************************
 * RawStr Statics
 */

int RawStr::instance = 0;
char RawStr::nl = '\n';


/******************************************************************************
 * RawStr Constructor - Initializes data for instance of RawStr
 *
 * ENT:	ipath - path of the directory where data and index files are located.
 *		be sure to include the trailing separator (e.g. '/' or '\')
 *		(e.g. 'modules/texts/rawtext/webster/')
 */

RawStr::RawStr(const char *ipath, int fileMode)
{
	SWBuf buf;

	lastoff = -1;
	path = 0;
	stdstr(&path, ipath);

#ifndef O_BINARY		// O_BINARY is needed in Borland C++ 4.53
#define O_BINARY 0		// If it hasn't been defined than we probably
#endif				// don't need it.

	if (fileMode == -1) { // try read/write if possible
		fileMode = O_RDWR;
	}
		
	buf.setFormatted("%s.idx", path);
	idxfd = FileMgr::getSystemFileMgr()->open(buf, fileMode|O_BINARY, true);

	buf.setFormatted("%s.dat", path);
	datfd = FileMgr::getSystemFileMgr()->open(buf, fileMode|O_BINARY, true);

	if (datfd < 0) {
		buf.setFormatted("Error: %d", errno);
		perror(buf);
	}

	instance++;
}


/******************************************************************************
 * RawStr Destructor - Cleans up instance of RawStr
 */

RawStr::~RawStr()
{
	if (path)
		delete [] path;

	--instance;

	FileMgr::getSystemFileMgr()->close(idxfd);
	FileMgr::getSystemFileMgr()->close(datfd);
}


/******************************************************************************
 * RawStr::getidxbufdat	- Gets the index string at the given idx offset
 *						NOTE: buf is allocated and must be freed by
 *							calling function
 *
 * ENT:	ioffset	- offset in dat file to lookup
 *		buf		- address of pointer to allocate for storage of string
 */

void RawStr::getIDXBufDat(long ioffset, char **buf)
{
	int size;
	char ch;
	if (datfd > 0) {
		lseek(datfd->getFd(), ioffset, SEEK_SET);
		for (size = 0; read(datfd->getFd(), &ch, 1) == 1; size++) {
			if ((ch == '\\') || (ch == 10) || (ch == 13))
				break;
		}
		*buf = (*buf) ? (char *)realloc(*buf, size*2 + 1) : (char *)malloc(size*2 + 1);
		if (size) {
			lseek(datfd->getFd(), ioffset, SEEK_SET);
			read(datfd->getFd(), *buf, size);
		}
		(*buf)[size] = 0;
		toupperstr_utf8(*buf, size*2);
	}
	else {
		*buf = (*buf) ? (char *)realloc(*buf, 1) : (char *)malloc(1);
		**buf = 0;
	}
}


/******************************************************************************
 * RawStr::getidxbuf	- Gets the index string at the given idx offset
 *						NOTE: buf is allocated and must be freed by
 *							calling function
 *
 * ENT:	ioffset	- offset in idx file to lookup
 *		buf		- address of pointer to allocate for storage of string
 */

void RawStr::getIDXBuf(long ioffset, char **buf)
{
	char *trybuf, *targetbuf;
	long offset;
	
	if (idxfd > 0) {
		lseek(idxfd->getFd(), ioffset, SEEK_SET);
		read(idxfd->getFd(), &offset, 4);

		offset = swordtoarch32(offset);

		getIDXBufDat(offset, buf);
	}
}


/******************************************************************************
 * RawStr::findoffset	- Finds the offset of the key string from the indexes
 *
 * ENT:	key		- key string to lookup
 *		start	- address to store the starting offset
 *		size		- address to store the size of the entry
 *		away		- number of entries before of after to jump
 *					(default = 0)
 *
 * RET: error status -1 general error; -2 new file
 */

signed char RawStr::findOffset(const char *ikey, long *start, unsigned short *size, long away, long *idxoff)
{
	char *trybuf, *targetbuf, *key = 0, quitflag = 0;
	signed char retval = -1;
	long headoff, tailoff, tryoff = 0, maxoff = 0;

	if (idxfd->getFd() >=0) {
		tailoff = maxoff = lseek(idxfd->getFd(), 0, SEEK_END) - 6;
		retval = (tailoff >= 0) ? 0 : -2;	// if NOT new file
		if (*ikey) {
			headoff = 0;

			stdstr(&key, ikey, 3);
			toupperstr_utf8(key, strlen(key)*3);

			trybuf = 0;

			while (headoff < tailoff) {
				tryoff = (lastoff == -1) ? headoff + ((((tailoff / 6) - (headoff / 6))) / 2) * 6 : lastoff; 
				lastoff = -1;
				getIDXBuf(tryoff, &trybuf);

				if (!*trybuf && tryoff) {		// In case of extra entry at end of idx (not first entry)
					tryoff += (tryoff > (maxoff / 2))?-6:6;
					retval = -1;
					break;
				}
					
				if (!strcmp(key, trybuf))
					break;

				int diff = strcmp(key, trybuf);
				if (diff < 0)
					tailoff = (tryoff == headoff) ? headoff : tryoff;
				else headoff = tryoff;
				if (tailoff == headoff + 6) {
					if (quitflag++)
						headoff = tailoff;
				}
			}
			if (headoff >= tailoff)
				tryoff = headoff;
			if (trybuf)
				free(trybuf);
			delete [] key;
		}
		else	tryoff = 0;

		lseek(idxfd->getFd(), tryoff, SEEK_SET);

		*start = *size = 0;
		read(idxfd->getFd(), start, 4);
		read(idxfd->getFd(), size, 2);
		if (idxoff)
			*idxoff = tryoff;

		*start = swordtoarch32(*start);
		*size  = swordtoarch16(*size);

		while (away) {
			long laststart = *start;
			unsigned short lastsize = *size;
			long lasttry = tryoff;
			tryoff += (away > 0) ? 6 : -6;
		
			bool bad = false;
			if (((tryoff + (away*6)) < -6) || (tryoff + (away*6) > (maxoff+6)))
				bad = true;
			else if (lseek(idxfd->getFd(), tryoff, SEEK_SET) < 0)
				bad = true;
			if (bad) {
				retval = -1;
				*start = laststart;
				*size = lastsize;
				tryoff = lasttry;
				if (idxoff)
					*idxoff = tryoff;
				break;
			}
			read(idxfd->getFd(), start, 4);
			read(idxfd->getFd(), size, 2);
			if (idxoff)
				*idxoff = tryoff;

			*start = swordtoarch32(*start);
			*size  = swordtoarch16(*size);

			if (((laststart != *start) || (lastsize != *size)) && (*start >= 0) && (*size)) 
				away += (away < 0) ? 1 : -1;
		}
	
		lastoff = tryoff;
	}
	else {
		*start = 0;
		*size  = 0;
		if (idxoff)
			*idxoff = 0;
		retval = -1;
	}
	return retval;
}


/******************************************************************************
 * RawStr::preptext	- Prepares the text before returning it to external
 *					objects
 *
 * ENT:	buf	- buffer where text is stored and where to store the prep'd
 *				text.
 */

void RawStr::prepText(SWBuf &buf) {
	unsigned int to, from; 
	char space = 0, cr = 0, realdata = 0, nlcnt = 0;
	char *rawBuf = buf.getRawData();
	for (to = from = 0; rawBuf[from]; from++) {
		switch (rawBuf[from]) {
		case 10:
			if (!realdata)
				continue;
			space = (cr) ? 0 : 1;
			cr = 0;
			nlcnt++;
			if (nlcnt > 1) {
//				*to++ = nl;
				rawBuf[to++] = 10;
//				*to++ = nl[1];
//				nlcnt = 0;
			}
			continue;
		case 13:
			if (!realdata)
				continue;
//			*to++ = nl[0];
			rawBuf[to++] = 10;
			space = 0;
			cr = 1;
			continue;
		}
		realdata = 1;
		nlcnt = 0;
		if (space) {
			space = 0;
			if (rawBuf[from] != ' ') {
				rawBuf[to++] = ' ';
				from--;
				continue;
			}
		}
		rawBuf[to++] = rawBuf[from];
	}
	buf.setSize(to);

	while (to > 1) {			// remove trailing excess
		to--;
		if ((rawBuf[to] == 10) || (rawBuf[to] == ' '))
			buf.setSize(to);
		else break;
	}
}


/******************************************************************************
 * RawStr::readtext	- gets text at a given offset
 *
 * ENT:
 *	start	- starting offset where the text is located in the file
 *	size		- size of text entry
 *	buf		- buffer to store text
 *
 */

void RawStr::readText(long istart, unsigned short *isize, char **idxbuf, SWBuf &buf)
{
	unsigned int ch;
	char *idxbuflocal = 0;
	getIDXBufDat(istart, &idxbuflocal);
	long start = istart;

	do {
		if (*idxbuf)
			delete [] *idxbuf;
		buf = "";
		buf.setFillByte(0);
		buf.setSize(++(*isize));

		*idxbuf = new char [ (*isize) ];

		lseek(datfd->getFd(), start, SEEK_SET);
		read(datfd->getFd(), buf.getRawData(), (int)((*isize) - 1));

		for (ch = 0; buf[ch]; ch++) {		// skip over index string
			if (buf[ch] == 10) {
				ch++;
				break;
			}
		}
		buf = SWBuf(buf.c_str()+ch);
		// resolve link
		if (!strncmp(buf.c_str(), "@LINK", 5)) {
			for (ch = 0; buf[ch]; ch++) {		// null before nl
				if (buf[ch] == 10) {
					buf[ch] = 0;
					break;
				}
			}
			findOffset(buf.c_str() + 6, &start, isize);
		}
		else break;
	}
	while (true);	// while we're resolving links

	if (idxbuflocal) {
		int localsize = strlen(idxbuflocal);
		localsize = (localsize < (*isize - 1)) ? localsize : (*isize - 1);
		strncpy(*idxbuf, idxbuflocal, localsize);
		(*idxbuf)[localsize] = 0;
		free(idxbuflocal);
	}
}


/******************************************************************************
 * RawLD::settext	- Sets text for current offset
 *
 * ENT: key	- key for this entry
 *	buf	- buffer to store
 *      len     - length of buffer (0 - null terminated)
 */

void RawStr::doSetText(const char *ikey, const char *buf, long len)
{

	long start, outstart;
	long idxoff;
	long endoff;
	long shiftSize;
	unsigned short size;
	unsigned short outsize;
	static const char nl[] = {13, 10};
	char *tmpbuf = 0;
	char *key = 0;
	char *dbKey = 0;
	char *idxBytes = 0;
	char *outbuf = 0;
	char *ch = 0;

	char errorStatus = findOffset(ikey, &start, &size, 0, &idxoff);
	stdstr(&key, ikey, 2);
	toupperstr_utf8(key, strlen(key)*2);

	len = (len < 0) ? strlen(buf) : len;

	getIDXBufDat(start, &dbKey);

	if (strcmp(key, dbKey) < 0) {
	}
	else if (strcmp(key, dbKey) > 0) {
		if (errorStatus != (char)-2)	// not a new file
			idxoff += 6;
		else idxoff = 0;
	}
	else if ((!strcmp(key, dbKey)) && (len>0 /*we're not deleting*/)) { // got absolute entry
		do {
			tmpbuf = new char [ size + 2 ];
			memset(tmpbuf, 0, size + 2);
			lseek(datfd->getFd(), start, SEEK_SET);
			read(datfd->getFd(), tmpbuf, (int)(size - 1));

			for (ch = tmpbuf; *ch; ch++) {		// skip over index string
				if (*ch == 10) {
					ch++;
					break;
				}
			}
			memmove(tmpbuf, ch, size - (unsigned short)(ch-tmpbuf));

			// resolve link
			if (!strncmp(tmpbuf, "@LINK", 5) && (len)) {
				for (ch = tmpbuf; *ch; ch++) {		// null before nl
					if (*ch == 10) {
						*ch = 0;
						break;
					}
				}
				findOffset(tmpbuf + 6, &start, &size, 0, &idxoff);
			}
			else break;
		}
		while (true);	// while we're resolving links
	}

	endoff = lseek(idxfd->getFd(), 0, SEEK_END);

	shiftSize = endoff - idxoff;

	if (shiftSize > 0) {
		idxBytes = new char [ shiftSize ];
		lseek(idxfd->getFd(), idxoff, SEEK_SET);
		read(idxfd->getFd(), idxBytes, shiftSize);
	}

	outbuf = new char [ len + strlen(key) + 5 ];
	sprintf(outbuf, "%s%c%c", key, 13, 10);
	size = strlen(outbuf);
	memcpy(outbuf + size, buf, len);
	size = outsize = size + (len);

	start = outstart = lseek(datfd->getFd(), 0, SEEK_END);

	outstart = archtosword32(start);
	outsize  = archtosword16(size);

	lseek(idxfd->getFd(), idxoff, SEEK_SET);
	if (len > 0) {
		lseek(datfd->getFd(), start, SEEK_SET);
		write(datfd->getFd(), outbuf, (int)size);

		// add a new line to make data file easier to read in an editor
		write(datfd->getFd(), &nl, 2);
		
		write(idxfd->getFd(), &outstart, 4);
		write(idxfd->getFd(), &outsize, 2);
		if (idxBytes) {
			write(idxfd->getFd(), idxBytes, shiftSize);
			delete [] idxBytes;
		}
	}
	else {	// delete entry
		if (idxBytes) {
			write(idxfd->getFd(), idxBytes+6, shiftSize-6);
			lseek(idxfd->getFd(), -1, SEEK_CUR);	// last valid byte
			FileMgr::getSystemFileMgr()->trunc(idxfd);	// truncate index
			delete [] idxBytes;
		}
	}

	delete [] key;
	delete [] outbuf;
	free(dbKey);
}


/******************************************************************************
 * RawLD::linkentry	- links one entry to another
 *
 * ENT: testmt	- testament to find (0 - Bible/module introduction)
 *	destidxoff	- dest offset into .vss
 *	srcidxoff		- source offset into .vss
 */

void RawStr::doLinkEntry(const char *destkey, const char *srckey) {
	char *text = new char [ strlen(destkey) + 7 ];
	sprintf(text, "@LINK %s", destkey);
	doSetText(srckey, text);
	delete [] text;
}


/******************************************************************************
 * RawLD::CreateModule	- Creates new module files
 *
 * ENT: path	- directory to store module files
 * RET: error status
 */

signed char RawStr::createModule(const char *ipath)
{
	char *path = 0;
	char *buf = new char [ strlen (ipath) + 20 ];
	FileDesc *fd, *fd2;

	stdstr(&path, ipath);

	if ((path[strlen(path)-1] == '/') || (path[strlen(path)-1] == '\\'))
		path[strlen(path)-1] = 0;

	sprintf(buf, "%s.dat", path);
	FileMgr::removeFile(buf);
	fd = FileMgr::getSystemFileMgr()->open(buf, O_CREAT|O_WRONLY|O_BINARY, S_IREAD|S_IWRITE);
	fd->getFd();
	FileMgr::getSystemFileMgr()->close(fd);

	sprintf(buf, "%s.idx", path);
	FileMgr::removeFile(buf);
	fd2 = FileMgr::getSystemFileMgr()->open(buf, O_CREAT|O_WRONLY|O_BINARY, S_IREAD|S_IWRITE);
	fd2->getFd();
	FileMgr::getSystemFileMgr()->close(fd2);

	delete [] path;
	
	return 0;
}

SWORD_NAMESPACE_END
