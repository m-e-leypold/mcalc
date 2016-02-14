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


import os

# take 1: build translationunits in order of registration (unconditional FTM)
# take 2: switch translate -> update

filetypes        = []
files            = {}
generators       = []
translationunits = []

def register_filetype(t) : filetypes.append(t)
def register_generator(g): generators.append(g)
def register_translationunit(u): translationunits.append(u)

class FileDoublyInstantiated(Exception): pass

def register_file(f)     :
    path = f.path
    if path in files:
        if not f is files[path]: raise FileDoublyInstantiated("file doubly instantiated [internal error]: "+path+".")
    else:
        files[path] = f # check for dups!

class FileTypeCannotBeDetermined(Exception): pass
class ConflictingFileTypes(Exception):
    def __init__(self,path,cls1,cls2):
        super().__init__("conflicting files types for " + path + ": "+str(cls1)+", "+str(cls2)+".")

def get_file( path, cls = None, auto = True ):

    if not cls == None:
        if path in files:
            cls2 = files[path].__class__
            if not cls is cls2: raise ConflictingFileTypes(path,cls,cls2)
            return files[path]
        else:
            return cls(path)
    else:
        if path in files:
            return files[path]
        else:
            obj = None
            for t in ( reversed ( filetypes )):
                obj  = t.create(path,auto)     # give a FileDoublyInstantiated if already exists
                if obj != None: break            
            if (obj == None): raise FileTypeCannotBeDetermined("file type cannot be determined [warning]: " + path + ".")
            return obj

class AutoRun( type ):
    def __init__(cls,*pargs,**opts):
        type.__init__(cls,*pargs,**opts)
        if hasattr(cls,'autorun'):
            if hasattr(cls,'run'):
                if cls.autorun():
                    cls().run()
                
class Project( object, metaclass = AutoRun ):

    def __init__(self):
        if not hasattr(self,'name'): self.name = self.__class__.__name__
        print("=> Project: ",self.name)
    
    @classmethod
    def autorun(cls):
        return cls.__module__ == '__main__'
    
    def run(self):
        self.build()

    def readtree(self):
        for path in os.listdir():
            try:
                get_file(path)
            except FileTypeCannotBeDetermined as e:
                print(str(e))

    def generate(self):
        print("Generating ...")
        for g in generators:
            g.generate()
        print("... OK (generating).")

    def translate(self):
        for u in translationunits:
            u.translate()

    def update(self):
        for u in translationunits:
            u.update()            
        
    def build(self):
        print("Building "+self.name+" ...")
        self.readtree()
        self.generate()
        self.update()
        print("... OK.")
