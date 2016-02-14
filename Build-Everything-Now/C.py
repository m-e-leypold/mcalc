#
#   Mcalc -- evaluate numerical expressions given in natural language.
#   Copyright (C) 2005-2016  M E Leypold.
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2 of the
#   License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
#   02110-1301 USA.

import framework

from fileslots        import Fileslot
from translationunits import TranslationUnit


class Header(Fileslot):
    rx = "(.*)([.]h)"

class Implementation(Fileslot):
    rx = "(.*)([.]c)"

class Object(Fileslot):
    rx = "(.*)([.]o)"

class Executable(Fileslot):
    rx           = ".*"             # not a good idea!
    product_only = True
    pass

class Module( TranslationUnit ):

    sources   = [ Header, Implementation ]
    generates = [ Object ]
    
    template  = "gcc -c -o {target} {source}"

    # __repr__

    @classmethod
    def generate(cls):
        stems = cls.stems = list(Header.stems.intersection( Implementation.stems ))
        headers         = Header.get_all_with_stems(stems)
        implementations = Implementation.get_all_with_stems(stems)
        cls.members     = list(map(Module,headers,implementations))
        
    def __init__( self, header, implementation ):
        super().__init__()
        
        self.header = header
        self.implementation = implementation
        self.stem = header.stem

        print("=> c module: "+self.stem)

        # TODO: The following operations should perhaps go to a
        #   translation-unit preparation function. I'm not sure about that yet.
        
        self.objectfile = framework.get_file(self.stem + ".o")

    @property
    def variables(self): return dict(target=self.objectfile.path,source=self.implementation.path)

    @property
    def inputs(self): return [ self.implementation, self.header ]

    @property
    def outputs(self): return [ self.objectfile ]
        
class Program( TranslationUnit ):

    sources   = [ Header, Implementation ] # could we use Modules here instead?
    generates = [ Executable ]
    
    template  = "gcc -o {target} {source} {objects}"

    @classmethod
    def generate(cls):
        stems = cls.stems = Implementation.stems.difference( Header.stems )
        sources           = Implementation.get_all_with_stems(stems)
        cls.members       = list(map(cls,sources))

    def __init__( self, source ):
        super().__init__()
        
        self.source = source
        path = self.path   = self.stem = source.stem

        print("=> c program: "+self.stem)

        self.executable = Executable.new( path )
        
    @property
    def variables(self):
        return dict(
            target=self.executable.path,
            source=self.source.path,
            objects=" ".join(Object.paths)
        )

    @property
    def inputs(self): return [ self.source ]

    @property
    def outputs(self): return [ self.executable ]

