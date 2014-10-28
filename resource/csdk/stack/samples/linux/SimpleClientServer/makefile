# override with `make BUILD=release`
# default to debug build
BUILD	 := release
PLATFORM := linux
CC	 := g++
OUT_DIR	  := ./$(BUILD)
OBJ_DIR	  := $(OUT_DIR)/obj

ifeq ($(ROOT_DIR), )
ROOT_DIR	= ./../../../..
endif

OUT_DIR	= .

OCLOGGER_DIR	= $(ROOT_DIR)/logger
OCRANDOM_DIR	= $(ROOT_DIR)/ocrandom
OCSOCKET_DIR	= $(ROOT_DIR)/ocsocket
LCOAP_DIR		= $(ROOT_DIR)/libcoap-4.1.1
OCCOAP_DIR		= $(ROOT_DIR)/occoap
OCTBSTACK_DIR	= $(ROOT_DIR)/stack

OCLOGGER_INC	= $(OCLOGGER_DIR)/include
OCRANDOM_INC	= $(OCRANDOM_DIR)/include
OCSOCKET_INC	= $(OCSOCKET_DIR)/include
LCOAP_INC		= $(LCOAP_DIR)
OCCOAP_INC		= $(OCCOAP_DIR)/include
OCTBSTACK_INC	= $(OCTBSTACK_DIR)/include

INC_DIRS	:= -I$(OCLOGGER_INC)
INC_DIRS	+= -I$(OCRANDOM_INC)
INC_DIRS	+= -I$(OCSOCKET_INC)
INC_DIRS	+= -I$(LCOAP_INC)
INC_DIRS	+= -I$(OCCOAP_INC)
INC_DIRS	+= -I$(OCTBSTACK_INC)

CC_FLAGS.debug      := -O0 -g3 -Wall -ffunction-sections -fdata-sections -fno-exceptions \
                        -std=c++0x -pedantic $(INC_DIRS) -L$(ROOT_DIR)/$(BUILD) -DTB_LOG
CC_FLAGS.release    := -Os -Wall -fdata-sections -Wl,--gc-sections -Wl,-s -fno-exceptions \
                        -std=c++0x $(INC_DIRS) -L$(ROOT_DIR)/$(BUILD) -DTB_LOG

LDLIBS		+= -loctbstack -lpthread
CPPFLAGS	+= $(CC_FLAGS.$(BUILD)) $(LDLIBS)

SOURCES     := occlient.cpp ocserver.cpp occlientcoll.cpp ocservercoll.cpp common.cpp

OBJECTS:= $(patsubst %.c, $(OBJ_DIR)/%.o, $(SOURCES))

PROGRAMS	+= ocserver
PROGRAMS	+= occlient
PROGRAMS	+= ocservercoll
PROGRAMS	+= occlientcoll

all:	prep_dirs $(OBJECTS) $(PROGRAMS)

prep_dirs:
	-mkdir -p $(OUT_DIR)
	-mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

ocserver: $(OBJ_DIR)/ocserver.o $(OBJ_DIR)/common.o
	$(CC) $^ $(CPPFLAGS) -o $(OUT_DIR)/$(BUILD)/$@

ocservercoll: $(OBJ_DIR)/ocservercoll.o $(OBJ_DIR)/common.o
	$(CC) $^ $(CPPFLAGS) -o $(OUT_DIR)/$(BUILD)/$@

occlient: $(OBJ_DIR)/occlient.o $(OBJ_DIR)/common.o
	$(CC) $^ $(CPPFLAGS) -o $(OUT_DIR)/$(BUILD)/$@

occlientcoll: $(OBJ_DIR)/occlientcoll.o $(OBJ_DIR)/common.o
	$(CC) $^ $(CPPFLAGS) -o $(OUT_DIR)/$(BUILD)/$@

.PHONY: clean

clean: legacy_clean
	-rm -rf release
	-rm -rf debug

legacy_clean:
	rm -f *.o $(PROGRAMS)