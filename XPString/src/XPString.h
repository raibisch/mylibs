/*
  PString.h - Lightweight printable string class
  Copyright (c) 2009-2012 Mikal Hart.  All right reserved.

  XPString.h (modifications)
  Copyright (c) 2016 Federico Tedin

  XPString.h (modification and extended functions)
  2025 by Jürgen Goldmann

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


/*
Beispiel für statischen String im Flashspeicher
const char home_page[] PROGMEM = R"=====(
 <html>
 
<head>
<title> ESP32 PAGE </title>
</head>
<body>
</body>
</html>
)=====";    end of array, no escape characters needed in the array

*/


#ifndef XPString_h
#define XPString_h

#include "Print.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// with XPARTS
#define DEF_XPARTS

#ifdef DEF_XPARTS
class XParts {
private:
    char *_pch;
    const char *_dels;
    char *_save;
public:
    XParts(char *buf, const char *dels) : _dels(dels), _save(NULL)
    {
        _pch = strtok_r(buf, _dels, &_save);
    }
    bool has_next();
    char *get_next();
};
#endif

class XPString : public Print
{
private:
    XPString() {}
    
    char *_buf, *_cur;
    size_t _size;
    bool _weOwn; // by JG

public:
    virtual size_t write(uint8_t);

  void setBuffer(char *buffer, size_t size);


    // Basic constructor requires a preallocated buffer
    XPString(char *buf, size_t size) : _buf(buf), _size(size)
    {
        reset();
    }

    /*
    idee wie man vielleicht size übergeben kann
    ===========================================
     https://solarianprogrammer.com/2016/11/28/cpp-passing-c-style-array-with-size-information-to-function/
     
     // The array is passed as a reference to the function, keeping the size information (only for C++)
     template<typename T, size_t N>
     void func(T (&a)[N]) 
     {
      // Here N stores the correct size information for the array
	  // Basically N is equal to sizeof(my_array)/sizeof(my_array[0]) so the next code will work as expected:
        // nur als Beispiel...:
      	for(int i = 0; i < N; ++i) 
        {
    		a[i] = a[i] + i;
    	}
     }

     Aufruf

12 int main() {
13 	// C-style array
14 	int my_array[5] = {1, 2, 3, 4, 5};
15 
16 	// Pass the array to a function
17 	func(my_array);
18 	return 0;
19 }

    */

    XPString from_c_str(char *buf, size_t buf_len);

    // returns the length of the current string, not counting the 0 terminator
    inline const size_t length()
    {
        return _cur - _buf;
    }

    // returns the capacity of the string
    inline const size_t capacity()
    {
        return _size;
    }

    // gives access to the internal string
    inline operator const char *()
    {
        return c_str();
    }

    inline const char *c_str()
    {
        return _buf;
    }

    

    // compare to another string
    bool operator==(const char *str)
    {
        return _size > 0 && !strcmp(_buf, str);
    }

    // call this to re-use an existing string
    void reset();

    // This function allows assignment to an arbitrary scalar value like str = myfloat;
    template<class T> inline XPString &operator =(T arg)
    {
        reset();
        print(arg);
        return *this;
    }

    // Concatenation str += myfloat;
    template<class T> inline XPString &operator +=(T arg)
    {
        print(arg);
        return *this;
    }

    // Safe access to sprintf-like formatting, e.g. str.format("Hi, my name is %s and I'm %d years old", name, age);
    // cast string with "(char*)" for const char*
    int format(char *str, ...);

    // by JG
    /** format - add support for __FlashStringHelper */
    //int format(const __FlashStringHelper *str, ...);

    // by JG
    /** format - add support for PGM_P (PSTR) strings */
    //int format_P(PGM_P str, ...);
  
    // by JG
    /** add support for PSTR and (const prog_char str[] PROGMEM) style strings. */
    //size_t print_P(PGM_P str);

    void append(XPString other);

    void remove(unsigned index);
    void remove(unsigned index, unsigned count);

    bool starts_with(const char *str);
    bool ends_with(const char *str);
    int index_of(const char *str);
    int index_of(const char *str, int start);
    //void reverse(); // brauch ich nicht
   
    bool substring(XPString xdest, unsigned index, unsigned len); // by JG
    bool substringEndwith(XPString xdest, unsigned sBegin, const char* sEnd, int offsetEnd=0);
    bool substringBeetween(XPString xdest, const char* sBegin, int offsetBegin, const char* sEnd, int offsetEnd); // by JG
    bool substringBeetween(XPString xdest, int startSearch, const char* sBegin, int offsetBegin, const char* sEnd, int offsetEnd);

    bool xmlvaluebyname(const char* key, size_t namelen, XPString xdest); // by JG
    bool xmlvaluebyname(const char* name,  size_t namelen, const char* endpattern, XPString xdest); // by JG value define endpattern

#ifdef DEF_XPARTS
    XParts split(const char *dels);
#endif
};

#endif /* XPString_h */
