
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

/*  str-utils.h -- Interface für 'Mcalc' string utility functions.
*/

#ifndef INCLUDED_STR_UTILS_H
#define INCLUDED_STR_UTILS_H

char*
strappend(char* s1, char* s2); 

/* ^ Hängt die 0-begrenzten C-Strings s1, s2 aneinander. Der
   zurückgegebene Zeige is mit malloc() allozierter Speicher, s1, s2
   bleiben unberührt.

   Gibt nie einen 0-Zeiger zurück (stattdessen wird das Programm in
   der Prozedur abgebrochen).

 */


int
strisprefix(char* s1, char* s2);

/* Prüft, ob s1 ein Prefix von s1 ist. Beide C-Strings müssen durch
   '\0' begrenzt sein. 
*/



#endif
