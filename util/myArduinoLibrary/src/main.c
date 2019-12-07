void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}

// compile this project
// go to .pio/build/uno/src
// open folder in terminal
// avr-ar rcs libmyArduinoLib.a myADC.c.o myGlobalDefines.c.o myLOG.c.o myTWI.c.o myUSART.c.o --plugin=$(gcc --print-file-name=liblto_plugin.so)
// check content of library with
// ar -cvq libmylib.a *.o
// ar -t libmylib.a
//
// To use the library, insert the following line to platformio.ini file
// build_flags = -L/home/michael/GitHubRepositories/Arduino/util/myArduinoLibrary/.pio/build/uno/src -lmyArduinoLib -I/home/michael/GitHubRepositories/Arduino/util/myArduinoLibrary/include