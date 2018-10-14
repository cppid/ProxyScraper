

COMPILER 		= g++
LINKER			= g++

LINKER_FLAGS	= -o
COMPILER_FLAGS	= -Wall -g -O3 -std=c++11 -c -o

DEPENDENCIES	= -lcurl -lpcre

BIN				= bin
SOURCES			= $(shell find src -name '*.cpp')
OBJECTS			= $(SOURCES:%.cpp=%.o)

RM				= rm -vf

all:	${OBJECTS} ${BIN}

${OBJECTS}:
		${COMPILER} ${COMPILER_FLAGS} $@ ${@:%.o=%.cpp}

${BIN}:
		${LINKER} ${LINKER_FLAGS} $@ ${OBJECTS} ${DEPENDENCIES}

clean:
	${RM} ${OBJECTS}
	${RM} ${BIN}