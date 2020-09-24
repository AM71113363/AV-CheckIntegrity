# Project: AM71113363
# Makefile created by Dev-C++ 4.9.9.2

CC   = gcc.exe -s
WINDRES = windres.exe
RES  = main.res
OBJ  = main.o md5.o $(RES)
LINKOBJ  = main.o md5.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -mwindows -lcomctl32  
INCS =  -I"C:/Dev-Cpp/include" 
BIN  = "AV.exe"
CFLAGS = $(INCS)  
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before "AV.exe" all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o "AV.exe" $(LIBS)

main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)

md5.o: md5.c
	$(CC) -c md5.c -o md5.o $(CFLAGS)

main.res: main.rc 
	$(WINDRES) -i main.rc --input-format=rc -o main.res -O coff 
