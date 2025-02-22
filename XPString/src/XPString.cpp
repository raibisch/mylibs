/*
  PString.cpp - Lightweight printable string class
  Copyright (c) 2009-2012 Mikal Hart.  All right reserved.

  XPString.cpp (modifications)
  Copyright (c) 2016 Federico Tedin

  XPString.cpp (modification and extended functions)
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

#include "XPString.h"

// by JG
/*
char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}
*/


#ifdef DEF_XPARTS
bool XParts::has_next()
{
    return _pch != NULL;
}

char *XParts::get_next()
{
    char *tmp = _pch;
    _pch = strtok_r(NULL, _dels, &_save);
    return tmp;
}

XParts XPString::split(const char *dels)
{
    return XParts(_buf, dels);
}
#endif

// by JG
void XPString::setBuffer(char *buffer, size_t size) 
{
  if ((_weOwn) &&(_buf))
	free(_buf);
  
  _buf=buffer; 
  _size=size; 
  if ((_buf==NULL) && (_size > 0))
  {
	// allocating our own buffer;
	_weOwn = true;
	_buf = (char *)calloc(_size + 1, sizeof(char));
  }
  reset();
} 


XPString XPString::from_c_str(char *buf, size_t buf_len)
{
    size_t len = strnlen(buf, buf_len - 1);
    XPString xps;
    xps._buf = buf;
    xps._cur = &buf[len];
    xps._size = buf_len;
    *xps._cur = '\0';

    return xps;
}

void XPString::append(XPString other)
{
    const char *other_buf = other.c_str();
    print(other_buf);
}

void XPString::reset()
{
    _cur = _buf;
    if (_size > 0)
    {
        _buf[0] = '\0';
    }
}

size_t XPString::write(uint8_t b)
{
    if (_cur + 1 < _buf + _size)
    {
        *_cur++ = (char)b;
        *_cur = '\0';
		return 1;
	}

	return 0;
}

int XPString::format(char *str, ...)
{
    va_list argptr;
    va_start(argptr, str);
    int ret = vsnprintf(_cur, _size - (_cur - _buf), str, argptr);
    if (_size)
    {
        while (*_cur)
        {
            ++_cur;
        }
    }
    return ret;
}

// by JG
/*
int XPString::format(const __FlashStringHelper *str, ...) 
{ 
  va_list argptr;  
  va_start(argptr, str); 
  const prog_char *p = (const prog_char *)str;
  int ret = vsnprintf_P((char *)_cur, _size - (_cur - _buf), p, argptr);
  va_end(argptr);
  if (_size)
     while (*_cur) 
        ++_cur;
  return ret;
}
*/

// by JG
/**
int XPString::format_P(PGM_P str, ...) 
{ 
  va_list argptr;  
  va_start(argptr, str); 
  int ret = vsnprintf_P((char *)_cur, _size - (_cur - _buf), str, argptr);
  va_end(argptr);
  if (_size)
     while (*_cur) 
        ++_cur;
  return ret;
}

size_t XPString::print_P(PGM_P str)
{
  while (_buf[*_cur++] = pgm_read_byte(str++)) 
    if (*_cur == _size)
	  break;
  _buf[*_cur++] ='\0';
  return 0;
}
*/



bool XPString::starts_with(const char *str)
{
    size_t len = strnlen(str, _size - 1);
    if (len == 0)
    {
        return true;
    }
    else if (len > length())
    {
        return false;
    }

    int comp = strncmp(_buf, str, len);
    return comp == 0;
}

bool XPString::ends_with(const char *str)
{
    size_t other_len = strnlen(str, _size - 1);
    size_t self_len = length();
    if (other_len == 0)
    {
        return true;
    }
    else if (other_len > self_len)
    {
        return false;
    }

    size_t offset = self_len - other_len;
    int comp = strncmp(&_buf[offset], str, other_len);
    return comp == 0;
}

int XPString::index_of(const char *str)
{
    size_t len = strnlen(str, _size - 1);
    if (len > length())
    {
        return -1;
    }

    char *index = strstr(_buf, str);
    if (index == NULL) {
        return -1;
    }

    return (int)(index - _buf);
}


