/***************************************************************************
                     thmlhtmlhref.cpp  -  ThML to HTML filter with hrefs  
                             -------------------
    begin                    : 2001-09-03
    copyright            : 2001 by CrossWire Bible Society
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <thmlhtmlhref.h>
#include <swmodule.h>

SWORD_NAMESPACE_START
 
ThMLHTMLHREF::ThMLHTMLHREF() {
	setTokenStart("<");
	setTokenEnd(">");
/*        
	setEscapeStart("&");
	setEscapeEnd(";");

	setEscapeStringCaseSensitive(true);

	addEscapeStringSubstitute("nbsp", " ");
	addEscapeStringSubstitute("quot", "\"");
	addEscapeStringSubstitute("amp", "&");
	addEscapeStringSubstitute("lt", "<");
	addEscapeStringSubstitute("gt", ">");
	addEscapeStringSubstitute("brvbar", "|");
	addEscapeStringSubstitute("sect", "�");
	addEscapeStringSubstitute("copy", "�");
	addEscapeStringSubstitute("laquo", "�");
	addEscapeStringSubstitute("reg", "�");
	addEscapeStringSubstitute("acute", "�");
	addEscapeStringSubstitute("para", "�");
	addEscapeStringSubstitute("raquo", "�");

	addEscapeStringSubstitute("Aacute", "�");
	addEscapeStringSubstitute("Agrave", "�");
	addEscapeStringSubstitute("Acirc", "�");
	addEscapeStringSubstitute("Auml", "�");
	addEscapeStringSubstitute("Atilde", "�");
	addEscapeStringSubstitute("Aring", "�");
	addEscapeStringSubstitute("aacute", "�");
	addEscapeStringSubstitute("agrave", "�");
	addEscapeStringSubstitute("acirc", "�");
	addEscapeStringSubstitute("auml", "�");
	addEscapeStringSubstitute("atilde", "�");
	addEscapeStringSubstitute("aring", "�");
	addEscapeStringSubstitute("Eacute", "�");
	addEscapeStringSubstitute("Egrave", "�");
	addEscapeStringSubstitute("Ecirc", "�");
	addEscapeStringSubstitute("Euml", "�");
	addEscapeStringSubstitute("eacute", "�");
	addEscapeStringSubstitute("egrave", "�");
	addEscapeStringSubstitute("ecirc", "�");
	addEscapeStringSubstitute("euml", "�");
	addEscapeStringSubstitute("Iacute", "�");
	addEscapeStringSubstitute("Igrave", "�");
	addEscapeStringSubstitute("Icirc", "�");
	addEscapeStringSubstitute("Iuml", "�");
	addEscapeStringSubstitute("iacute", "�");
	addEscapeStringSubstitute("igrave", "�");
	addEscapeStringSubstitute("icirc", "�");
	addEscapeStringSubstitute("iuml", "�");
	addEscapeStringSubstitute("Oacute", "�");
	addEscapeStringSubstitute("Ograve", "�");
	addEscapeStringSubstitute("Ocirc", "�");
	addEscapeStringSubstitute("Ouml", "�");
	addEscapeStringSubstitute("Otilde", "�");
	addEscapeStringSubstitute("oacute", "�");
	addEscapeStringSubstitute("ograve", "�");
	addEscapeStringSubstitute("ocirc", "�");
	addEscapeStringSubstitute("ouml", "�");
	addEscapeStringSubstitute("otilde", "�");
	addEscapeStringSubstitute("Uacute", "�");
	addEscapeStringSubstitute("Ugrave", "�");
	addEscapeStringSubstitute("Ucirc", "�");
	addEscapeStringSubstitute("Uuml", "�");
	addEscapeStringSubstitute("uacute", "�");
	addEscapeStringSubstitute("ugrave", "�");
	addEscapeStringSubstitute("ucirc", "�");
	addEscapeStringSubstitute("uuml", "�");
	addEscapeStringSubstitute("Yacute", "�");
	addEscapeStringSubstitute("yacute", "�");
	addEscapeStringSubstitute("yuml", "�");

	addEscapeStringSubstitute("deg", "�");
	addEscapeStringSubstitute("plusmn", "�");
	addEscapeStringSubstitute("sup2", "�");
	addEscapeStringSubstitute("sup3", "�");
	addEscapeStringSubstitute("sup1", "�");
	addEscapeStringSubstitute("nbsp", "�");
	addEscapeStringSubstitute("pound", "�");
	addEscapeStringSubstitute("cent", "�");
	addEscapeStringSubstitute("frac14", "�");
	addEscapeStringSubstitute("frac12", "�");
	addEscapeStringSubstitute("frac34", "�");
	addEscapeStringSubstitute("iquest", "�");
	addEscapeStringSubstitute("iexcl", "�");
	addEscapeStringSubstitute("ETH", "�");
	addEscapeStringSubstitute("eth", "�");
	addEscapeStringSubstitute("THORN", "�");
	addEscapeStringSubstitute("thorn", "�");
	addEscapeStringSubstitute("AElig", "�");
	addEscapeStringSubstitute("aelig", "�");
	addEscapeStringSubstitute("Oslash", "�");
	addEscapeStringSubstitute("curren", "�");
	addEscapeStringSubstitute("Ccedil", "�");
	addEscapeStringSubstitute("ccedil", "�");
	addEscapeStringSubstitute("szlig", "�");
	addEscapeStringSubstitute("Ntilde", "�");
	addEscapeStringSubstitute("ntilde", "�");
	addEscapeStringSubstitute("yen", "�");
	addEscapeStringSubstitute("not", "�");
	addEscapeStringSubstitute("ordf", "�");
	addEscapeStringSubstitute("uml", "�");
	addEscapeStringSubstitute("shy", "�");
	addEscapeStringSubstitute("macr", "�");
*/
	setTokenCaseSensitive(true);

	addTokenSubstitute("note", " <font color=\"#800000\"><small>(");
	addTokenSubstitute("/note", ")</small></font> ");
	addTokenSubstitute("/scripture", "</i> ");
}


