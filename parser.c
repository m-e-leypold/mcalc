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

/*  parser.c -- Mcalc Parser.

    Aufgabe des Parsers ist es , aus dem Token-Strom einen Parsetree
    für die Eingabe zu konstruieren, diesen abzulaufen, dabei den Wert
    des Ausdrucks als 'synthetisiertes Attribut' (siehe Aho, Sethi,
    Ullmann: Compilers) zu evaluieren und Aktionen im Backend
    auszulösen.

    Im vorliegenden Fall ist das Attribut vom Typ 'number', das
    Backend dagegen die Ausgabedatei und der Standardfehlerkanal.

    BEMERKUNGEN ZUR IMPLEMENTATION:

      Mittel der Wahl für 'manuell' implementierte Parser ist in der
      Regel ein rekursiv absteigender Parser (vorzugsweise mit einem
      Look-Ahead von 1), bei dem jede Grammatikregel durch eine
      Prozedur implementiert wird, deren Rückgabewert(e) die Attribute
      des jeweiligen Knoten sind. Der Baum wird in diesem Fall nie
      tatsächlich aufgebaut, sondern durch den Graphen tatsächlicher
      Funktionsaufrufe nachgebildet.
    
      Rekursiv absteigende Parser parsen die LL-Grammatiken,
      angestrebt wird hier eine LL(1)-Grammatik (Left-to-right,
      Look-Ahead 1).

      Im vorliegenden Fall stellt sich leider heraus, dass die
      Untergrammatik für die zusammengesetzten Zahlwörter sich nur
      sehr schwer auf eine LL(1) Form bringen lässt (die Methoden
      wären Elimination von Linksrekursion und Linksfaktorisierung)
      ohne wirklich jede Versändlichkeit einzubüssen und in Unzahl von
      Regeln zu Fragmentieren.

      Im Nachhinein wäre vielleicht einer der folgenden Ansätze die
      besser Wahl gewesen:

       - Interessanterweise ist die 'naive' Form der Untergrammatik
         für zusammengesetzte Zahlwörte wohl RL(1): Man könnte alle
         dazugehörigen Tokens in einen Puffer lesen, die Reihenfolge
         _umdrehen_ und durch einen LL(1) rekursiv absteigenden
         Unterparser analysieren lassen.

       - Man könnte die Tokens in einen Puffer lesen, nach dem
         höchsten Skalenfaktor (million, tausend. hundert ...) suchen
         und dann die Teile links und rechts vom Skalenfaktor durch
         rekursiven Aufruf der Funktion analysieren.
       

      Stattdessen wurde aber im vorliegenden Fall die Grammatik für
      zusammengesetzten Zahlwörter in eine LL(2) Form umgeschrieben
      und Infrastruktur zur Verfügung gestellt um nötigenfalls ein
      einzelnes überlesenes Token wieder in den Eingabestrom
      zurückzuschreiben (dies ist die Art wie der Look-Ahead 2 hier
      realisiert wird).
      
      In diesem Kompromiss wird kein unbeschränkter Puffer zur
      Zwischenspeicherung der Tokens benötigt, und die resultierende
      Grammatik bleibt noch einigermassen verständlich.

      Well -- talk about a golden hammer :-).

*/

#include <assert.h>
#include <stdio.h>
#include <setjmp.h>

#include "panic.h"

#include "lexer.h"
#include "parser.h"


/* INFRASTRUCTUR zur Fehlerbehandlung 
   
   Die folgenden Prozeduren und Variablen dienen dem Parser zum
   Fehler-Recovery: Wird ein Fehler gefunden, wird mittels longjmp()
   auf eine höhere Ebene der Aufrufhierarchie zurückgekehrt (und zwar
   auf die Prozedure, die eine vollständige Zeile parst). Es kann dann
   der Rest der Phrase / Zeile ignoriert werden (panic mode recovery).

   Kommunikation erfolgt hier über globale Variabeln,
   d.h. Implementation des Parser-Moduls ist nicht threadsafe.

*/


static char*    errortext = "";

static int      error_line;
static size_t   error_lexeme_start;
static size_t   error_lexeme_end;


static jmp_buf  error_jmp_buf;

static void     
throw_error(char* error_message, size_t lexeme_start, size_t lexeme_end, int line){

  /* Fehlerbedingung auslösen - longjmp() - mit Angabe des anstößigen
     Teils der Eingabe (ein oder mehrere Token einer Zeile */

  errortext            = error_message;
  error_line           = line;
  error_lexeme_start   = lexeme_start;
  error_lexeme_end     = lexeme_end;

  longjmp(error_jmp_buf, 1);
 
  assert(0);  
  return;
}


