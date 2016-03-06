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

/*  smalloc.c -- Wrapper um die 'malloc'-Bibliothek, die statt
    Nullzeiger zurückzugeben die Anwendung via panic() terminieren.

    Erläuterungen in 'smalloc.h'.
*/


#include <stdlib.h>
#include "panic.h"
#include "smalloc.h"
#include <string.h>

void*
smalloc (size_t size ) {

  void* ptr;
  
  ptr = malloc(size);
  if (!ptr) panic_syserror("can't allocate requested memory", "smalloc()");

# ifndef NDEBUG
  memset(ptr, 255, size);
# endif

  return ptr;
}


void
sfree (void *ptr ) {

  free(ptr);
}


void *
srealloc(void *ptr, size_t size){
  
  void* new_ptr;

  new_ptr = realloc(ptr, size);          
  if (!new_ptr) panic_syserror("can't allocate requested memory", "srealloc()");
  
  return new_ptr;
}



void *
sresizemem(void *ptr, size_t size, size_t old_size){
  
  char* new_ptr;

  new_ptr = realloc(ptr, size);          
  if (!new_ptr) panic_syserror("can't allocate requested memory", "srealloc()");

# ifndef NDEBUG
  memset(new_ptr+old_size, 255, size-old_size);
# endif
  
  return new_ptr;
}
