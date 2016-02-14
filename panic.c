
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

/*  panic.c -- Utility-Funktionen für Ausgabe von Fehlern und
               Programmabbruch.
*/




#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int
panic_syserror(char* message, char* object_name) {

  fflush(stdout);
  fprintf(stderr,"%s: %s.\n",object_name, message);
  perror(object_name);
  abort();
}


int
panic(char* message, char* object_name) {

  fflush(stdout);
  fprintf(stderr,"%s: %s.\n",object_name, message);
  abort();
}


int 
warning(char* message, char* object_name){

  fflush(stdout);  
  fprintf(stderr,"Warning: %s: %s.\n", object_name, message);
  return 1;
}