/* DATENTYP parser.
 */


void
parser_init(parser* p, lexer* l, FILE*  output_channel, FILE*  error_channel){

  p->lx             = l;
  p->output_channel = output_channel;
  p->error_channel  =  error_channel;

  p->errors         = 0;
}


/* INFRASTRUKTUR für rekursiven Parser 

   Um nicht zu lange Ausdrücke schreiben zu müssen und nicht jedesmal
   den Parserzustand an die einzelnen Prozeduren der Parser mitgeben
   zu müssen, werden Teile des Parserzustandes (Eingabequelle,
   Ausgabekanäle) vor Beginn der Evaluation in lokale Variablen
   gespeichert.

   (Sicher keine so gute Idee, aber für den Prototyp des Parsers
   einfacher und verständlicher. Gäbe es verschachtelte Prozeduren in
   C oder rename-Direktiven (wie in Ada), wäre dieser Zirkus nicht
   nötig).

   Konsequenz: Modul 'parser' ist nicht threadsafe.

*/


static FILE*        out;         /* Ausgabedatei    */
static FILE*        err;         /* Fehlerkanal     */
static lexer_token  tok;         /* Aktuelles Token */
static lexer_token  last_tok;    /* Letztes Token   */
static lexer*       lex;         /* Lexer           */

static parser*  current_parser;  /* Parser           */


static void
setup_parsing(parser* p){

  /* Setzen der globalen Variablen aus Parser p. */

  out = p->output_channel;
  err = p->error_channel;
  lex = p->lx;
  tok = lexer_current_token(lex);

  current_parser = p;
}


static void
next_token(){

  /* Nächstes Token holen, Aktuelles Token merken */

  last_tok = tok;
  tok      = lexer_next_token(lex);  
}

static void
token_rewind(){

  /* Letztes Token zurückschreiben */
  
  lexer_push_token(lex,last_tok);
  tok = lexer_current_token(lex);
}


static void
match_token(lexer_token_class c, char* message){

  /* Prüfen ob aktuelles Token vom gegebenen Typ ist, sonst
     Fehlerbehandlung auslösen */

  if (tok.class != c) {
    
    throw_error(message, tok.lexeme_start, tok.lexeme_end, tok.line);
  }
  next_token();
}



#define TOK_IS(x)  (tok.class == LEXER_TOKEN_CLASS_##x)     /* Abkürzungen   */
#define MATCH(x,m) (match_token(LEXER_TOKEN_CLASS_##x,m))   /* (böse Tricks) */



void
throw_error_current_lexeme(char* error_message){

  /* Fehlerbehandlung mit dem Lexeme des aktuellenToken tok als
     markiertem Fehlerbereich auslösen */
  
  throw_error(error_message, tok.lexeme_start, tok.lexeme_end, tok.line);
}


/* INFRSTRUKTUR zur Ausgabe "schöner" visueller Fehlermeldungen 
 */


static void 
ftab_to(FILE* f, int n){                 /* Zur angegebenen Position tabulieren */
  
  for (;n>-15; n--) { fputc(' ',f);};
}


static int
fprint_error(FILE* f){                    /* Aktuellen Fehler ausgeben
					     (mit Markierung und
					     Meldung) */

  int i;

  fprintf(f,"\n");
  fprintf(f,"  Zeile %-4d - ",error_line);
  input_fprint_line(f, lex->source, error_line);

  ftab_to(f,error_lexeme_start);
  for (i=error_lexeme_start; i<=error_lexeme_end; i++) { fputc('-',f);};
  fprintf(f,"\n");

  ftab_to(f,error_lexeme_start);
  fprintf(f,"| %s.\n\n", errortext);
  
  fflush(f);

  return 0;
}


/* PARSER für Untergrammatik der zusammengesetzten Zahlwörter  ----------
*/

static
int token_is_numeral(){
  
  return lexer_token_class_is_numeral(tok.class);
}


number 
parse_range_2_9(){             /* ==> Alle Zahlwörter von 'zwei' bis 'neun' */

  number n;
  
  if (!TOK_IS(O_ONE)){
    throw_error_current_lexeme("Zahlwort von zwei (oder eins) bis neun erwartet."); }

  n=tok.val; next_token(); return n;
}


