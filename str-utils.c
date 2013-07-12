/*  Mcalc -- evaluate numerical expressions given in natural language.
    Copyright (C) 2005  M E Leypold.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
    02110-1301 USA.
*/

/*  str-utils.c: Implementation der 'str-utils' Stringfunktionen für
    Mcalc.
*/



#include <string.h>
#include "smalloc.h"

char*
strappend(char* s1, char* s2){


  size_t s1_len;	/* length of first string      */
  size_t total_size;	/* size   of result buffer     */
  char*  r;		/* temporary buffer for result */
  
  s1_len = strlen(s1); total_size = s1_len +  strlen(s2) + 1;

  r = smalloc(total_size);   /* never returns a 0 pointer */

  strcpy(r,s1); strcpy(r + s1_len, s2);
  
  return r;
}


int
strisprefix(char* s1, char* s2){

  size_t l = strlen(s1);

  return (0 == strncmp(s2,s1,l));
}


