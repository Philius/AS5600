%PHONY: all

long_dirs := $(wildcard examples/RaspberryPi/*)
dirs := $(patsubst examples/RaspberryPi/%,%,$(long_dirs))
$(info DIRS: ${dirs})
PROGRAMS=
OBJS=
all: $(foreach dir,$(dirs),examples/RaspberryPi/$(dir)/$(dir))

clean:
	rm $(PROGRAMS) $(OBJS) 2>/dev/null

AS5600.o: AS5600.cpp AS5600.h
	g++ -c -O3 AS5600.cpp -o AS5600.o

define PROGRAM_template =
examples/RaspberryPi/$1/$1: AS5600.o examples/RaspberryPi/$1/$1.cpp
	g++ -I ${PWD} -c -O3 examples/RaspberryPi/$1/$1.cpp -o examples/RaspberryPi/$1/$1.o
	g++ AS5600.o examples/RaspberryPi/$1/$1.o -o examples/RaspberryPi/$1/$1 -lwiringPi $(1_deps)
PROGRAMS += examples/RaspberryPi/$1/$1
OBJS     += examples/RaspberryPi/$1/$1.o
endef

$(foreach dir,${dirs},$(eval $(call PROGRAM_template,$(dir))))
