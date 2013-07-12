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

/*  number.h -- Versuch �ber den numerischen Typ des Resultates von
    Auswertungen der Ausdr�cke zu abstrahiern.

    Dieser Versuch ist unvollst�ndig. Z.zT. geschehen alle
    Rechenoperationen direkt in 'parser'. Das vorliegende Modul w�re
    auszubauen, wenn zB �berlaufdetektion (generell: Fehlerbehandlung
    bei den arithmetischen Operationen) eingebaut werden soll.

    Dabei w�re etwa folgendes Muster einzuhalten:

      number number_add(number n1, number n2, jmp_buf* errorhandler);

    Alternativ k�nnte im Ergebnis ein Fehlerflag mitgegeben werden
    oder die libgmp zum Einsatz kommen.

*/


#ifndef INCLUDED_NUMBER_H
#define INCLUDED_NUMBER_H

typedef long int number;

#define NUMBER_0 0l              /* Die 0 f�r den Typ 'number'                 */
#define NUMBER_FMT "ld"          /* Formatspezifikation um 'number' auszugeben */

#endif
