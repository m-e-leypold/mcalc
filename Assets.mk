# TODO: adapt predicate names => .is-file?
# TODO: Allow for .Assets.mk 

test-f                  = $(strip $(shell test -f $1 && echo "true" || echo "false"))
test-d                  = $(strip $(shell test -d $1 && echo "true" || echo "false"))
test-fd                 = $(strip $(shell  { test -f $1 || test -d $1 ; } && echo "true" || echo "false"))
have-assets             = $(call test-f,.assets/MK)
using-submodule-assets  = $(call test-d,.assets)

ifdef ASSETS
ifeq  ($(have-assets),false)
ifeq  ($(using-submodule-assets),false)
$(shell set -x; ln -s $(HOME)/.shared-assets/$(ASSETS) .assets 1>&2)
else
$(error SUBMODULES NOT SUPPORTED)
endif
else
$(shell if ! test -f Assets.mk; then set -x ; cp .assets/mklib/bootstrap-assets.mk Assets.mk; fi)
endif
endif


