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

/*  input.c -- Implementierung des Moduls 'input' (Eingabeabstraktion
    für Mcalc).

    Zum Interface siehe 'input.h'.

    Die meisten Prozeduren sind verhältnismäßig trivial. Dreh- und
    Angelpunkt für alles sind die Bedingungen end_of_buf(), die
    aussagt, wann neu gelesen werden muss und read_line() in dem die
    ganze "harte Arbeit" geschieht.

    Alle Prozeduren sind so formuliert, dass unnötiger Look-Ahead über
    Zeilenenden hinaus vermieden wird: Dies würde im interaktiven
    Modus dazu führen, dass das Programm bereits während der
    Auswertung der ersten Zeile beginnt, die zweite Zeile vom Terminal
    zu lesen und deshalb die Ausgabe zur ersten Zeile erst nach der
    vollständigen Eingabe der zweiten Zeile durch den Benutzer
    ausgibt. Im interkativen Modus ist das nicht erwünscht.
*/


#include <assert.h>
#include <string.h>

#include "panic.h"
#include "smalloc.h"
#include "str-utils.h"
#include "input.h"

#define INITIAL_BUF_SIZE 11     /* small for testing and demonstration purposes   */
#define BUF_INCREMENT 50        /* minimum quantum for increasing the buffer size */



/*  DATENTYP input_symbol.   -----------------------------------------------

*/


#define INPUT_EOF     -1

int 
input_is_eof(input_symbol s){
  return s == INPUT_EOF;
}


int 
input_is_whitespace(input_symbol s){

  return (s==(input_symbol)' ') || (s==(input_symbol)'\t') ;
}


int 
input_is_digit(input_symbol s){

  return (s>=(input_symbol)'0') && (s<=(input_symbol)'9') ;
}


/* DATENTYP input_source  ---------------------------------------------------------

   
*/


void
input_source_init(input_source* s, FILE* f){

  /* Initialisiert 'input_source'.
     
     Wie oben bereits erläutert, bleibt der Eingabezeiger _vor_ dem
     ersten Zeichen der Datei stehen, um 'lookahead' soweit als
     möglich zu vermeiden.
  */
  
  s->file         = f;
  s->buf          = smalloc(INITIAL_BUF_SIZE); /* smalloc never returns 0 */
  s->buf_size     = INITIAL_BUF_SIZE;
  s->buf_fill     = 0          ;
  s->current_line = 0          ;
  s->current_char = 0          ;

  s->old_buf      = smalloc(INITIAL_BUF_SIZE);
  s->old_buf_size = INITIAL_BUF_SIZE;
}


int 
input_source_current_line(input_source* s){ return s->current_line; }

size_t
input_source_current_char_pos(input_source* s){ return s->current_char; }


static size_t
input_source_free_bytes(input_source* s){ 

  /*  gibt zurück: Anzahl der freien Bytes im Eingabepuffer */

  return s->buf_size - 1 -  s->buf_fill;
}


input_symbol
input_current_symbol(input_source* s){

  /* Prüfe ob EOF-Bedingung zutrifft, wenn ja, INPUT_EOF zurückgeben,
     ansonsten das aktuelle Zeichen. 

     Der Aufruf dieser Prozedure ist nur zulässig, wenn entweder der
     Eingabezeiger auf bereits auf einem gültigen Eingabesysmbol
     steht, oder das Dateiende erreicht ist. 
  */

  assert(!(ferror(s->file)));

  if (s->buf_fill==0) return INPUT_EOF;
  return s->buf[s->current_char];
}


static int
end_of_buffer(input_source* s){ 

  /* Prüft, ob der Zeiger auf das letzer Zeichen des Eingabepuffers
     zeigt, also, ob es notwendig ist, vor dem Weitersetzen des
     Zeigers bzw Holen des nächsten Zeichens eine neue Zeile
     einzulesen */

  return (!(s->buf_fill) || (s->buf_fill - s->current_char == 1));
}


static size_t
ensure_free_bytes(input_source* s, size_t free_bytes){

  /* Stellt sicher, dass mindestens free_bytes freie Bytes im
     (aktuellen) Eingabepuffer sind. Falls nötig wird der Puffer
     vergrössert. */
 
  size_t new_size = s->buf_size;
  
  
  while ((new_size - 1 - s->buf_fill ) < free_bytes) {
    new_size *= 2;
  }

  s->buf      = sresizemem(s->buf, new_size, s->buf_size); /* sresizemem never returns 0 */
  s->buf_size = new_size;

  return input_source_free_bytes(s);
}



