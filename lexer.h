
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

/*  lexer.h -- Schnittstelle zum Mcalc Lexer.

    Aufgabe des Lexers ist es, den Strom der Eingabesymbole in
    W�rter/Symbole/Zahlen/Operatoren (im Compiler-Speak: Tokens) zu
    unterteilen und redunante Leerr�ume zu entfernen. Dies vereinfacht
    die Aufgabe f�r den Parser.

    Im vorliegende Fall gibt es zwei Besonderheiten: 

    (1) Wie bereits in input.c angemerkt, soll unn�tiger Look-Ahead
        vermieden werden, um zu vermeiden, dass der Parser implizit
        das lesen der n�chsten Zeile anst�sst, ehe die Zeile komplett
        ausgewertet ist. Dies w�rde im interaktiven Modus dazu f�hren,
        dass das Parsen / die Auswertung einer Zeile erst dann
        abgeschlossen wird / werden kann, wenn die n�chste Zeile
        vollst�ndig eingegeben / gelesen ist (Pufferung durch das
        Betriebsystem!).

	Im Compilerbau �blich ist es, mit (konstant) einem
	Eingabesymbol Look-Ahead zu arbeiten. Dies kommt im
	vorliegenden Fall nicht in Frage.

    (2) Leerr�ume zwischen Wortbestandteilen sollten optional sein
        (das habe ich MEL jetzt so gesetzt). Die Eingaben
        'zweihundert' und 'zwei hundert' sollen �quivalent sein.

	Das erleichtert sowohl Parsen der Zahlenphrasen, als auch die
	Eingaben f�r den Benutzer.	

    Um die Ziele in 1+2 zu erreichen wird folgenderma�en vorgegangen:

    Anstatt dass Worte durch nichtalphanumerische Zeichen begrenzt
    werden (wie im Compilerbau �blicher), werden Wort als Pr�fixe des
    Eingabestroms erkannt. (Die zu erkennenden Strings sind im
    Array symtable (Modul 'symtable') gespeichert.)

    Wenn ein Token erkannt ist, wird der Eingabezeiger der
    input_source auf das letzte Zeichen des Token vorger�ckt (anstatt
    wie �blich auf das Zeichen danach).

    Lexer-Tokens und der Lexer-Zustand werden jeweils als abstrakte
    Datentypen implementiert.

    Tokentypen werden als Aufz�hlungstypen implementiert.

    Um eine Grammatik mit eine Token-Look-Ahead von 2 zu
    implementieren (siehe 'parser') wurde die M�glichkeit vorgesehen
    mit lexer_push_token() _ein_ Token wieder in die Eingabe
    zur�ckzuschreiben.

*/

#ifndef  INCLUDED_LEXER_H
#define  INCLUDED_LEXER_H

#include "number.h"
#include "input.h"


/* DATENTYP lexer_token_class ------------------------------------------------
*/


typedef enum {                       /* Tokentypen, 0 ist reserviert
					als Ende-Markierung f�r Symtable */

  LEXER_TOKEN_CLASS_null = 1,        /* Es ist essentiell f�r                */
  LEXER_TOKEN_CLASS_eins,            /* lexer_token_class_is_numeral(), dass */ 
  LEXER_TOKEN_CLASS_ein,             /* alle Zahlw�rter einen geschlossenen  */
  LEXER_TOKEN_CLASS_eine,            /* Block bilden. */
  LEXER_TOKEN_CLASS_O_ONE,
  LEXER_TOKEN_CLASS_ELEVEN_UP,
  LEXER_TOKEN_CLASS_O_TEN,
  LEXER_TOKEN_CLASS_hundert,
  LEXER_TOKEN_CLASS_tausend,
  LEXER_TOKEN_CLASS_million,
  LEXER_TOKEN_CLASS_millionen,

  LEXER_TOKEN_CLASS_und,
  LEXER_TOKEN_CLASS_geteilt,
  LEXER_TOKEN_CLASS_durch,

  LEXER_TOKEN_CLASS_NUMBER,
  LEXER_TOKEN_CLASS_PLUS,
  LEXER_TOKEN_CLASS_MINUS,
  LEXER_TOKEN_CLASS_MULT,
  LEXER_TOKEN_CLASS_DIV,
  LEXER_TOKEN_CLASS_PAR_OPEN,
  LEXER_TOKEN_CLASS_PAR_CLOSE,

  LEXER_TOKEN_CLASS_CHAR,
  LEXER_TOKEN_CLASS_EOL,
  LEXER_TOKEN_CLASS_EOF

} lexer_token_class;

/* Beginn und Ende  der Zahlw�rter-Tokentypen (f�r lexer_token_class_is_numeral) */

#define LEXER_FIRST_NUMERAL_TOKEN LEXER_TOKEN_CLASS_null     
#define LEXER_LAST_NUMERAL_TOKEN  LEXER_TOKEN_CLASS_millionen


int                                                   
lexer_token_class_is_numeral(lexer_token_class c); /* Ist c ein
						      Tokentype f�r
						      ein Zahlwort? */


/* DATENTYP lexer_token -------------------------------------------------------
*/

typedef struct {

  lexer_token_class class; 
  number val;

  int line;
  int lexeme_start;
  int lexeme_end;

  /* no private implementation part */
  
}       lexer_token;


/* DATENTYP lexer -------------------------------------------------------------
*/

typedef struct {

  input_source* source;

  /* PRIVATE: All members below are private to the implementation. */

  lexer_token current_token; /* Aktuelles (zuletzt erkanntes) Token */

  lexer_token next_token;    /* Zwischenspeicher f�r
				zur�ckgeschriebenes Token,
				s. lexer_push_token().  */

}       lexer;


void
lexer_init(lexer* l, input_source* s);  /* Initialisiere Lexer,
					   verbinde mit Eingabequelle
					   s. */

lexer_token
lexer_current_token(lexer* l);          /* Aktuelles (zuletzt
					   erkanntes) Token */
lexer_token
lexer_next_token(lexer* l);             /* N�chstes Token erkennen und
					   zur�ckgeben */
void
lexer_push_token(lexer* l, lexer_token t);  /* Token in Tokenstrom
					       zur�ckschreiben */

/* bzgl. lexer_push_token(): Es ist nur Platz f�r genau ein
   zur�ckgeschriebenes Token: Dieses Token muss zuerst wieder
   konsumiert werden (mit lexer_next_token(), ehe ein weiteres
   zur�ckgeschrieben werden kann. */

#endif

   

   
