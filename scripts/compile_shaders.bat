@ECHO OFF
SETLOCAL

REM // Copyright (C) Chad McKinney - All Rights Reserved
REM // Unauthorized copying of this file, via any medium is strictly prohibited
REM // Proprietary and confidential

if exist glslangValidator.exe (
    goto convert
)
for %%X in (glslangValidator.exe) do (set glslangValidator=%%~$PATH:X)
if defined glslangValidator (
    goto convert
)

echo Could not find "glslangValidator.exe" file.
goto end

:convert

set folder=data\shaders

if not exist %folder% (
    echo Could not find data\shaders
    goto end
)

pushd %folder%
echo compiling shaders in folder: %folder%

for %%a in (*.vert) do (
    echo Converting the following shader file: %%a
    glslangValidator.exe -V -H -o %%a.spv %%a > %%a.spv.txt
    type %%a.spv.txt
)

for %%a in (*.frag) do (
    echo Converting the following shader file: %%a
    glslangValidator.exe -V -H -o %%a.spv %%a > %%a.spv.txt
    type %%a.spv.txt
)
:end

ENDLOCAL
