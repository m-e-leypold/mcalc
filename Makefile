#
#   Mcalc -- evaluate numerical expressions given in natural language.
#   Copyright (C) 2005  M E Leypold.
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


#  Makefile -- This is a GNU-Makefile for the Mcalc project.
#
#  The parametrisation/configuration section follows immediately.
#
#  The other sections of this file are generic rules for programs
#  written in C and keeping to certain "modularity guidelines".



#  PROJECT PARAMETRISATION / CONFIGURATION   -----------------
#
#

default      : taschenrechner mcalc tests

test         : mcalc
	echo; echo; \
	./mcalc test-input-3; ERR=$$?; echo; \
	echo ---; cat test-input-3.out; echo --- ; echo; exit $$ERR


PROJECT-NAME = mcalc

C-PROGRAMS   = mcalc
C-MODULES    = input lexer parser symtable \
               panic smalloc str-utils

TESTS        = test-input-numerals test-input-expressions

mcalc: $(C-MODULES:%=%.o)

CC         = gcc
CPPFLAGS   =
CFLAGS     = -g -Wall -DLDEBUG -ansi -pedantic
LOADLIBES  = 
LDLIBS     = 

%.log: % mcalc
	@echo Running Test: ./mcalc $< 
	@./mcalc 1>"$@" 2>&1 $< || $(FAIL-TEST)

CLEANABLES     = *.out *.log
TEST-PROTOCOLS = $(TESTS:%=%.log) 


taschenrechner: mcalc
	$(COPY) 

PRODUCTS += taschenrechner

# UTILITIES
#

STAMP      := $(shell date -Is)
FAIL        = { rm "$@" ; exit 1; }
FAIL-TEST   = { mkdir -p FAILED-TESTS/ && mv "$@" FAILED-TESTS/"$@;$(STAMP)"; \
                exit 0; }
MDIR@       = mkdir -p $$(dirname "$@")
COPY        = cp $< $@ || $(FAIL)


# MODULAR C RULES   --------------------------------------------
#
#   ...

C-COMPILE = $(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
C-LINK    = $(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)
C-MKDEPS  = $(CC) $(CPPFLAGS) $(CFLAGS) -MM $< | $(C-DEPS-FORMAT) >$@ 
C-LIST    = enscript -r -2 -c -f Courier@7 \
            -j --margins=:40:0:100 -Ec	   \
            -o "$@" $< || $(FAIL)

$(C-MODULES:%=%.o): %.o: %.c %.h
	$(C-COMPILE)

$(C-MODULES:%=%.d): %.d: %.c %.h
	@echo Generating dependencies: $< ...
	@$(C-MKDEPS) || $(FAIL)

$(C-PROGRAMS:%=%.o): %.o: %.c
	$(C-COMPILE)

$(C-PROGRAMS:%=%.d): %.d: %.c
	@echo Generating dependencies: $< ...
	@$(C-MKDEPS) || $(FAIL)

$(C-PROGRAMS): %: %.o
	$(C-LINK)

$(C-MODULES:%=%.h.ps) $(C-MODULES:%=%.c.ps) $(C-PROGRAMS:%=%.c.ps): %.ps: %
	$(C-LIST)

C-DEPS       =  $(C-MODULES:%=%.d) $(C-PROGRAMS:%=%.d)

c-modules    :  $(C-MODULES:%=%.o)
c-programs   :  $(C-PROGRAMS)
c-deps       :  $(C-DEPS)


PHONIES      += c-modules c-programs c-deps
CLEANABLES   += *.o *.c.ps *.h.ps
DEPS         += $(C-DEPS)

PRODUCTS     += $(C-PROGRAMS)
OBJECT-FILES += $(C-MODULES:%=%.o)

LISTINGS     += $(C-PROGRAMS:%=%.c.ps) \
                $(C-MODULES:%=%.c.ps) $(C-MODULES:%=%.h.ps)

# ADMINISTRATIVE AND COLLECTIVE TARGETS   ------------------------
#

.PHONY: all tests tidy clean cleaner test $(PHONIES)

all: $(PRODUCTS)

tests: $(TEST-PROTOCOLS)
	@if ls -1 FAILED-TESTS/*$(STAMP) >/dev/null 2>&1; then \
	    echo                                       ;  \
	    echo '*********************************'   ;  \
	    echo '*                               *'   ;  \
	    echo '***    SOME TESTS FAILED !    ***'   ;  \
	    echo '*                               *'   ;  \
	    echo '*********************************'   ;  \
	    echo                                       ;  \
	    cd FAILED-TESTS && ls -1 *$(STAMP)         ;  \
	    echo                                       ;  \
            fi

listings: $(LISTINGS)

tidy:
	rm -f *~ $(CLEANABLES)
	find . -name "?" -maxdepth 1 -type f | xargs rm -f
	rm -f foobar*
	rm -f ?.*
	rm -f *.d

clean: tidy
	rm -f $(PRODUCTS)
	rm -rf FAILED-TESTS/

cleaner: clean
	rm -f $(DEPS)


CLEANER-PHONIES += cleaner


# VERSION CONTROL HOOKS (for Leypold's Project Shell Bindings)
#

check-versions:
	cvs -nq update

commit-versions:
	cvs commit



# MECHANISM(S) TO AUTOMATE DEPENDENCY-GENERATION   ----------------
#
#

C-DEPS-FORMAT = awk '      	\
                                \
   BEGIN{ BUF="" }    		\
	              		\
   {print $$0}        		\
                                \
   /^[a-zA-Z_]/{                \
                                \
      printf BUF;               \
      BUF = BUF $$0 "\n";       \
      sub(/\.o:/,".d: ",BUF);   \
      next;                     \
   }		                \
                                \
   {                            \
      BUF = BUF $$0 "\n";       \
      next;                     \
   }		                \
                                \
   END{ printf BUF;}            \
'


ifneq ($(filter $(CLEANER-PHONIES),$(MAKECMDGOALS)),cleaner)
      include $(DEPS)
endif

     


BUILD.sh: Makefile $(DEPS)
	$(MAKE) clean
	{                                                              \
	  echo "#    ";                                                \
	  echo "# Automatically generated build script for 'mcalc'.";  \
	  echo "# For use on systems w/o GNU Make";                    \
	  echo "#";                                                    \
	  echo "";                      		\
	  echo "";                      		\
	  echo "set -x" ;               		\
	  $(MAKE) -s -n                                 \
             CC='$$$${CC:-cc}'                          \
             CFLAGS='$$$${CFLAGS:-"-g"}'                \
             LDFLAGS='$$$${LDFLAGS:-}'                  \
             CPPFLAGS='$$$${CPPFLAGS:-}'                \
             LDLIBS='$$$${LDLIBS:-}'                    \
             LOADLIBES='$$$${LOADLIBES:-}'              \
	     all ; \
	} > $@ || $(FAIL) 
	chmod 755 $@

CLEANABLES += BUILD.sh


# GENERATING RELEASE BRANCHES
#
#

release-new:
	LABEL=$$(echo $(STAMP) | date -Is | sed 's|:|-|g;s|+.*||'); \
        LABEL=S-$(PROJECT-NAME)+$$LABEL; \
	$(MAKE) cleaner && \
	cvs tag -b -F $$LABEL && cvs update -r $$LABEL  && \
	$(MAKE) -s BUILD.sh && \
	cvs add $(DEPS) BUILD.sh && cvs commit -m '';
	@echo; echo Snapshot date: $(SPAMP); echo