static int
read_line(input_source* s){


  /* read_line präsentiert den Inhalt einer Datei als Folge von
     Zeilen, denen eine EOF-Marker folgt. Die Prozedure ermöglicht es,
     diese Zeilen nacheinander in s->buf zu lesen und eine
     EOF-bedingung zu erkennen.

     Der interne Puffer wird bei Bedarf vergrössert.

     Enthält die letzte Zeile kein '\n' am Ende, so wird dieses
     stillschweigend angehängt.
     
     BESCHRÄNKUNGEN:

       Eine Eingabedatei darf keine '\0'en enthalten.

       Wirklich lange Zeilen führen dazu, dass der Speicher nicht mehr
       aussreicht und/oder die Host-Maschine beginnt zu trashen. Es
       gibt kein explizites Limit auf die Zeilenlänge.

       Die interne Puffergrösse wird während der Laufzeit des
       Programms nie reduziert.

     BEMERKUNGEN ZUR IMPLEMENTATION:

       Der Literatur zum C89 Standard zufolge wird das eof-Flag auf
       einer FILE*-Struktur dann gesetzt, wenn versuch wird, über das
       letzte Zeichen hinauszulesen.

       Im vorliegenden Fall wird versucht mit fgets die Datei
       zeilenweise einzulesen. Das bedeutet, dass das EOF-Flag
       u.U. bereits gesetzt wird, wenn die letzte Zeile gelesen wird
       (weil dieser das '\n' am Ende fehlt).
       
       Die vorliegende Implemantation verbirgt den Unterschied
       zwischen dem Fall einer vollständige und einer unvollständigen
       letzten Zeile in der Datei: EOF wird erst signalisiert, wenn
       read_line nach der letzten Zeile ein weiteres Mal aufgerufen
       wird.

       Wenn die Eingabe '\0' enthält, wird er Rest der Zeile
       (inklusive des '\n' ignoriert, d.h. die betreffende Zeile wird
       vor die darauf folgende Zeile gehängt. Dies ist der Effekt,
       kein "Feature": Die Eingabe darf keine '\0'en enthalten.

       Um die Implementation zu verstehen, ist es nützlich sich vor
       Augen zu halten, dass alle Zeilen immer mindestens ein Zeichen
       enthalten ('\n' für alle Zeilen bis auf die letzte, die
       entweder nichtexistent ist, oder eben ein anderes Zeichen
       enthält).
            
     RÜCKGABEWERTE:

       n>1  wenn die Zeile gelesen wurde.
       n<1  wenn EOF erreicht wurde (Zeile leer!).
       0    im Fehlerfall.
  */


  size_t bytes_read;
  size_t free_bytes;
  int    line_complete = 0;

  char*  tmp_buf;
  size_t tmp_size;

  assert(!ferror(s->file));             /* client procedure is not allowed to call 
                                           read_line any more after errors */
  
  tmp_buf         = s->old_buf;         /* old_buf und buf austauschen => */
  s->old_buf      = s->buf;             /* aktuelle Zeile sichern.        */
  s->buf          = tmp_buf;

  tmp_size        = s->old_buf_size;
  s->old_buf_size = s->buf_size;
  s->buf_size     = tmp_size;

  s->current_line++;                   
  s->current_char = 0;                  /* Eingabezeiger auf erstes Zeichen setzen */

  s->buf_fill = 0;                      /* Puffer rücksetzen <=> "leer"            */
  s->buf[0]   = '\0';
  bytes_read  = 0;


  if (feof(s->file)) { return -1; };    /* Bei EOF war's das schon */


  while (!(line_complete || feof(s->file) || ferror(s->file))) { 

    /*  Solange Zeile unvollständig, kein Dateiende und 
	kein Dateifehler, lies ein weiteres Fragment ...
    */
        
    free_bytes = ensure_free_bytes(s, BUF_INCREMENT);

    assert(free_bytes>0);    /* if this is triggered: indicates internal error/bug */

    fgets(s->buf + bytes_read, s->buf_size - bytes_read, s->file);

    bytes_read    = strlen(s->buf);
    s->buf_fill   = bytes_read;

    line_complete = (bytes_read>0) && s->buf[bytes_read-1] == '\n';
  }

  if (ferror(s->file)) return 0;          /* Fehlerbedingung                      */
  if (!bytes_read) { return -1; }         /* Zeigt Leserversuch nach Dateiende an */

  if (!(line_complete)){    /* Ab hier: Fragment vollständig, aber
  		               Zeile unvollständig, d.h.  Dateiende
  		               ist erreicht, aber wir haben noch Daten
  		               im Puffer => In diesem Durchlauf noch
  		               kein EOF signalisieren, aber
  		               stillschweigend fehlendes '\n'
  		               anhängen. */
					    
    ensure_free_bytes(s, 1);
    s->buf[s->buf_fill] = '\n'; 
    s->buf_fill++;
    s->buf[s->buf_fill] = '\0';
  }

  return 1;               
}



input_symbol
input_next_symbol(input_source* s){

  /* Nächstes Symbol aus der Eingabe holen: Wenn Pufferende erreicht,
     vorher Puffer neu füllen, wenn das nicht mehr klappt: EOF zurück
     oder Fehler signalisieren. */

  if (end_of_buffer(s)){ 

    if ( !read_line(s) ) { 
      panic_syserror("error reading from input file","input_next_symbol()");};
    
  } else {

    s->current_char++;
  }

  return input_current_symbol(s);
}



void
input_source_forward(input_source* s, int n){

  /* Setzt Eingabezeiger um n Zeichen vorwärts (indem n Zeichen
     überlesen werden) */

  input_symbol sym;

  while(n>0){

    if (input_is_eof(sym)) { 
      panic("trying to forward beyond end of input", "input_source_forward()"); }
    
    sym = input_next_symbol(s); 
    n--;    
  }
}


long
input_strtol(input_source* s){                          /* s. input.h */
  
  long  n ;
  char* startptr = s->buf + s->current_char;
  char* endptr;
  
  n = strtol(startptr, &endptr, 10);

  assert(endptr!=startptr);
  
  input_source_forward(s, endptr-startptr-1);
  return n;
}


int
input_is_prefix(input_source* s, char* p){               /* s. input.h */

  return strisprefix(p, s->buf + s->current_char);
}



int
input_fprint_line(FILE* f, input_source* s, int line){   /* s. input.h */

  int r;

  assert( (line == s->current_line) || ((line+1) == s->current_line));

  if ((s->buf_fill==0 && line == s->current_line)) { 
    return fprintf(f,"EOF\n"); };

  if ( line == s->current_line ) r = fprintf(f, s->buf);
  else                           r = fprintf(f, s->old_buf);


  return r;
}
