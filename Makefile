default: main.o clockMode.o devices.o counterMode.o textEditorMode.o drawBoardMode.o
	arm-none-linux-gnueabi-gcc -static -o hw1_20161581 main.o clockMode.o devices.o counterMode.o textEditorMode.o drawBoardMode.o
main.o: main.c main.h clockMode.h devices.h counterMode.h
	arm-none-linux-gnueabi-gcc -c main.c -o main.o
clockMode.o: clockMode.c main.h clockMode.h
	arm-none-linux-gnueabi-gcc -c clockMode.c -o clockMode.o
counterMode.o: counterMode.c main.h counterMode.h
	arm-none-linux-gnueabi-gcc -c counterMode.c -o counterMode.o
textEditorMode.o: textEditorMode.c main.h textEditorMode.h
	arm-none-linux-gnueabi-gcc -c textEditorMode.c -o textEditorMode.o
drawBoardMode.o: drawBoardMode.c main.h drawBoardMode.h
	arm-none-linux-gnueabi-gcc -c drawBoardMode.c -o drawBoardMode.o
devices.o: devices.c main.h devices.h
	arm-none-linux-gnueabi-gcc -c devices.c -o devices.o
clean:
	-rm *.o
	-rm hw1_20161581
