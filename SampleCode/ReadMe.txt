----------------------------------------------------
 B5T-007001 Sample Code
----------------------------------------------------

(1) Contents
  This contains sample code for the detection process, the registration process and the recognition process.

  The detection process for the B5T-007001 will execute 9 functions except recognition and will output the result
  in standard output.
  The registration process for the B5T-007001 will execute the face recognition registration function
  and output the result in standard output.
  The recognition process for the B5T-007001 will execute recognition function and output the result 
  in standard output.

(2) Directory Structure
      src/
        HVCApi/                     B5T-007001 interface function
          HVCApi.c                    API function
          HVCApi.h                    API function definition
          HVCDef.h                    Struct definition
          HVCExtraUartFunc.h          Definition for external functions called from API function
        bmp/                        Function to save bitmap file
          bitmap.c                    Function to save to bitmap file the image from B5T-007001
        uart/                       UART interface function
          uart.c                      UART function for Windows
          uart.h                      UART function definition
        Album/                      Album file save/read function
          Album.c                     Function to I/O album obtained from B5T-007001
        Sample/                     Detection process sample
          main.c                      Sample code
        FR_Sample/                  Registration/Recognition process sample
          register_main.c             Registration process sample code
          FR_main.c                   Recognition process sample code

(3) Build method for sample code
  1. The sample code is built as to operate on Windows 7.
     It can be compiled and linked with VC10 (Visual Studio 2010 C++) / VC14 (Visual Studio 2015 C++).
  2. Detection process sample
     The detection process module can be built with just all the files under the HVCApi directory
     and all the files under the bmp, uart and main directories.
  3. Registration process sample
     The registration process module can be built with just all the files under the HVCApi directory
     and all the files under the bmp, uart and register_main directories.
  4. Recognition process sample
     The recognition process module can be built with all the files under HVCApi directory, 
     all the files under the bmp,uart,Album directories, and FR_Sample/FR_main.c.
     
(4) Execute method for sample code
  It is necessary to specify "COM number" and "baud rate" connected to the B5T-007001 in start-up argument 
  to execute this sample code.

  1. Detection process sample
     sample.exe "COM number" "baud rate"
     e.g.)sample.exe 1 921600
     
  2. Registration process sample
     register.exe "COM number" "baund rate"
     e.g.)register.exe 1 921600
     
  3. Recognition process sample
     FRsample.exe "COM number" "baund rate"
     e.g.)FRsample.exe 1 921600
     
(5) Porting on non-Windows environment
  1. HVCApi
       The code in the directory below is built for versability.
       It can be used as-is in environments that can compile C language.
  2. bitmap.c
       Implement a function that can save bitmap files compatible to the environment used.
       The standard API functions for Windows CreateFile(), WriteFile() and CloseHandle() are used.
  3. uart.c, uart.h
       Implement a UART interface function compatible to the environment used.
       It should include the UART initialization, UART send, UART receive and UART end processes.
  4. main.c, register_main.c, FR_main.c
       A keyboard input function for Windows is used.
         _kbhit(): check for keyboard input
         _getch(): Read input from the console
       Change these functions if required by the operating environment.

[NOTES ON USAGE]
* This sample code and documentation are copyrighted property of OMRON Corporation  
* This sample code does not guarantee proper operation
* This sample code is distributed in the Apache License 2.0.

----
OMRON Corporation 
Copyright(C) 2014-2017 OMRON Corporation, All Rights Reserved.
