@echo off
cls

cl /Ox matrixmult.cpp -D_USING_V110_SDK71_ -DSUBSYSTEM_CONSOLE /link /FILEALIGN:512 /OPT:REF /OPT:ICF /INCREMENTAL:NO /subsystem:console,5.01 user32.lib /out:matrixmult.exe
