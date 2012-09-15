rem This script exists to be called from a pre-build rule in Visual Studio on Windows
rem May also be useful in Eclipse on Windows
rem The first argument is the path to a MinGW installation
rem The second argument is the name of the build rule to execute
set RP=%1
set PATH=%RP%\msys\1.0\bin;%RP%\bin;%PATH%
make %2
