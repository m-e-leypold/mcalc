
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

/*  panic.h -- Utility-Funktionen für Ausgabe von Fehlern und
               Programmabbruch.

    Fehlermeldung werden auf stderr ausgegeben und in der Regel nach
    dem Muster:

       object: message.

    zum Beispiel

       foo.txt: file not found.

    formatiert. Dies folgt der Schule der Unix-Pidgin-Fehlermeldungen
    (die soo schlecht nicht ist :-) und ist ohne eine Toolsbox, welche
    über Fehlermeldung (und deren Formatierung ...) geschickt
    abstrahiert, auch nicht anders machbar.
*/

#ifndef INCLUDED_PANIC_H
#define INCLUDED_PANIC_H

int
panic_syserror (char* message, char* object); /* Ausgabe eines
						 errno-Fehlers und
						 Abbruch */

int
panic (char* message, char* object);          /* Abbruch des
						 Programms */

int
warning (char* message, char* object);        /* Ausgabe einer
						 Warnung */



/* Die folgenden Prozeduren sind nicht implementiert:

int                                           
error (char* message, char* object); 

int                                          
shutdown();

*/


#endif