int XPString::index_of(const char *str, int start)
{
    size_t len = strnlen(str, _size - 1);
    if (len > length())
    {
        return -2;
    }

    if (start > length())
    {
        return -3;
    }

    //const char* mybuf = _buf;
    char *index = strstr(&_buf[start], str);
    //const char * buf2 = _buf+start;
    if (index == NULL) 
    {
        return -1;
    }

    //unsigned xx = (int) ((index) - (mybuf));
    return ((int) ((index) - (_buf)));
}


/*
void XPString::reverse()
{
    strrev(_buf);
}
*/

void XPString::remove(unsigned index)
{
    remove(index, length() - index);
}

void XPString::remove(unsigned index, unsigned count)
{
    size_t len = length();
    if (index >= len || count == 0 || len == 0) {
        return;
    }

    if (count > len - index) {
        count = len - index;
    }

    char *start = &_buf[index];
    char *end = &_buf[index + count];
    unsigned i;
    for (i = 0; i < len - (index + count); i++) {
        start[i] = end[i];
    }
    start[i] = '\0';
    _cur = &start[i];
}




/* alt:
void XPString::substring(char *str, unsigned index, unsigned len)
{
    strncpy(str,_buf+index,len);
    str[len] = '\0';
}
*/


/// @brief copy substring by JG 
/// @param str 
/// @param index 
/// @param len 
bool XPString::substring(XPString xdest, unsigned index, unsigned len)
{
    char* str = (char*) xdest.c_str();
    if (xdest.capacity() > len)
    {
     strncpy(str,_buf+index,len);
     str[len] = '\0';
     return true;
    }
    return false;
}

bool XPString::substringEndwith(XPString xdest, unsigned beginFind, const char* sFind, int offsetEnd)
{
    int pos2 = index_of(sFind, beginFind);
    if ((pos2-offsetEnd) > beginFind)
    {
      substring(xdest, beginFind, pos2-beginFind-offsetEnd);
      return true;
    }
    return false;
}

bool XPString::substringBeetween(XPString xdest, int startSearch, const char* sBegin, int offsetBegin, const char* sEnd, int offsetEnd)
{
  int pos1,pos2 = -1;
  pos1 = index_of(sBegin, startSearch) + offsetBegin;
  pos2 = index_of(sEnd, pos1) + offsetEnd; // erst nach sBegin aus suchen !
  if (pos2 > pos1)
  {
    substring(xdest, pos1, pos2-pos1);
    return true;
  }
  return false;
}


bool XPString::substringBeetween(XPString xdest, const char* sBegin, int offsetBegin, const char* sEnd, int offsetEnd)
{
  int pos1,pos2 = -1;
  pos1 = index_of(sBegin) + offsetBegin;
  pos2 = index_of(sEnd, pos1) + offsetEnd; // erst nach sBegin aus suchen !
  //pos2 = index_of(sEnd) + offsetEnd;
  if (pos2 > pos1)
  {
    substring(xdest, pos1, pos2-pos1);
    return true;
  }

  return false;
}


 /*
 get value from

XPString:
 <name>Vorlauf</name><value>20.2°C</value></item>

myStringvalue.reset();
myString.xmlvaluebyname("Vorlauf", myStringvalue)
  
myStringvalue:
 --> return "20.2°C"

 */
 bool XPString::xmlvaluebyname(const char* name, size_t namelen, XPString xdest)
 {

    int pos1 = index_of(name);

    if (pos1 > 0)
    {
        pos1 = pos1 + namelen +13;
    }
    else
    {
        return -1;
    }
    
    
    const char* sValue = "</value>";
    char *index2 = strstr(_buf+pos1, sValue);
    if (index2 == NULL) {
        return -1;
    }
    int pos2 = (int) (index2 - _buf);
    if (pos2 > pos1)
    {
        return substring(xdest, pos1, pos2-pos1);
    }
    
    return false;
 }

 
 bool XPString::xmlvaluebyname(const char* name, size_t namelen, const char* endpattern, XPString xdest)
 {

    int pos1 = index_of(name);

    if (pos1 > 0)
    {
        pos1 = pos1 + namelen +13;
    }
    else
    {
        return -1;
    }
    
    
    //const char* sValue = "</value>";
    char *index2 = strstr(_buf+pos1, endpattern);
    if (index2 == NULL) {
        return -1;
    }
    int pos2 = (int) (index2 - _buf);
    if (pos2 > pos1)
    {
        return substring(xdest, pos1, pos2-pos1);
    }
    
    return false;
 }

 


