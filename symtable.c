
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

/*  symtable.c -- Mcalc 'symtable' implementation.

    Die Reihenfolge der Einträge ist kritisch: Ist das Lexeme eines
    Eintrags a das Präfix des Lexems eines Eintrags b, so muss b vor a
    in die Symboltafel eingertragen werden (der Lexer erkennt den
    _ersten_ passenden Eintrag, nicht, wie es bei Erkennung mit
    regulären Ausdrücken der Fall ist, den längste. S. Bemerkungen in
    lexer.h zu diesem Thema).
*/


#include "symtable.h"


symtable_entry symtable [] = 
  { 
    {"zehn",       LEXER_TOKEN_CLASS_O_TEN,     10  }, 
    {"zwanzig",    LEXER_TOKEN_CLASS_O_TEN,     20  }, 
    {"dreißig",    LEXER_TOKEN_CLASS_O_TEN,     30  }, 
    {"dreissig",   LEXER_TOKEN_CLASS_O_TEN,     30  }, 
    {"vierzig",    LEXER_TOKEN_CLASS_O_TEN,     40  }, 
    {"fünfzig",    LEXER_TOKEN_CLASS_O_TEN,     50  }, 
    {"sechzig",    LEXER_TOKEN_CLASS_O_TEN,     60  }, 
    {"siebzig",    LEXER_TOKEN_CLASS_O_TEN,     70 }, 
    {"achtzig",    LEXER_TOKEN_CLASS_O_TEN,     80  }, 
    {"neunzig",    LEXER_TOKEN_CLASS_O_TEN,     90  }, 

    {"hundert",    LEXER_TOKEN_CLASS_hundert,   100   }, 
    {"tausend",    LEXER_TOKEN_CLASS_tausend,   1000  }, 
    {"millionen",  LEXER_TOKEN_CLASS_millionen, 1000000 }, 
    {"million",    LEXER_TOKEN_CLASS_million,   1000000 }, 



    {"elf",        LEXER_TOKEN_CLASS_ELEVEN_UP, 11  }, 
    {"zwölf",      LEXER_TOKEN_CLASS_ELEVEN_UP, 12  }, 
    {"dreizehn",   LEXER_TOKEN_CLASS_ELEVEN_UP, 13  }, 
    {"vierzehn",   LEXER_TOKEN_CLASS_ELEVEN_UP, 14  }, 
    {"fünfzehn",   LEXER_TOKEN_CLASS_ELEVEN_UP, 15  }, 
    {"sechzehn",   LEXER_TOKEN_CLASS_ELEVEN_UP, 16  }, 
    {"siebzehn",   LEXER_TOKEN_CLASS_ELEVEN_UP, 17  }, 
    {"achtzehn",   LEXER_TOKEN_CLASS_ELEVEN_UP, 18  }, 
    {"neunzehn",   LEXER_TOKEN_CLASS_ELEVEN_UP, 19  }, 

    {"null",       LEXER_TOKEN_CLASS_null,      0   },
    {"eins",       LEXER_TOKEN_CLASS_eins,      1   },
    {"eine",       LEXER_TOKEN_CLASS_eine,      1   },
    {"ein",        LEXER_TOKEN_CLASS_ein,       1   },
    {"zwei",       LEXER_TOKEN_CLASS_O_ONE,     2   },
    {"drei",       LEXER_TOKEN_CLASS_O_ONE,     3   },
    {"vier",       LEXER_TOKEN_CLASS_O_ONE,     4   },
    {"fünf",       LEXER_TOKEN_CLASS_O_ONE,     5   },
    {"sechs",      LEXER_TOKEN_CLASS_O_ONE,     6   },
    {"sieben",     LEXER_TOKEN_CLASS_O_ONE,     7   },
    {"acht",       LEXER_TOKEN_CLASS_O_ONE,     8   },
    {"neun",       LEXER_TOKEN_CLASS_O_ONE,     9   },

    {"und",        LEXER_TOKEN_CLASS_und,       0   },

    {"geteilt",    LEXER_TOKEN_CLASS_geteilt,   0   },
    {"durch",      LEXER_TOKEN_CLASS_durch,     0   },

    {"plus",       LEXER_TOKEN_CLASS_PLUS,      0   },
    {"minus",      LEXER_TOKEN_CLASS_MINUS,     0   },
    {"mal",        LEXER_TOKEN_CLASS_MULT,      0   },

    {"+",          LEXER_TOKEN_CLASS_PLUS,      0   },
    {"-",          LEXER_TOKEN_CLASS_MINUS,     0   },
    {"*",          LEXER_TOKEN_CLASS_MULT,      0   },
    {"/",          LEXER_TOKEN_CLASS_DIV,       0   },
    {"(",          LEXER_TOKEN_CLASS_PAR_OPEN,  0   },
    {")",          LEXER_TOKEN_CLASS_PAR_CLOSE, 0   },

    {"\n",         LEXER_TOKEN_CLASS_EOL,       0   },

    {0,0,0}
  };







