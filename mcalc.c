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

/*  mcalc.h -- Mcalc Hauptmodul. 

    Interpretiert die Kommandozeile, initialisiert "Backend" und
    Parser und ruft die Haupschleife des Parsers auf. Traten während
    der Evaluation Fehler in den verarbeiteten Ausdrücken aus, wird am
    Ende eine Warnung ausgegeben und das Programm mit einem Exit-Code
    von 1 beendet.

    Bei Laufzeitfehlern (kein Speicher, Datei kann nicht geöffnet
    werden etc.) wird das Programm mit einer Fehlermeldung sofort
    abgebrochen.

    EIN/AUSGABE:

      Banner          => Standardausgabe
      Fehlermeldungen => Standardfehlerkanal
      Resultate       => Ausgabedatei (ist Standardausgabe im interaktiven Modus)
      Eingabe         <= Eingabedatei (ist Standardeingabe im interaktiven Modus)

      Resultate bedeute hier auch Fehlermedlungen, die von den
      Fehlermeldungen für den Benutzer zu unterscheiden sind (jeder
      Fehler in den Ausdrücken erzeugt zwei Fehlermeldungen: Eine
      graphische der folgenden Art

         Zeile 6 - zwei fünf
                        ---
                        |Operator erwartet.

      für den Benutzer, und eine einzeilige füe die Ausgabe in die
      Resultatdatei).


    KOMMANDOZEILE:

       Keine Argumente -- Interkativer Modus, Eingabe wird vom der
                          Standardeingabe gelesene, Ergebnisse und
                          Fehler auf Standardausgabe bzw.
                          -fehlerkanal ausgegeben.

      ./mcalc <dateiname>

                       -- Batchmodus: Eingabe wird aus Datei .
		          <dateiname> gelesen und Ergebnisse in
		          <dateiname>.out geschrieben.

    RÜCKGABEWERTE:

      0,   wenn die Eingabe fehlerfrei verarbeitet wurde.
      1,   wenn die Eingabe fehlerhaft war.

      n>1, bei Abbrüchen durch Laufzeitfehler                          
*/



#include <stdio.h>
#include <stdlib.h>

#include "panic.h"
#include "str-utils.h" 

#include "input.h"
#include "lexer.h"
#include "parser.h"


int
main(int argc, char** argv) {

  input_source source;
  lexer        my_lexer;
  parser       my_parser;

  FILE*        f_in;                /* Ein- und Ausgabedatei(en) */
  FILE*        f_out;                  
  
  char*        in_file_name  = 0;
  char*        out_file_name = 0; 



  /* Das Banner wird immer auf dis Standardausgabe ausgegeben.
  */


  printf("%s", /* perhaps only needed for interactive mode' */                 "\n"
	 "MCalc Version 0.0.3, Copyright (C) 2005 Markus E Leypold"            "\n"
	 "MCalc comes with ABSOLUTELY NO WARRANTY; for details see LICENSE."   "\n"
	 "This is free software, and you are welcome to redistribute it"       "\n"
	 "under certain conditions; see LICENSE for details."                  "\n\n" );
  fflush(stdout);


  /* Interpretieren der Kommandozeile, Setzen von Ein- und
     Ausgabedatei f_in und f_out. */
   
  
  if (argc<2) {
    
    f_in  = stdin;
    f_out = stdout;
    
  } else {

    if (argc>2) 
      warning("Ignoring additional argument(s) on commandline", argv[2]);
    
    in_file_name = argv[1];
    
    f_in = fopen(in_file_name,"r");
         if (!f_in) panic_syserror ("Could not open input file",in_file_name);
    
    out_file_name = strappend(in_file_name,".out");

    f_out = fopen(out_file_name,"w");
      if (!f_out)  panic_syserror("Could not open output file", out_file_name);
  }



  /* Initialisierung und Verkettung von Eingabequelle 'source', Lexer
     'my_lexer' und Parser 'my_parser'. */
  
  input_source_init(&source, f_in);
  lexer_init(&my_lexer, &source);
  parser_init(&my_parser, &my_lexer, f_out, stderr);
  
  

  /* Evaluation der Eingabe, "single-shot" modus (nur eine Zeile),
     wenn im interaktiven Modus (d.h. f_in == stdin). */

  parser_evaluate(&my_parser,(f_in == stdin));

  if (my_parser.errors) {

    warning("Errors occured during evaluation", in_file_name? in_file_name: "STDIN");
    exit(1);
  }


  /* Schließen der Dateien, Freigabe von Speicher usw. werden mit der
     Beendigung des Prozesse vom Betriebsystem erledigt. Wir sparen
     uns das deshalb hier. Zur Sicherheit werden jedoch die 
     Ausgabedateies ge'flusht'. */

  fflush(f_in); fflush(stderr); fflush(stdout);

  return 0;
}


