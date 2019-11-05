MODEL ?= n0110
USE_LIBA = 0
EXE = elf

BUILD_DIR := $(BUILD_DIR)/$(MODEL)

$(BUILD_DIR)/python/port/port.o: CXXFLAGS += -DMP_PORT_USE_STACK_SYMBOLS=1

include build/platform.device.$(MODEL).mak

# giac
SFLAGS += -DGIAC_NUMWORKS -DDEVICE
LDFLAGS += -Lext/$(MODEL)/lib -lgiac -lmpfi -lmpfr -lgmp
