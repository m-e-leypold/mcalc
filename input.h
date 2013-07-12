
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

/*  input.h -- Interface des Moduls 'input'. Abstrahiert �ber die
               Eingabe von Mcalc.

    'input' pr�sentiert die Eingabe in zwei Sichten: Als eine Folge
    von Eingabesymsbolen des Typs 'input_symbol' und als eine Folge
    von (vollst�ndigen!) Zeilen, gefolgt einer Datei-Ende-Markierung
    (d.h. die Eingabesymbole werden zeilenweise gelesen, EOF ist ein
    spezielles Eingabesymbol).

    Wenn die letzte Zeile der Datei kein '\n' am Ende hat, so wird
    dieses eingef�gt (um f�r den Nutzer schwer verst�ndliche Fehler
    von vornherein zu vermeiden).

    Im Wesentlichen definiert 'input' zwei wichtige Datentypen:
    'input_symbol' und 'input_source', sowie die Operationen auf
    diesen Datentypen.

    Dar�berhinaus werden zwei Utility-Prozeduren f�r den Lexer
    definiert -- input_isprefix und input_strtol --, die es dem Lexer
    m�glich machen, bestimmte Operationen auszuf�hren, ohne entweder
    die Datenabstraktion (soweit vorhanden :-) zu brechen oder
    (ineffiziente?) dynamische Speicherverwaltung beginnen zu m�ssen.

*/

#ifndef INCLUDED_INPUT_H
#define INCLUDED_INPUT_H

#include <stdio.h>


/*   DATENTYP input_symbol.

 */

typedef int           input_symbol;     /* Ganzzahltyp, exakter Typ soll als PRIVATE
                                           behandelt werden */
typedef unsigned char input_char;


int 
input_is_eof(input_symbol s);           /* Ist das Symbol der EOF-Marker ? */

int 
input_is_whitespace(input_symbol s);    /* Repr�sentiert das Symbol nichtdruckbaren
					   Leerraum? (Whitespace) */ 

int                                     /* Ist das Symbol eine Ziffer? */
input_is_digit(input_symbol s);


/*   DATENTYP input_source.

 */


typedef struct {
  
  FILE*  file;

  /* PRIVATE -- structure members below are private to the implementation */
  
  char*  buf ;            /* Gr�sse wird dynamisch angepasst.             */ 
  size_t buf_size;        /* Aktuelle Puffer-Gr�sse.                      */
  size_t buf_fill;        /* Zul�ssiger Bereich: 0 .. buf_size-1,
			     Zeichen im Puffer _ohne_ abschliessende \0   */

  size_t current_char;    /* if buf_fill>0 then 0 .. buf_fill-1, 
                                           else 0                         */
  int    current_line;    /* >=0, aktuelle Zeile in buf.                  */

  char*  old_buf ;        /* Jeweils die letzte gelesene Zeile            */
  size_t old_buf_size;


  /* Nach dem Lesen der letzten Zeile ist
     (a) feof(file) wahr, (b) buf_fill == 0, (c) Erstes Byte im Puffer  '\0'.
     
     Dies ist essentiell f�r ein funktionieren einiger input interne
     Prozeduren.
  */

}        input_source;


void
input_source_init(input_source* s, FILE* f);    /* Initialiere S, verbinde
						   mit f                    */

int 
input_source_current_line(input_source* s);     /* Augenblickliche 
						   Zeilennummer             */
size_t
input_source_current_char_pos(input_source* s); /* Position innerhalb 
						   der Zeile                */

input_symbol
input_current_symbol(input_source* s);          /* Augenblickliches 
						   Eingabe-Symbol           */

input_symbol
input_next_symbol(input_source* s);             /* N�chstes Eingabesymbol 
						   holen, Eingabezeige wird 
						   um eins erh�ht           */

void
input_source_forward(input_source* s, int n);   /* n Zeichen in der Eingabe 
						   nach vorne gehen */


int
input_is_prefix(input_source* s, char* p);      /* 1, wenn p Pr�fix der 
						   aktuellem Zeile in s ist,
						   0 sonst. */

long
input_strtol(input_source* s);     /* Wandelt einen als Dezimalzahl
				      interpretierbaren Pr�fix der
				      aktuellen Zeile ab dem
				      Eingabepuffer in einen long int
				      um. Vorbedingung: Aktuelles Zeichen sollte
				      eine Ziffer sein. [1] */


/* [1] strtol bewegt den Eingabezeige bis zum letzten Zeichen des
   umgewandelten Pr�fix/Substring. */
 
int
input_fprint_line(FILE* f, input_source* s, int line);

             /* Schreibt die aktuelle oder die letzte Zeile auf f, je
		nachdem, welche Zeilenummer 'line' angegeben
		ist. Diese Prozedur ist vor allem f�r Fehlermeldungen
		n�tzlich. */

#endif
