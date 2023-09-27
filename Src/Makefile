#CC      = gcc34
CC      = gcc34 -std=gnu99
RM      = rm
EXE     = asm
PROF    =
C_FLAGS = -ggdb -Wall $(PROF) -DMERC
L_FLAGS = -O -lcrypt $(PROF)

SRC_FILES := $(wildcard *.c)
H_FILES   := $(wildcard *.h)
OBJ_DIR    = obj
OBJ_FILES := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

all:	build asm

asm: $(OBJ_FILES)
	  $(RM) -f $(EXE)
	  $(CC) $(L_FLAGS) -o $(EXE) $(OBJ_FILES)
          $(OBJ_DIR)/%.o: %.c
	  $(CC) $< $(C_FLAGS) -c -o $@

build:
	@if [ -f .build ]; then \
	    expr `cat .build` + 1 > .build; \
	else \
	    echo '1' > .build; \
	fi
	@if [ -f .version ]; then \
	    expr `cat .version` + 1 > .version; \
	else \
	    echo '10001' > .version; \
	fi
	@if [ `cat .build` -ge 51 ]; then \
	    echo '1' > .build; \
	fi
	@echo '#ifndef BUILD_H' > build.h
	@echo '#define BUILD_H' >> build.h
	@echo "#define LINKCOUNT   `cat .build`" >> build.h
	@echo '#endif' >> build.h
	@echo '#ifndef VERSION_H' > version.h
	@echo '#define VERSION_H' >> version.h
	@echo "#define VERSION   `cat .version`" >> version.h
	@echo '#endif' >> version.h
	@echo "Build number `cat .build`, version `cat .version` set."

clean:
	$(RM) -f $(OBJ_FILES) $(EXE) *~ *.bak *.orig *.rej

store:
	cp $(SRC_FILES) $(H_FILES) ./backup

archive:
	make clean && cd ../../ && tar vzcf devel-`date -I`.tgz devel -X devel/area/tarxlist
