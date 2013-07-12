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

/*  parser.h -- Interface für den Mcalc Parser.

*/

#ifndef INCLUDED_PARSER_H
#define INCLUDED_PARSER_H

#include "lexer.h"

typedef struct {                 /* Parser Zustand */
  
  lexer* lx;

  FILE*  output_channel;
  FILE*  error_channel;
  int    errors;

  /* no private fields */

}        parser;


void
parser_init(parser* p, lexer* lx, FILE*  output_channel, FILE*  error_channel)
     
     /* Parser initialisieren.
     */
;


int
parser_evaluate(parser* p, int single_line)
     
     /* Tokenstrom aus p->lx parsen und Ausdrücke evaluieren.  Fehler
	werde auf p->error_channel geschrieben, Resultate (inklusive
	Fehlernotizen) auf p->output_channel. 
	
	Bei (single_line != 0) wird nur die erste Zeile geparst und
	verarbeitet. Das Token nach EOL wird nie begonnen zu
	analysieren, d.h. EOl ist das letzte Zeichen, das jeweils
	gelesen wird.

     */
;


#endif






