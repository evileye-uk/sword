/******************************************************************************
 *
 *  markupfiltmgr.cpp -	implementaion of class MarkupFilterMgr, subclass of
 *			used to transcode all module text to a requested
 *			markup
 *
 * $Id: markupfiltmgr.cpp 3075 2014-03-05 02:27:33Z chrislit $
 *
 * Copyright 2001-2013 CrossWire Bible Society (http://www.crosswire.org)
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

#include <thmlplain.h>
#include <gbfplain.h>
#include <osisplain.h>
#include <teiplain.h>
#include <thmlgbf.h>
#include <gbfthml.h>
#include <thmlhtml.h>
#include <gbfhtml.h>
#include <thmlhtmlhref.h>
#include <gbfhtmlhref.h>
#include <teihtmlhref.h>
#include <thmlrtf.h>
#include <gbfrtf.h>
#include <gbfosis.h>
#include <thmlosis.h>
#include <osisrtf.h>
#include <osislatex.h>
#include <teirtf.h>
#include <osisosis.h>
#include <osishtmlhref.h>
#include <gbfwebif.h>
#include <thmlwebif.h>
#include <osiswebif.h>
#include <swmodule.h>
#include <thmlxhtml.h>
#include <gbfxhtml.h>
#include <osisxhtml.h>
#include <teixhtml.h>
#include <gbflatex.h>
#include <thmllatex.h>
#include <teilatex.h>

#include <teimarkdown.h>
#include <osismarkdown.h>
#include <gbfmarkdown.h>
#include <thmlmarkdown.h>
 
#include <markupfiltmgr.h>

#include <swmgr.h>


SWORD_NAMESPACE_START


/******************************************************************************
 * MarkupFilterMgr Constructor - initializes instance of MarkupFilterMgr
 *
 * ENT:
 *      enc - Encoding format to emit
 *      mark - Markup format to emit
 */

MarkupFilterMgr::MarkupFilterMgr(char mark, char enc)
		: EncodingFilterMgr(enc) {

	markup = mark;

	CreateFilters(markup);
}


/******************************************************************************
 * MarkupFilterMgr Destructor - Cleans up instance of MarkupFilterMgr
 */

MarkupFilterMgr::~MarkupFilterMgr() {
	delete fromthml;
	delete fromgbf;
	delete fromplain;
	delete fromosis;
	delete fromtei;
}


/******************************************************************************
 * MarkupFilterMgr::Markup	- sets/gets markup
 *
 * ENT:	mark	- new encoding or 0 to simply get the current markup
 *
 * RET: markup
 */
char MarkupFilterMgr::Markup(char mark) {
	if (mark && mark != markup) {
		markup = mark;
		ModMap::const_iterator module;

		SWFilter *oldplain = fromplain;
		SWFilter *oldthml  = fromthml;
		SWFilter *oldgbf   = fromgbf;
		SWFilter *oldosis  = fromosis;
		SWFilter *oldtei   = fromtei;

		CreateFilters(markup);

		for (module = getParentMgr()->Modules.begin(); module != getParentMgr()->Modules.end(); ++module) {
			switch (module->second->getMarkup()) {
			case FMT_THML:
				if (oldthml != fromthml) {
					if (oldthml) {
						if (!fromthml) {
							module->second->removeRenderFilter(oldthml);
						}
						else {
							module->second->replaceRenderFilter(oldthml, fromthml);
						}
					}
					else if (fromthml) {
						module->second->addRenderFilter(fromthml);
					}
				}
				break;

			case FMT_GBF:
				if (oldgbf != fromgbf) {
					if (oldgbf) {
						if (!fromgbf) {
							module->second->removeRenderFilter(oldgbf);
						}
						else {
							module->second->replaceRenderFilter(oldgbf, fromgbf);
						}
					}
					else if (fromgbf) {
						module->second->addRenderFilter(fromgbf);
					}
				}
				break;

			case FMT_PLAIN:
				if (oldplain != fromplain) {
					if (oldplain) {
						if (!fromplain) {
							module->second->removeRenderFilter(oldplain);
						}
						else {
							module->second->replaceRenderFilter(oldplain, fromplain);
						}
					}
					else if (fromplain) {
						module->second->addRenderFilter(fromplain);
					}
				}
				break;

			case FMT_OSIS:
				if (oldosis != fromosis) {
					if (oldosis) {
						if (!fromosis) {
							module->second->removeRenderFilter(oldosis);
						}
						else {
							module->second->replaceRenderFilter(oldosis, fromosis);
						}
					}
					else if (fromosis) {
						module->second->addRenderFilter(fromosis);
					}
				}
				break;

			case FMT_TEI:
				if (oldtei != fromtei) {
					if (oldtei) {
						if (!fromtei) {
							module->second->removeRenderFilter(oldtei);
						}
						else {
							module->second->replaceRenderFilter(oldtei, fromtei);
						}
					}
					else if (fromtei) {
						module->second->addRenderFilter(fromtei);
					}
				}
				break;
			}
		}

		delete oldthml;
		delete oldgbf;
		delete oldplain;
		delete oldosis;
		delete oldtei;
	}
	return markup;
}


