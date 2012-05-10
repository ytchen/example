#include <iostream>
#include <string.h>
#include <ctype.h>
#include "String.h"

using namespace std;

/*String_cell::~String_cell(void)
{
 assert(!refs);
 delete [] s;
}*/
/*

String::~String()
{
  if (--(guts->refs) == 0) {
    delete guts;
  }
}

String::String(int sz)
{ 
  guts = new String_cell(sz); 
}

String::String(const String& st)
{ 
  guts = st.guts; 
  st.guts->refs += 1; 
}

String::String(const char* st)
{
  int newlen = strlen(st);
  guts = new String_cell(newlen);
  guts->len = newlen;
  strcpy(guts->s, st);
}



String String::operator+(const String& y) const
{
  int newlen = guts->len + y.guts->len;
  String z(newlen);
  strcpy(z.guts->s, guts->s);
  strcat(z.guts->s, y.guts->s);
  z.guts->len = newlen;
  return z;
}

void String::operator+=(const String& y)
{
  int newlen = guts->len + y.guts->len;
  clone(newlen);
  strcat(guts->s, y.guts->s);
  guts->len = newlen;
}

void String::operator+=(char c)
{
  clone(guts->len+1);
  guts->s[guts->len] = c;
  guts->len += 1;
  guts->s[guts->len] = '\0';
}
          
  
String& String::operator=(const char* st)
{
  int newlen = strlen(st);

  if (guts->refs > 1 || guts->sz < newlen) {
    guts->refs -= 1;
    if (guts->refs == 0) delete guts;
    guts = new String_cell(newlen);
  }

  assert(guts->refs == 1);
  strcpy(guts->s, st);
  guts->len = newlen;

  return *this;
}

*/
    
/* the subString before pos (exclusive) */
/*
String String::sub_upto(int pos) const
{
  if (pos > guts->len)
    pos = guts->len+1;
  
  String res(pos-1);
  strncpy(res.guts->s, guts->s, pos-1);
  res.guts->len = pos-1;

  return res;
}
*/
/* the subString up to and including pos */
/*
String String::sub_upto_inc(int pos) const
{ 
  if (pos > guts->len)
    pos = guts->len;
  String res(pos);
  strncpy(res.guts->s, guts->s, pos);

  res.guts->len = pos;
  res.guts->s[pos] = '\0';

  return res;
}
*/
/* the subString after pos */
/*
String String::sub_after(int pos) const 
{
  
  if (pos >= guts->len) {
    String res;
    return res;
  }

  int newlen = guts->len - pos;
  
  String res(newlen);
  strcpy(res.guts->s, guts->s + pos);
  res.guts->len = newlen;
  return res;
}
*/

/* the subString after pos (inclusive) */
/*
String String::sub_after_inc(int pos) const
{
  if (pos > guts->len) {
    String res;
    return res;
  }

  int newlen = guts->len - pos + 1;

  String res(newlen);
  strcpy(res.guts->s, guts->s + pos-1);
  res.guts->len = newlen;
  return res;
}


bool String::operator==(const String& s2) const
{
  return 0 == strcmp(guts->s, s2.guts->s);
}


bool String::operator<=(const String& s) const
{
  return strcmp((const char*) *this, (const char*) s) <= 0;
}

istream& operator>>(istream& ist, String& str)
{
  str = "";                     // reinitialize to NULL

  char t;
  ist >> t;                     // first char must be letter
  if (!ist) return ist;
  if (!isalpha(t)) {
    ist.putback(t);
    return ist;
  }
  str += t;
*/
  /* read additional characters */

/*
  ist >> t;
  while (ist && (isalnum(t) || t == '_')) {
    str += t;
    ist >> t;
  }
  if (ist)                      // fix up the istream
    ist.putback(t);
  else
    ist.clear();
  return ist;
}

*/