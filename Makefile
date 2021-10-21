CC=g++
CFLAGS=--std=c++11

TARGETS=alice bob

all: ${TARGETS}

%: %.cpp common.h
	# ${CC} -g ${CFLAGS} $< -o $@ -lprofiler
	${CC} ${CFLAGS} $< -o $@ -pthread

clean:
	rm -f ${TARGETS}
