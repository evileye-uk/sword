/******************************************************************************
 *
 * $Id: gbfhtmlhref.h,v 1.6 2003/08/07 23:23:24 chrislit Exp $
 *
 * Copyright 1998 CrossWire Bible Society (http://www.crosswire.org)
 *	CrossWire Bible Society
 *	P. O. Box 2528
 *	Tempe, AZ  85280-2528
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#ifndef GBFHTMLHREF_H
#define GBFHTMLHREF_H

#include <swbasicfilter.h>

SWORD_NAMESPACE_START

/** this filter converts GBF  text to HTML text with hrefs
 */
class SWDLLEXPORT GBFHTMLHREF : public SWBasicFilter {
protected:
	class MyUserData : public SWFilterUserData {
	public:
		MyUserData(const SWModule *module, const SWKey *key) : SWFilterUserData(module, key) {}
		bool hasFootnotePreTag;
	};
	virtual SWFilterUserData *createUserData(const SWModule *module, const SWKey *key) {
		return new MyUserData(module, key);
	}
	virtual bool handleToken(SWBuf &buf, const char *token, SWFilterUserData *userData);
public:
	GBFHTMLHREF();
};

SWORD_NAMESPACE_END
#endif
