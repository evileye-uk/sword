#ifdef _ICU_

#include <iostream>
#include <string>

#include "unicode/utypes.h"   /* Basic ICU data types */
#include "unicode/ucnv.h"     /* C   Converter API    */
#include "unicode/convert.h"  /* C++ Converter API    */
#include "unicode/ustring.h"  /* some more string fcns*/

#include "unicode/translit.h"

int main() {

  UChar * uBuf;
  UChar * target;
  UConverter *conv;
  UErrorCode status = U_ZERO_ERROR;
  int32_t uBufSize = 0, uLength = 0;
  
  char * samplestring = "If this compiles and runs without errors, apparently ICU is working.";

  uLength = strlen(samplestring);
  conv = ucnv_open("utf-8", &status);		
  uBufSize = (uLength/ucnv_getMinCharSize(conv));
  uBuf = (UChar*)malloc(uBufSize * sizeof(UChar));
  
  target = uBuf;
  
  ucnv_toUChars(conv, target, uLength, 
		samplestring, uLength, &status);

  cout << samplestring << endl;

  return 0;
}
#endif
