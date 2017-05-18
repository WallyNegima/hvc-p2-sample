----------------------------------------------------
 B5T-007001 Sample Code
----------------------------------------------------

(1)About this informant
  It provides the sample code of B5T-007001(HVC-P2).
    1-1) Sample code to transmit command (execute functions) with the B5T-007001
    1-2) Source code of HVC sensing result stabilizing library (STBLib)
    1-3) Sample code to stabilize sensing result of the B5T-007001 using STBLib

(2) Directory Structure
      SampleCode/         Sample code to transmit command (execute functions) with the B5T-007001 ---(noted above 1-1)
         bin/             Output directory for building
         platform/        Building environment at VC10/VC14
         src/             Source code body
      STBLib/             STBLib kit                                                              ---(noted above 1-2)
         lib/             STBLib body
            doc/          Documents set of STBLib
            bin/          STBLib binary file
            platform/     Building environment at VC10(Visual Studio 2010 C++)/VC14(Visual Studio 2015 C++)
            src/          STBLib  source code body
         SampleCode/      Sample code to stabilize sensing result of the B5T-007001 using STBLib  ---(noted above 1-3)
            bin/          Output directory for building 
            import/       Import directory for using STBLib
            platform/     Building environment at VC10/VC14
            src/          Source code body

* When building STBLib with VC10 / VC14, MFC library is required. Install it in advance.
* Refer bundled documents for more detail.

[NOTES ON USAGE]
* This sample code and documentation are copyrighted property of OMRON Corporation  
* This sample code does not guarantee proper operation
* This sample code is distributed in the Apache License 2.0.
* STBLib is specialized for B5T-007001.
  It will be assumed as agreeing the relevant products "Terms of Use and Disclaimer" to use this sample code.

----
OMRON Corporation
Copyright(C) 2016 OMRON Corporation, All Rights Reserved.

