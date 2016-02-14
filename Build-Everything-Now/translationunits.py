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
import os

class Generator(object):
    pass

class ClassAsGenerator(type):
    def __init__(cls,*pargs,**opts):
        type.__init__(cls,*pargs,**opts)
        framework.register_generator(cls)
        
    def generate(cls): pass
        
class TranslationUnit( Generator, metaclass = ClassAsGenerator ):

    def __init__( self ):
        framework.register_translationunit(self)

    def __repr__( self ): return "<"+self.__module__+"."+self.__class__.__name__ + " " + self.stem + ">"

    def is_uptodate(self):

        # TODO: This should not go by time stamps, but rather by fingerprints and
        #       memoization. Cache uptodate property per run.

        for input in self.inputs:
            for output in self.outputs:
                if not output.is_newer(input): return False
        return True        

    def update(self):
        if self.is_uptodate():
            print("=> ",self, " is up to date already.")
            return
        self.translate()
    
    def translate(self):
        script = self.template.format(**self.variables)
        print( "+ " + script.replace("\n","\n+ "))

        err = os.system(script)

        if (err): raise Exception("External script failed")
        
        # TODO: Better error checking
