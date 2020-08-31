@echo off
setlocal enabledelayedexpansion

cd E:\intelFPGA_lite\18.1\hls\examples\csmith_test
::RM resultFile.txt
touch resultFile.txt
ECHO Welcome to HLS Fuzz testing - Intel HLS!

set /p num=Please enter number of tests you would like to run: 
ECHO %num%
SET /A i =1
:loop

IF %i%==%num% GOTO END
	CD E:\intelFPGA_lite\18.1\hls\genTest1000\%i%
	ECHO E:\intelFPGA_lite\18.1\hls\genTest1000\%i%
	CP test.c E:\intelFPGA_lite\18.1\hls\examples\csmith_test\test.cpp
   	CD E:\intelFPGA_lite\18.1\hls\examples\csmith_test
	RM test.txt 
	RM test_Mod.txt
	TOUCH test_Mod.txt
	RM test-x86-64.exe
	RM test-fpga.exe
	RM -rf test-fpga.prj *obj* *obj *prj_name*
   	CP test.cpp test.txt
	GCC modifyMain.c -o modifyMain
   	modifyMain 0 > test_Mod.cpp
	CP test_Mod.cpp test.txt
	modifyMain 1 > test_Mod.cpp
	CP test_Mod.cpp test.txt
   	ECHO Test %i% >> E:\intelFPGA_lite\18.1\hls\examples\csmith_test\resultFile.txt
	timeout 120m ./build.bat test-x86-64 
	IF %errorlevel%==124 GOTO :INCRE
   	ECHO .
 	ECHO Synthesis result: >> E:\intelFPGA_lite\18.1\hls\examples\csmith_test\resultFile.txt
   	timeout 5m ./test-x86-64.exe >> E:\intelFPGA_lite\18.1\hls\examples\csmith_test\resultFile.txt
	IF %errorlevel%==124 GOTO :INCRE
   	ECHO .
	GCC addDirective_c.c -o addDirective
	addDirective
	CP test_Mod.txt test_Mod.cpp
	timeout 120m ./build.bat test-fpga
	IF %errorlevel%==124 GOTO :INCRE
  	ECHO Cosimulation result: >> E:\intelFPGA_lite\18.1\hls\examples\csmith_test\resultFile.txt
 	timeout 20m ./test-fpga.exe >> E:\intelFPGA_lite\18.1\hls\examples\csmith_test\resultFile.txt
	IF %errorlevel%==124 GOTO :INCRE
	Taskkill /im i++.exe /f /T
	Taskkill /im test-fpga.exe /f /T
   	ECHO .  
  	ECHO Test %i% finished!
	SET /a i=%i%+1
GOTO :LOOP


:END
ECHO Finished!
GCC checkResult.c -o checkResult
checkResult > resultCheck.txt
exit

:incre
SET /a i=%i%+1
GOTO :LOOP