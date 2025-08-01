cd %~dp0
gcc -o ..\sources\main.o -c ..\sources\main.c -I..\sources
gcc -o ..\build\out.exe ..\sources\main.o -lws2_32