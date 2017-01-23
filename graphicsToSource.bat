@echo off

echo #ifndef RESOURCES_H > MonsterGo\Resources.h
echo #define RESOURCES_H >> MonsterGo\Resources.h
echo. >> MonsterGo\Resources.h


echo #ifdef __cplusplus >> MonsterGo\Resources.h
echo extern "C" { >> MonsterGo\Resources.h
echo #endif >> MonsterGo\Resources.h
echo. >> MonsterGo\Resources.h

echo #include "Resources.h" > MonsterGo\Resources.c
echo. >> MonsterGo\Resources.c

for /R %%x in (*.bmp) do (
	
	echo %%~x
	(echo %%~x
	echo MonsterGo\Resources.h
	echo %%~nx) | Converter >> MonsterGo\Resources.c

)

echo. >> MonsterGo\Resources.h
echo #ifdef __cplusplus >> MonsterGo\Resources.h
echo } >> MonsterGo\Resources.h
echo #endif >> MonsterGo\Resources.h
echo. >> MonsterGo\Resources.h

echo #endif >> MonsterGo\Resources.h