/***************************************************************************
                     thmlosis.cpp  -  ThML to OSIS filter
                             -------------------
    begin                : 1999-10-27
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
#include <stdio.h>
#include <string.h>
#include <thmlosis.h>
#include <versekey.h>


ThMLOSIS::ThMLOSIS() {

	setTokenStart("<");
	setTokenEnd(">");

	setEscapeStart("&");
	setEscapeEnd(";");

	setEscapeStringCaseSensitive(true);

	setTokenCaseSensitive(true);

}

bool ThMLOSIS::handleToken(char **buf, const char *token, DualStringMap &userData) {

	if (!substituteToken(buf, token)) {

		if (!strncmp(token, "scripRef", 8)) {
			pushString(buf, "<reference work=\"Bible.KJV\" reference=\"");
			userData["suspendTextPassThru"] = "true";
		}
		if (!strncmp(token, "/scripRef", 9)) {
			pushString(buf,
				convertToOSIS(userData["lastTextNode"].c_str())
			);
			pushString(buf, "\" />");
			userData["suspendTextPassThru"] = "false";
		}
//	addTokenSubstitute("/scripRef", "|}");
//	addTokenSubstitute("/note", ") }");

//        addTokenSubstitute("br", "\\line ");
//        addTokenSubstitute("br /", "\\line ");
//        addTokenSubstitute("i", "{\\i1 ");
//        addTokenSubstitute("/i", "}");
//        addTokenSubstitute("b", "{\\b1 ");
//        addTokenSubstitute("/b", "}");
//        addTokenSubstitute("p", "\\par ");
//
        //we need uppercase forms for the moment to support a few early ThML modules that aren't XHTML compliant
//        addTokenSubstitute("BR", "\\line ");
//        addTokenSubstitute("I", "{\\i1 ");
//        addTokenSubstitute("/I", "}");
//        addTokenSubstitute("B", "{\\b1 ");
//        addTokenSubstitute("/B", "}");
//        addTokenSubstitute("P", "\\par ");

	// manually process if it wasn't a simple substitution
		if (!strncmp(token, "sync type=\"Strongs\" value=\"", 27)) {
/*                        if (token[27] == 'H' || token[27] == 'G' || token[27] == 'A') {
        			pushString(buf, " {\\fs15 <");
                                for (unsigned int i = 28; token[i] != '\"'; i++)
                		        *(*buf)++ = token[i];
        			pushString(buf, ">}");
                        }
                        else if (token[27] == 'T') {
                                pushString(buf, " {\\fs15 (");
        			for (unsigned int i = 28; token[i] != '\"'; i++)
        				*(*buf)++ = token[i];
        			pushString(buf, ")}");
                        }
		}
		else if (!strncmp(token, "sync type=\"morph\" ", 18)) {
			pushString(buf, " {\\fs15 (");
			for (const char *tok = token + 5; *tok; tok++) {
				if (!strncmp(tok, "value=\"", 7)) {
					tok += 7;
					for (;*tok != '\"'; tok++)
						*(*buf)++ = *tok;
					break;
				}
			}

			pushString(buf, ")}");
*/		}
		else if (!strncmp(token, "sync type=\"lemma\" value=\"", 25)) {
			pushString(buf, "{\\fs15 (");
			for (unsigned int i = 25; token[i] != '\"'; i++)
				*(*buf)++ = token[i];
			pushString(buf, ")}");
		}
                else if (!strncmp(token, "div", 3)) {
                        *(*buf)++ = '{';
                        if (!strncmp(token, "div class=\"title\"", 17)) {
                                pushString(buf, "\\par\\i1\\b1 ");
                                userData["sechead"] = "true";
                        }
                        else if (!strncmp(token, "div class=\"sechead\"", 19)) {
                                pushString(buf, "\\par\\i1\\b1 ");
                                userData["sechead"] = "true";
                        }
                }
                else if (!strncmp(token, "/div", 4)) {
                        *(*buf)++ = '}';
                        if (userData["sechead"] == "true") {
                                pushString(buf, "\\par ");
                                userData["sechead"] == "false";
                        }
                }
                else if (!strncmp(token, "note", 4)) {
                        pushString(buf, " {\\i1\\fs15 (");
                }

		else {
			return false;  // we still didn't handle token
		}
	}
	return true;
}


char ThMLOSIS::ProcessText(char *text, int maxlen, const SWKey *key, const SWModule *module) {
	char retVal = SWBasicFilter::ProcessText(text, maxlen, key, module);
	VerseKey *vkey = SWDYNAMIC_CAST(VerseKey, key);
	if (vkey) {
		char ref[254];
		sprintf(ref, "<verseStart VerseStart=\"%s\" />", vkey->getOSISRef());
		memmove(text+strlen(ref), text, maxlen-strlen(ref)-1);
		memcpy(text, ref, strlen(ref));
		sprintf(ref, "<verseEnd refVerseStart=\"%s\" />", vkey->getOSISRef());
		strcat(text, ref);
	}
	return retVal;
}


const char *ThMLOSIS::convertToOSIS(const char *inRef) {
	static string outRef;

	outRef = "";

	VerseKey defLanguage;
	ListKey verses = defLanguage.ParseVerseList(inRef, (*key), true);
	for (int i = 0; i < verses.Count(); i++) {
		VerseKey *element = SWDYNAMIC_CAST(VerseKey, verses.GetElement(i));
		char buf[512];
		if (element) {
			sprintf(buf, "<reference work=\"Bible.KJV\" reference=\"%s\" referenceEnd=\"%s\" />", element->LowerBound().getOSISRef(), element->UpperBound().getOSISRef());
		}
		else sprintf(buf, "<reference work=\"Bible.KJV\" reference=\"%s\" />", VerseKey(*verses.GetElement(i)).getOSISRef());
		outRef+=buf;
	}
	return outRef.c_str();
}

