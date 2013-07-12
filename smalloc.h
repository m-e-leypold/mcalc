
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

/*  smalloc.h -- Wrapper um malloc/free-Funktionen.

    Die Prozeduren in 'smalloc' funktionieren im Wesentlichen wie ihre
    Analoge in malloc.h, mit dem einzigen Unterschied, dass sie nie
    einen Nullzeiger zurückgeben, sondern, wenn die Speicherbelegung
    scheitert, 

    Über Sinn- und Unsinn dieser Funktionen lässt sich trefflich
    streiten. Aufgrund des Memory-Overcommit wird auf modernen
    Systemen sowieso in den seltensten Fällen ein Nullzeiger
    zurückgegeben. Vielmehr wird der Speicher scheinbar belegt und
    erst implizit durch die VM bereitgestellt, wenn auf die
    betreffenden Speicherseiten geschrieben wird.

    Ist diese implizite Bereitstelleung nicht möglich, so werden
    entweder irgendwelche Applikationen durch das OS (wahllos ...)
    beendet, oder die Applikation, die gerade keine Speicherseite mehr
    haben kann, erhält ein SIGSEGV.

    Dieses Szenario macht es für applikationens-seitig ungeheuer
    schwer, eine "Out-Of-Memory"-Situation zu erkennen, geschweige
    denn sinnvoll zu behandeln (schließlich treten die Probleme nicht
    mehr beim Aufruf von malloc() auf).

    Um das Problem zumindest zeitlich dann auftreten zu lassen, wenn
    alloziert wird, ist 'smalloc' so implementiert, dass durch
    smalloc() und sresizemem() allozierter Speicher unmittelbar mit
    konstanten Daten beschrieben wird. sresizemem() benötigt zu diesem
    Zweck die alte Grösse des Speicherbereichs als zusätzlichen
    Parameter.
*/

#ifndef INCLUDED_SMALLOC_H
#define INCLUDED_SMALLOC_H


#include <stdlib.h>

void*
smalloc (size_t size );               /* analog zu malloc() */

void
sfree (void *ptr );                   /* analog zu free() */

void*                                 /* analog realloc() */
srealloc(void *ptr, size_t size);

void*                                 /* wie realloc(), siehe oben */
sresizemem(void *ptr, size_t size, size_t old_size);


#endif

