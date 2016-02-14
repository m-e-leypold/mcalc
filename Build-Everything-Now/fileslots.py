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

# TODO: add tmpfile method (patterhn stem~label~tmp)

import framework
import re
import os

class Filetype ( type ):
    
    def __init__( cls, *pargs, **opts ):
        type.__init__(cls,*pargs,**opts)
        framework.register_filetype(cls)
        if hasattr(cls,'rx'):
            cls.name_rx = re.compile(cls.rx + '$')
            cls.members = []
            cls.index   = {}
            
    @property
    def stems(cls): return { m.stem  for m in cls.members }

    @property
    def paths(cls): return { m.path  for m in cls.members }
    
    def get_all_with_stems( cls, stems ):
        r = []
        for s in stems:
            r.append(cls.index[s])
        return r

class Fileslot ( object, metaclass=Filetype ):

    product_only = False

    @classmethod
    def new ( cls, path ):
        return framework.get_file( path, cls, False )
    
    @classmethod
    def create ( cls, path, auto = True ):

        if auto and cls.product_only: return None
        
        if not (hasattr(cls,'name_rx')):            
            return None
        
        m = cls.name_rx.match(path)
        if not m: return None

        obj = cls(path)
        cls.members.append(obj)
        cls.index[obj.stem] = obj
                
        return obj
    
    def __init__( self, path ):
        self.path = path
        m  = self.name_rx.match(path)
        gs = m.groups()
        if    len(gs)>0: self.stem = gs[0]
        else: self.stem = path
        
        framework.register_file(self)        

    def __repr__(self):
        if hasattr(self,'name'): name = self.name
        else: name = self.__class__.__module__+'.'+self.__class__.__name__
        return "<"+name+": "+self.path+">"

    def __str__(self):
        return self.path

    def getctime(self):
        return os.path.getctime(self.path)

    def exists(self):
        return os.path.isfile(self.path)
    
    def is_newer(self,file):
        if not self.exists(): return False
        if not file.exists(): return False    
        return ( self.getctime()>file.getctime() )

class Editorbackup ( Fileslot ):
    rx = "(.*)(~)"