void MarkupFilterMgr::AddRenderFilters(SWModule *module, ConfigEntMap &section) {
	switch (module->getMarkup()) {
	case FMT_THML:
		if (fromthml)
			module->addRenderFilter(fromthml);
		break;
	case FMT_GBF:
		if (fromgbf)
			module->addRenderFilter(fromgbf);
		break;
	case FMT_PLAIN:
		if (fromplain)
			module->addRenderFilter(fromplain);
		break;
	case FMT_OSIS:
		if (fromosis)
			module->addRenderFilter(fromosis);
		break;
	case FMT_TEI:
		if (fromtei)
			module->addRenderFilter(fromtei);
		break;
	}
}


void MarkupFilterMgr::CreateFilters(char markup) {

	switch (markup) {
	case FMT_PLAIN:
		fromplain = NULL;
		fromthml  = new ThMLPlain();
		fromgbf   = new GBFPlain();
		fromosis  = new OSISPlain();
		fromtei   = new TEIPlain();
		break;

	case FMT_THML:
		fromplain = NULL;
		fromthml  = NULL;
		fromgbf   = new GBFThML();
		fromosis  = NULL;
		fromtei   = NULL;
		break;

	case FMT_GBF:
		fromplain = NULL;
		fromthml  = new ThMLGBF();
		fromgbf   = NULL;
		fromosis  = NULL;
		fromtei   = NULL;
		break;

	case FMT_HTML:
		fromplain = NULL;
		fromthml  = new ThMLHTML();
		fromgbf   = new GBFHTML();
		fromosis  = NULL;
		fromtei   = NULL;
		break;

	case FMT_HTMLHREF:
		fromplain = NULL;
		fromthml  = new ThMLHTMLHREF();
		fromgbf   = new GBFHTMLHREF();
		fromosis  = new OSISHTMLHREF();
		fromtei   = new TEIHTMLHREF();
		break;

	case FMT_RTF:
		fromplain = NULL;
		fromthml  = new ThMLRTF();
		fromgbf   = new GBFRTF();
		fromosis  = new OSISRTF();
		fromtei   = new TEIRTF();
		break;

	case FMT_LATEX:
		fromplain = NULL;
		fromthml  = new ThMLLaTeX();
		fromgbf   = new GBFLaTeX();
		fromosis  = new OSISLaTeX();
		fromtei   = new TEILaTeX();
		break;

	case FMT_MARKDOWN:
		fromplain = NULL;
		fromthml  = new ThMLMarkDown();
		fromgbf   = new GBFMarkDown();
		fromosis  = new OSISMarkDown();
		fromtei   = new TEIMarkDown();
		break;

	case FMT_OSIS:
		fromplain = NULL;
		fromthml  = new ThMLOSIS();
		fromgbf   = new GBFOSIS();
		fromosis  = new OSISOSIS();
		fromtei   = NULL;
		break;

	case FMT_WEBIF:
		fromplain = NULL;
		fromthml  = new ThMLWEBIF();
		fromgbf   = new GBFWEBIF();
		fromosis  = new OSISWEBIF();
		fromtei   = NULL;
		break;

	case FMT_TEI:
		fromplain = NULL;
		fromthml  = NULL;
		fromgbf   = NULL;
		fromosis  = NULL;
		fromtei   = NULL;
		break;

	case FMT_XHTML:
		fromplain = NULL;
		fromthml  = new ThMLXHTML();
		fromgbf   = new GBFXHTML();
		fromosis  = new OSISXHTML();
		fromtei   = new TEIXHTML();
		break;
	}

}


SWORD_NAMESPACE_END

