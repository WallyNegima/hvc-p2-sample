----------------------------------------------------
 B5T-007001 Sample Code (STBLib usage version)
----------------------------------------------------

(1) Contents
  This contains sample code for the detection process, the registration process and the recognition process.

  The detection process for the B5T-007001 will execute 9 functions except recognition and will output the result
  in standard output.
  The registration process for the B5T-007001 will execute the face recognition registration function
  and output the result in standard output.
  The recognition process for the B5T-007001 will execute recognition function and output the result 
  in standard output.

  * This sample has added some refinements based on the SampleCode with no STBLib provided accompanying. 
  * This sample executes stabilization for gender/age estimation and recognition.

(2) Directory Structure
      bin/                          Output directory for building 
      platform/                     Building environment at VC10/VC14
      src/
        HVCApi/                     B5T-007001 interface function
          HVCApi.c                    API function
          HVCApi.h                    API function definition
          HVCDef.h                    Struct definition
          HVCExtraUartFunc.h          Definition for external functions called from API function
        STBApi/                     STBLib interface function
          STBWrap.c                   STBLib wrapper function
          STBWrap.h                   STB Lib wrapper function definition 
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
      import/                       Import directory to use STBLib


        
(3) Build method for sample code
  1. Store the STBLib file on import/lib, the include header on import/include.
  2. The sample code is built as to operate on Windows 7.
     It can be compiled and linked with VC10 (Visual Studio 2010 C++) / VC14 (Visual Studio 2015 C++).
  3. Place STBLib DLL file on the same directory with the exe file generated under the bin/ after 
     the compilation.
  4. Detection process sample
     The detection process module can be built with just all the files under the HVCApi directory,
     STBApi directory and all the files under the bmp, uart and Sample directories.
  5. Registration process sample
     The registration process module can be built with just all the files under the HVCApi directory
     and all the files under the bmp, uart, Album and FR_Sample/register_main.c directories.
  6. Recognition process sample
     The recognition process module can be built with all the files under HVCApi directory, STBApi 
     directory, all the files under the bmp,uart,Album, and FR_Sample/FR_main.c.

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

[NOTES ON USAGE]
* This sample code and documentation are copyrighted property of OMRON Corporation  
* This sample code does not guarantee proper operation
* This sample code is distributed in the Apache License 2.0.

----
OMRON Corporation 
Copyright(C) 2014-2017 OMRON Corporation, All Rights Reserved.
