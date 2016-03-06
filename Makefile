#
#   Mcalc/B.E.N - Generic Makefile for C
#   Copyright (C) 2015,2016  M E Leypold.
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


default:: all

debug:
	echo ": $(BUILD-ID)"

PRODUCT = mcalc

# --------------------------------

$(shell echo >&2 -e "\n** Build-Everything-Now - Generic Makefile for C **\n")

LAST-BUILD-ID := $(shell if test -f .var/build-id; then cat .var/build-id; else : ; fi )

ifeq ($(LAST-BUILD-ID),)
BUILD-ID := $(shell mkdir -p .var ; uuidgen > .var/build-id ; cat .var/build-id )
else 
BUILD-ID := $(LAST-BUILD-ID)-DIRTY
endif


# --------------------------------

DEFAULT-IGNORE-DIRS  = .git .var 
DEFAULT-IGNORE-NAMES = .* *~ *.pyc *.o CVS __pycache__
IGNORE-DIRS          = $(DEFAULT-IGNORE-DIRS)  $(EXTRA-IGNORE-DIRS)
IGNORE-NAMES         = $(DEFAULT-IGNORE-NAMES) $(EXTRA-IGNORE-NAMES)
DEFAULT-ACCEPT-NAMES = *.c *.h *.l *.y *.d
ACCEPT-NAMES        ?= $(DEFAULT-ACCEPT-NAMES)
FIND-DEPTH           = 3

ifdef ACCEPT-NAMES
-ACCEPT-NAMES = -a \( -false $(ACCEPT-NAMES:%= -o -name "%") \)
endif

FIND = find . $(FIND-DEPTH:%=-maxdepth %) \( -type d -a -not -path "." -a \( \( -false $(IGNORE-DIRS:%= -o -path "./%") $(IGNORE-NAMES:%= -o -name "%") \) -a -prune \) -a -false \) -o  -type f $(-ACCEPT-NAMES) -a

ifndef SOURCES
SOURCES := $(shell $(FIND) -type f -print)
endif

SOURCES     := $(patsubst ./%,%,$(SOURCES))
SOURCE-DIRS := $(patsubst %/,%,$(sort $(dir $(SOURCES))))

$(shell echo>&2 '=> SOURCE-DIRS: $(SOURCE-DIRS)')

# --------------------------------

COPY! = @mkdir -p $(@D) && cp $< $@

clean:: ACCEPT-NAMES=
clean:: 
	@echo -e '=> Cleaning ...'
	@rm -rf .var/*
	@find . -name '*~' -print | xargs rm -f

all::
	@echo -e '=> Building ...'

# --------------------------------

C-HEADERS      = $(filter %.h,$(SOURCES))
C-SOURCES      = $(filter %.c,$(SOURCES))

C-MODULES      = $(filter $(C-HEADERS:%.h=%),$(C-SOURCES:%.c=%))
C-PROGRAMS     = $(filter-out $(C-MODULES),$(C-SOURCES:%.c=%))

.var/C-OBJECTS = $(C-MODULES:%=.var/src/%.o)
.var/C-DEPS    = $(C-MODULES:%=.var/src/%.mk) $(C-PROGRAMSS:%=.var/src/%.mk)

$(shell echo>&2 "=> C-SOURCES  : $(C-SOURCES)")
$(shell echo>&2 "=> C-PROGRAMS : $(C-PROGRAMS)")
$(shell echo>&2 "=> C-MODULES  : $(C-MODULES)")


# --------------------------------

STAGED-HEADERS := $(C-HEADERS:%=.var/include/$(PRODUCT)/%) # XXX plus staging from generated headers

$(C-HEADERS:%=.var/include/$(PRODUCT)/%): .var/include/$(PRODUCT)/%: %
	$(COPY!)

headers: $(STAGED-HEADERS)


# --------------------------------

deps  : $(.var/C-DEPS)
-include $(.var/C-DEPS)

# .var/src/%.mk: .var/src/%.d
#	@awk <$< >$@ '(NR=1){gsub("$*.o:$$","$*.DEPS=",$$1)}{print}'

# ^ XXX put that into the translation step => avoid restarting

# --------------------------------

# XXX support different includepath models: FUZZY, strictly local, local path, public paths

# check this syntax => XXX s.th. wrong


INCLUDE-FROM        ?= $(FUZZY-INCLUDE-PATHS)
FUZZY-INCLUDE-PATHS := . $(SOURCE-DIRS:%=/$(PRODUCT)/%)
INCLUDE-PATHS        = $(INCLUDE-FROM:%/.=/%)

CPPFLAGS-I = $(patsubst %,-I %,$(INCLUDE-PATHS:/%=.var/include/%))

%.o: %.c
%: %.c
%: %.o

$(.var/C-OBJECTS): .var/src/%.o: .var/src/%.c
	@mkdir -p $(@D)
	@echo "=> CC $(<:.var/src/%=%)  -o $(@:.var/src/%=%)"
	@$(CC) -MMD -c $(CPPFLAGS-I) $(CPPFLAGS) $(CFLAGS) -o $@ -MF $(@:%.o=%.d) $<
	@awk <$(@:%.o=%.d) >$(@:%.o=%.mk) '(NR=1){gsub(".*/$*.o:$$","$*.DEPS =",$$1)}{print}'

