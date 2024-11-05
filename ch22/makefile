include ../makefile.inc

BIN = block_cnt sig_order sig_speed_sigwaitinfo

all: ${BIN} sig_sysv.o

clean:
	${RM} *.o ${BIN}

${BIN}: ${TLPI_EXERCISE_LIB}
