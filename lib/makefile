include ../makefile.inc

OBJECTS=$(patsubst %.c,%.o,$(wildcard *.c))

all: ${TLPI_EXERCISE_LIB}

${TLPI_EXERCISE_LIB}: ${OBJECTS}
	${RM} ${TLPI_EXERCISE_LIB}
	${AR} rs ${TLPI_EXERCISE_LIB} *.o

clean:
	${RM} *.o ${TLPI_EXERCISE_LIB}