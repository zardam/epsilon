# You can override those settings on the command line

PLATFORM ?= device
DEBUG ?= 0

EPSILON_VERSION ?= 11.1.0
EPSILON_ONBOARDING_APP ?= 0
# Valid values are "none", "update", "beta"
EPSILON_BOOT_PROMPT ?= none
#EPSILON_APPS ?= hello calculation graph khicas statistics probability solver sequence settings # check that SFLAGS += -DGIAC_LINKED is active
EPSILON_APPS ?= hello graph code statistics probability solver sequence calculation settings # check that SFLAGS += -DGIAC_LINKED is inactive
# regression disabled, requires too much RAM
EPSILON_I18N ?= en fr es de pt
EPSILON_GETOPT ?= 0
MATRICES_ARE_DEFINED ?=1
ESCHER_LOG_EVENTS_BINARY ?= 0

include scripts/defaults.mak
include scripts/platform.$(PLATFORM).mak
ifndef USE_LIBA
  $(error platform.mak should define USE_LIBA)
endif
ifndef EXE
  $(error platform.mak should define EXE, the extension for executables)
endif
include scripts/toolchain.$(TOOLCHAIN).mak

#SFLAGS += -DMALLOC_APPS
SFLAGS += -DDEBUG=$(DEBUG)
SFLAGS += -DEPSILON_ONBOARDING_APP=$(EPSILON_ONBOARDING_APP)
SFLAGS += -DEPSILON_GETOPT=$(EPSILON_GETOPT)
EPSILON_BETA_PROMPT := 1
EPSILON_UPDATE_PROMPT := 2
SFLAGS += -DEPSILON_BETA_PROMPT=$(EPSILON_BETA_PROMPT)
SFLAGS += -DEPSILON_UPDATE_PROMPT=$(EPSILON_UPDATE_PROMPT)
ifeq (beta,$(EPSILON_BOOT_PROMPT))
SFLAGS += -DEPSILON_BOOT_PROMPT=$(EPSILON_BETA_PROMPT)
endif
ifeq (update,$(EPSILON_BOOT_PROMPT))
SFLAGS += -DEPSILON_BOOT_PROMPT=$(EPSILON_UPDATE_PROMPT)
endif
SFLAGS += -DMATRICES_ARE_DEFINED=$(MATRICES_ARE_DEFINED)
SFLAGS += -DESCHER_LOG_EVENTS_BINARY=$(ESCHER_LOG_EVENTS_BINARY)

# giac
#SFLAGS += -DGIAC_LINKED
ifeq ($(PLATFORM),device)
SFLAGS += -DGIAC_NUMWORKS -DDEVICE
# next line for building with tommath (spares about 14K)
LDFLAGS += -Lgiac-1.5.0/src -Llibtommath-0.39 -lgiac -ltommath
# next line for building with MPFI/MPFR/GMP
#LDFLAGS += -Lgiac-1.5.0/src -Llib -lgiac -lmpfi -lmpfr -lgmp
endif

ifeq ($(PLATFORM),emscripten)
SFLAGS += -DGIAC_NUMWORKS
LDFLAGS += -O3 -v -s TOTAL_MEMORY=1GB -s LEGACY_GL_EMULATION=1 -s GL_UNSAFE_OPTS=0 --memory-init-file 0 -s ASSERTIONS=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -L/home/parisse/emgiac/giac  -lgiac -lpari -lmpfi -lmpfr -lgmp -lglpk --js-library /home/parisse/emgiac/giac/time.js
endif

ifeq ($(PLATFORM),simulator)
SFLAGS += -DGIAC_NUMWORKS -DSIMULATOR
LDFLAGS += -lgiac
endif