number 
parse_range_1_9(){             /* ==> Alle Zahlwörter von 'eins' bis 'neun' */

  number n;

  if (TOK_IS(eins))   {  n=tok.val; next_token(); return n; };
  return parse_range_2_9();
}


number 
parse_range_20_90(){           /* ==> Alle Zehner von 'zwanzig' bis 'neunzig' */

  number n;
  
  if (!(TOK_IS(O_TEN))){
    throw_error_current_lexeme("Zahlwort (ganze Dekade) von 20..90 erwartet."); }

  n=tok.val; next_token(); return n;
}


number                         /* ==> Alle Zahlwörter von 'zwei' bis 'neunundneunzig' */
parse_range_2_99(){

  number n;
  
  if (TOK_IS(eins))       { return parse_range_1_9();   }
  if (TOK_IS(O_TEN))      { return parse_range_20_90(); };
  if (TOK_IS(ELEVEN_UP))  { n = tok.val; next_token(); return n; };

  if (TOK_IS(ein)){ 
    n = tok.val; next_token(); MATCH(und,"Hier wurde 'und' erwartet");
    return n+parse_range_20_90();
  };

  n = parse_range_1_9(); 

  if (TOK_IS(und)){ next_token(); return n+parse_range_20_90(); }

  return n;
}


number 
parse_range_1_99(){           /* ==> Alle Zahlwörter von 'eins' bis 'neunundneunzig' */
  
  if (TOK_IS(eins))   { return parse_range_1_9();   }

  return parse_range_2_99();
}



number
parse_opt_1_99(){             /* ==> '' oder alle Zahlwörter von 'eins' bis 'neunundneunzig' */
  
  if (!(token_is_numeral() && (tok.val < 100))) { return 0; };
  
  return parse_range_1_99();
}


number 
parse_range_2_999(){          /* ==> Alle Zahlwörter von 'zwei' bis "999" */

  number n;

  if (TOK_IS(hundert))  { n=tok.val; next_token(); return n + parse_opt_1_99() ; }


  if ((TOK_IS(ein)) || (TOK_IS(O_ONE)))  {  

    n = tok.val; next_token();

    if (TOK_IS(hundert)){ n=n*tok.val; next_token(); return n + parse_opt_1_99() ; }

    token_rewind(); return parse_range_1_99();
  };

  return parse_range_1_99();
}


number 
parse_range_1_999(){          /* ==> Alle Zahlwörter von 'eins' bis "999" */

  if (TOK_IS(eins)) { return parse_range_1_9();   }
  return parse_range_2_999();
}


number
parse_opt_1_999(){            /* ==> '' oder alle Zahlwörter von 'eins' bis "999" */
  
  if (!(token_is_numeral() && (tok.val<1000))) { return 0; };
  
  return parse_range_1_999();
}



number 
parse_range_2_999999(){       /* ==> Alle Zahlwörter von 'zwei' bis "999999" */

  number n;

  if (TOK_IS(tausend))  { n=tok.val; next_token(); return n + parse_opt_1_999() ; }

  if (TOK_IS(ein)) { 
    
    n = tok.val; next_token();
    
    if (TOK_IS(tausend)){ n = n*tok.val; next_token(); return n + parse_opt_1_999() ; }
    else                { token_rewind(); };
  };
  
  n = parse_range_2_999();
  
  if (TOK_IS(tausend))  { n=n*tok.val; next_token(); return n + parse_opt_1_999() ; }
  
  return n;
}


number 
parse_range_1_999999(){       /* ==> Alle Zahlwörter von 'eins' bis "999999" */

  if (TOK_IS(eins))     { return parse_range_1_9();   }

  return parse_range_2_999999();
}



number
parse_opt_1_999999(){         /* ==> '' oder alle Zahlwörter von 'eins' bis "999999" */
  
  if (!(token_is_numeral()&&(tok.val<1000000))) { return 0; };
  
  return parse_range_1_999999();
}



number 
parse_range_1_999999999(){    /* ==> Alle Zahlwörter von 'eins' bis "999999999" */

  number n;

  size_t lexeme_start = tok.lexeme_start;

  if (TOK_IS(eins))     { return parse_range_1_9();   }

  if (TOK_IS(eine)) { 
    next_token(); n = tok.val; MATCH(million,"Zahlwort 'Million' erwartet");
    return n + parse_opt_1_999999();
  }

  n = parse_range_2_999999();
  
  if (TOK_IS(millionen))  { 
    
    if (n>999) {
      throw_error("Mehr als 999 Millionen werden nicht unterstützt (Überlaufgefahr)",
		  lexeme_start, tok.lexeme_end, tok.line);

      /* Syntax wie 'einhundertausend millionen' wird verweigert */

    };
    
    n=n*tok.val; next_token(); return n + parse_opt_1_999999() ; }
  
  return n;
}




