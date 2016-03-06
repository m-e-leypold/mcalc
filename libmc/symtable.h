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

/*  symtable.h -- Symboltafel für Mcalc.
    
    Im vorliegenden einfachen Fall wurde darauf verzichtet, das
    Interface der Symboltafel in Prozeduren zu formulieren. Die
    Symboltafel ist einfach ein grosses Array von 'symtable_entry's,
    dessen Ende k dadurch gekennzeichnet ist, dass symtable[k].class
    == 0 ist.
*/


#ifndef INCLUDED_SYMTABLE_H
#define INCLUDED_SYMTABLE_H



#include "number.h"
#include "lexer.h"


typedef struct {

  char*               lexeme;
  lexer_token_class   class;  
  number              val;

} symtable_entry;


extern symtable_entry symtable[];


#endif