bool ThMLHTMLHREF::handleToken(SWBuf &buf, const char *token, DualStringMap &userData) {
	const char *tok;
	if (!substituteToken(buf, token)) {
	// manually process if it wasn't a simple substitution
		if (!strncmp(token, "sync ", 5)) {
			if(strstr(token,"type=\"morph\"")){
				buf += "<small><em> (<a href=\"";
			}				
			else 
				buf += "<small><em> &lt;<a href=\"";
			for (tok = token + 5; *(tok+1); tok++)
				if(*tok != '\"')
					buf += *tok;
			buf += "\">";
				
                        //scan for value and add it to the buffer
			for (tok = token + 5; *tok; tok++) {
				if (!strncmp(tok, "value=\"", 7)) {
					if(strstr(token,"type=\"morph\"")) 
						tok += 7;
					else
						tok += 8;
					for (;*tok != '\"'; tok++)
						buf += *tok;
					break;
				}
			}
			if(strstr(token,"type=\"morph\"")) 
				buf += "</a>) </em></small>";
			else 				
				buf += "</a>&gt; </em></small>";
		}
		
		else if (!strncmp(token, "scripture ", 10)) {
			userData["inscriptRef"] = "true";
			buf += "<i>";
		} 

		else if (!strncmp(token, "scripRef p", 10) || !strncmp(token, "scripRef v", 10)) {
			userData["inscriptRef"] = "true";
			buf += "<a href=\"";
			for (const char *tok = token + 9; *(tok+1); tok++)				
				if(*tok != '\"') 			
					buf += *tok;
			buf += '\"';
			buf += '>';
		} 

		// we're starting a scripRef like "<scripRef>John 3:16</scripRef>"
		else if (!strcmp(token, "scripRef")) {
			userData["inscriptRef"] = "false";
			// let's stop text from going to output
			userData["suspendTextPassThru"] = "true";
		}

		// we've ended a scripRef 
		else if (!strcmp(token, "/scripRef")) {
			if (userData["inscriptRef"] == "true") { // like  "<scripRef passage="John 3:16">John 3:16</scripRef>"
				userData["inscriptRef"] = "false";
				buf +="</a>";
			}
			
			else { // like "<scripRef>John 3:16</scripRef>"
				buf += "<a href=\"passage=";
				//char *strbuf = (char *)userData["lastTextNode"].c_str();
				buf += userData["lastTextNode"].c_str();
				buf += '\"';
				buf += '>';
				buf += userData["lastTextNode"].c_str();
				// let's let text resume to output again
				userData["suspendTextPassThru"] = "false";	
				buf += "</a>";
			}
		}
			
		else if (!strncmp(token, "div class=\"sechead\"", 19)) {
		        userData["SecHead"] = "true";
			buf += "<br /><b><i>";
		}
		else if (!strncmp(token, "div class=\"title\"", 19)) {
		        userData["SecHead"] = "true";
			buf += "<br /><b><i>";
		}
		else if (!strncmp(token, "/div", 4)) {
		        if (userData["SecHead"] == "true") {
			        buf += "</i></b><br />";
				userData["SecHead"] = "false";
			}
		}
/*
		else if (!strncmp(token, "sync type=\"Strongs\" value=\"T", 28)) {
			buf +="<a href=\"");
			for (tok = token + 5; *(tok+1); tok++)				
				if(*tok != '\"') 			
					*(*buf)++ = *tok;
			*(*buf)++ = '\"';
			*(*buf)++ = '>';
			for (tok = token + 29; *(tok+2); tok++)				
				if(*tok != '\"') 			
					*(*buf)++ = *tok;		
			buf +="</a>");
		}
*/
		else if (!strncmp(token, "img ", 4)) {
			const char *src = strstr(token, "src");
			if (!src)		// assert we have a src attribute
				return false;

			buf += '<';
			for (const char *c = token; *c; c++) {
				if (c == src) {
					for (;((*c) && (*c != '"')); c++)
						buf += *c;

					if (!*c) { c--; continue; }

					buf += '"';
					if (*(c+1) == '/') {
						buf += "file:";
						buf += module->getConfigEntry("AbsoluteDataPath");
						if (buf[buf.length()-2] == '/')
							c++;		// skip '/'
					}
					continue;
				}
				buf += *c;
			}
			buf += '>';
		}
		else if (!strncmp(token, "note", 4)) {
			buf += " <small><font color=\"#800000\">(";
		}                
		else {
			buf += '<';
			for (const char *tok = token; *tok; tok++)
				buf += *tok;
			buf += '>';
			//return false;  // we still didn't handle token
		}
	}
	return true;
}


SWORD_NAMESPACE_END