number
parse_number(){

  number n;

  if (TOK_IS(null)) { n=tok.val; next_token(); return n; }  /* Null ist ein Sonderfall */
  return parse_range_1_999999999();
}



/* PARSER für Ausdrücke ----------------------------------------------------


   Standard rekursiv absteigender Parser.

   Das 'matchen' von EOL wird bis auf den Toplevel verschoben, damit
   im interkativen Modus das Programm beendet werden kann, ohne dass
   ein Token nach EOL gelesen wird.
*/



number
parse_expression(); 


number
parse_factor(){    /*   <factor> ->   <number> 
                                    | '-' <factor> 
				    | '(' expression ')'
		   */

  number n;
  
  
  if (TOK_IS(NUMBER))     { n = tok.val; next_token(); return n; }
  if (token_is_numeral()) { return parse_number(); }
  if (TOK_IS(MINUS))      { next_token(); return - parse_factor(); }

  if (!(TOK_IS(PAR_OPEN))){
    
    throw_error_current_lexeme("Zahl oder '(' erwartet");
  }

  next_token();
  n = parse_expression();
  MATCH(PAR_CLOSE, "Schließende Klammer ')' erwartet");
  return n;
}

number
parse_factor_list(){       /*  <factor-list> ->  <factor>
                                             |   <factor> '*' <factor-list>
			                     |   <factor> '/' <factor-list>
                                             |   ...          
  		           */

    number n1;

  n1 = parse_factor();

  while(1){

    if (TOK_IS(MULT)) { next_token(); n1 = n1 * parse_factor();  continue; }
    if (TOK_IS(DIV))  { next_token(); n1 = n1 / parse_factor();  continue; }

    if (TOK_IS(geteilt)) { 
      
      next_token(); MATCH(durch,"Schlüsselwort 'durch' erwartet");
      n1 = n1 / parse_factor();  
      continue; 
    }
    
    break;
  }

  return n1;
}


number 
parse_term(){                 /* <term> -> <factor-list> */

  return (parse_factor_list());
}


number 
parse_term_list(){            /*  <term-list> ->   <term>
                                              |   <term> '+' <term-list>
			                      |   <term> '-' <term-list>
                                              |   ...          
       		               */
  
  number n1;

  n1 = parse_factor_list();

  while(1){

    if (TOK_IS(PLUS))   { next_token(); n1 = n1 + parse_term(); continue; }
    if (TOK_IS(MINUS))  { next_token(); n1 = n1 - parse_term();  continue; }
    break;
  }

  return n1;
}

number
parse_expression(){            /* <expression> -> <term-list> */
  
  return parse_term_list();
}



static void 
recover(){                    /* 'panic mode' Recovery: Vorspulen bis
			          zum Ende der Zeile */
  
  while(!(TOK_IS(EOL))){

    assert(!(TOK_IS(EOF)));    /* if triggered indicates internal bug */
    next_token();
  };
}



void
parse_line(parser* p){         /* parse genau eine Zeile, erzeuge
				  Ausgabe. Evtl. abschliessendes EOL
				  wird noch nicht konsumiert. */

  number n;


  if (setjmp(error_jmp_buf)){ /* Fehlerbehandlung aufsetzen */

    /* Fehler ist aufgetreten: Fehlermeldungen in Ausgabedatei und
       Standardfehlerkanal schreiben, Recovery */

    fprintf(out,"ERROR: line %d: characters %ld-%ld: %s.\n", 
	    error_line, error_lexeme_start, error_lexeme_end, errortext);
    fprint_error(err);
    recover();
    current_parser->errors++;
    
  } else {                     /* Resultat ausgeben */
    
    if (TOK_IS(EOL)) {fprintf(out,"\n"); return ; }
    
    n = parse_expression(p); 
    fprintf(out,"%" NUMBER_FMT "\n",n);
  };
}



int
parser_evaluate(parser* p, int single_line){   /* s. parser.h */
  
  setup_parsing(p);

  while (!(TOK_IS(EOF))) {

    if (single_line){ fprintf(out," => "); };

    parse_line(p); 

    if (single_line && TOK_IS(EOL)) break;
    MATCH(EOL,"expected end of line here");
  };

  return 1;
}



