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

/*  lexer.c -- Mcalc Lexer.
 
    Zur Beschreibung von Funktion und Schnittstelle s. 'lexer.h'.

*/


#include <assert.h>

#include "input.h"
#include "lexer.h"

#include "symtable.h"


/* DATENTYP lexer_token_class ------------------------------------------------
 */

int
lexer_token_class_is_numeral(lexer_token_class c){

  return (LEXER_LAST_NUMERAL_TOKEN >= c) && (c <= LEXER_LAST_NUMERAL_TOKEN);
}



/* DATENTYP lexer ------------------------------------------------------------
 */


void
lexer_init(lexer* l, input_source* s){   /* Initialisierung */

  l->source           = s;
  l->next_token.class = 0;

  lexer_next_token(l);
}


lexer_token
lexer_current_token(lexer* l){

  return l->current_token;
}


static void
set_current_token(lexer* l, lexer_token_class class, number val, size_t len){

  /* Utility Funktion um current_token mit Typ und Angabe der
     Quelltextposition im Lexer-Zustand zu setzen und den
     Eingabezeiger auf das letzte Zeichen des Lexemes des Tokens
     vorzurücken.
  */

  lexer_token* tok = &(l->current_token);

  
  tok->class        = class;
  tok->val          = val;
  tok->line         = input_source_current_line(l->source);
      
  tok->lexeme_start = input_source_current_char_pos(l->source);
  tok->lexeme_end   = tok->lexeme_start + (len-1);

  if (len) {                                 /* (len == 0) ==> nur bei EOF */
    input_source_forward(l->source, len-1);
  }
}
      

static int 
try_keyword(lexer* l){

  /* Versucht alle Wörter/Strings im Array symtable[] als Präfixe des
     Eingabestroms zu identifizieren.

     RÜCKGABEWERTE:

       0, wenn kein Wort erkannt wurde.

       1, wenn erfolgreich. Dann wird als Seiteneffekt
          l->current_token gesetzt und der Eingabezeiger vorgerückt.
  */


  int i        = 0;
  char* lexeme = symtable[i].lexeme;
  size_t n;
  
  while (lexeme){

    if(input_is_prefix(l->source, lexeme)){

      n = strlen(lexeme);
      set_current_token(l, symtable[i].class, symtable[i].val, n);
      return 1;
    };
    
    i++;
    lexeme = symtable[i].lexeme;
  }
  
  return 0;
}


void
lexer_push_token(lexer* l, lexer_token t){

  assert(!(l->next_token.class));

  l->next_token    = l->current_token;
  l->current_token = t;
}

lexer_token
lexer_next_token(lexer* l){

  input_symbol sym;

  if (l->next_token.class) {               /* Zurückgeschriebenes
					      Token vorhanden? */
    
    l->current_token    = l->next_token;
    l->next_token.class = 0;

  } else {                                 /* -> Nein, dann neues
                                                 Token erkennen ... */
    sym = input_next_symbol(l->source);       

    while (input_is_whitespace(sym)){        /* Whitespace überlesen */
      sym = input_next_symbol(l->source); }

    if (input_is_digit(sym)) {               /* Ziffer -> Zahl erkannt */
      set_current_token(l, LEXER_TOKEN_CLASS_NUMBER, input_strtol(l->source), 0);
    } else {
      
      if (input_is_eof(sym)) {               /* Dateiende-markierung? */
	set_current_token(l, LEXER_TOKEN_CLASS_EOF, NUMBER_0, 0);	
      } else {
	
	if (!try_keyword(l)){ /* Probiere Schlüsselörter / symtable[],
				 wenn nichts erkannt, einzelnes
				 Zeichen zurückgeben. */

      	  set_current_token(l, LEXER_TOKEN_CLASS_CHAR, NUMBER_0, 1);
	}
      }
    }
  }
  return lexer_current_token(l); 
}