$(.var/C-OBJECTS:%.o=%.i): .var/src/%.i: .var/src/%.c 
	@mkdir -p $(@D)
	@echo "=> CC -E $(<:.var/src/%=%)  -o $(@:.var/src/%=%)"
	@$(CC) -E -c $(CPPFLAGS-I) $(CPPFLAGS) $(CFLAGS) -o $@ $<

define per-module-rules
ifndef $1.DEPS
$1.DEPS := $$(STAGED-HEADERS)
endif
.var/src/$1.o:  $$($1.DEPS)
.var/src/$1.i:  $$($1.DEPS)
endef

$(foreach MODULE,$(C-MODULES),$(eval $(call per-module-rules,$(MODULE))))

STAGED-SOURCES := $(C-SOURCES:%=.var/src/%) # XXX plus staging from generated source

$(STAGED-SOURCES): .var/src/%.c: %.c
	$(COPY!)


# XXX need to fix the include-paths => different models?

sources::     headers 
modules::     $(.var/C-OBJECTS)
preprocess::  $(.var/C-OBJECTS:%.o=%.i)


# --------------------------------

LIBS          = $(filter lib%,$(notdir $(SOURCE-DIRS)))
LIBS-OBJPOOL := $(.var/C-OBJECTS)

define per-lib-rules
ifndef $(1)_OBJS
$(1)_PATHS := $$(patsubst ./%,%,$$(filter %/$(1),$$(SOURCE-DIRS:%=./%)))
$(1)_OBJS  := $$(filter $$($(1)_PATHS:%=.var/src/%/%),$$(LIBS-OBJPOOL))
endif
.var/lib/$1.a: $$($(1)_OBJS)
LIBS-OBJPOOL := $$(filter-out $$($(1)_OBJS),$(LIBS-OBJPOOL))
endef

$(foreach LIB,$(LIBS),$(eval $(call per-lib-rules,$(LIB))))
libLOCAL_OBJS := $(LIBS-OBJPOOL)
LIBS += libLOCAL
$(eval $(call per-lib-rules,libLOCAL))

.var/LIBS = $(LIBS:%=.var/lib/%.a)
all:: $(.var/LIBS)

$(.var/LIBS): %.a:
	@mkdir -p "$(@D)"
	@echo "=> AR cr $(@:.var/lib/%=%)  $(^:.var/src/%=%)"
	@ar cr "$@" $^

libs:: $(.var/LIBS)

# --------------------------------

.var/PROGRAMS = $(C-PROGRAMS:%=.var/bin/%)

$(.var/PROGRAMS): .var/bin/%: .var/src/%.c
	@mkdir -p $(@D)
	@echo "=> CC $(<:.var/src/%=%) $(filter %.o,$(^:.var/src/%=%)) $(filter %.a,$(^:.var/lib/%=%)) -o $(@:.var/bin/%=%)"
	@$(CC) -MMD $(CPPFLAGS-I) $(CPPFLAGS) $(CFLAGS) -o $@ -MF $(<:%.c=%.d) $< $(filter %.o,$^) $(filter %.a,$^)
	@awk <$(<:%.c=%.d) >$(<:%.c=%.mk) '(NR=1){gsub(".*/$*:$$","$*.DEPS =",$$1)}{print}'

define per-program-rules
ifndef $(1).DEPS
$(1).DEPS := $$(STAGED-HEADERS)
endif
ifndef $(1).LIBS
$(1).LIBS := $$(.var/LIBS)
endif
.var/bin/$1: $$($(1).DEPS) $$($(1).LIBS)
endef

$(foreach PROGRAM,$(C-PROGRAMS),$(eval $(call per-program-rules,$(PROGRAM))))

programs:: $(.var/PROGRAMS)
all::      $(.var/PROGRAMS)

# --------------------------------

IGNORE-SOURCES-FOR-INSTALL = *.d

install::
	@echo -e "=> Installing ..."
	@rm -rf .var/STAGE
	@mkdir -p .var/STAGE
	@echo "=> CP lib/"
	@cp -r .var/lib .var/STAGE/lib
	@echo "=> CP bin/"
	@cp -r .var/bin .var/STAGE/bin
	@echo "=> CP include/"
	@cp -r .var/include .var/STAGE/include
	@echo "=> FIND src/"
	@mkdir -p .var/STAGE/src
	@set -e -u; cd .var/src ; $(FIND) -not -name '*.d' -print | xargs tar -cf- | tar -C ../STAGE/src -xf-
	@echo "=> CHMOD"
	@chmod -R g-w,o-w,ugo+X .var/STAGE
	@echo "=> BUILD-ID: $(BUILD-ID)"
	@echo "$(BUILD-ID) $(PRODUCT)" >.var/STAGE/BUILD-ID

package::
	@echo -e "=> Packaging ..."
	@echo "=> TAR -f $(PRODUCT)_$(BUILD-ID).tar.gz"
	@mkdir -p .var/dist
	@tar -C .var/STAGE -czf .var/dist/$(PRODUCT)_$(BUILD-ID).tar.gz .

# --------------------------------

all::
	@echo -e "=> OK.\n"

install::
	@echo -e "=> OK.\n"


clean::
	@echo -e "=> OK.\n"

package::
	@echo -e "=> OK.\n"


$(shell echo >&2)
