# Program wide settings
EXE       := rubik
EXEC      := RUBIK
RUBIK_VERSION   := 0
RUBIK_SUBLEVEL  := 1
RUBIK_PATCH     := 0
RUBIK_VERSION_N := $(RUBIK_VERSION).$(RUBIK_SUBLEVEL).$(RUBIK_PATCH)

RUBIK_LIBFLAGS := -pthread -lm
RUBIK_CFLAGS  += -I'./include'                             \
				  -Wall -Wextra -Wno-unused-parameter      \
					-DRUBIK_VERSION=$(RUBIK_VERSION)       \
					-DRUBIK_SUBLEVEL=$(RUBIK_SUBLEVEL)     \
					-DRUBIK_PATCH=$(RUBIK_PATCH)           \
					-DRUBIK_VERSION_N="$(RUBIK_VERSION_N)"

RUBIK_OBJS += ./rubik.o

