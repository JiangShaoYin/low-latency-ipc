CC=g++
CFLAGS=--std=c++11

TARGETS=alice bob

all: ${TARGETS}

%: %.cc common.h
	${CC} ${CFLAGS} $< -o $@ -pthread

clean:
	rm -f ${TARGETS}
