IF EXIST C:\dev\BlueFramework\ (set BLUEFRAMEWORK_ROOT_DIR=C:\dev\BlueFramework\)
IF EXIST E:\dev\BlueFramework\ (set BLUEFRAMEWORK_ROOT_DIR=E:\dev\BlueFramework\)

IF EXIST C:\build\vs2017\x64\IfcPL\Transpiler\Debug\Transpiler.exe (set TRANSPILER_PATH=C:\build\vs2017\x64\IfcPL\Transpiler\Debug\Transpiler.exe)
IF EXIST C:\build\vs2017\x64\Transpiler\Debug\Transpiler.exe (set TRANSPILER_PATH=C:\build\vs2017\x64\Transpiler\Debug\Transpiler.exe)

IF EXIST C:\dev\IfcPL\Framework (set IFCPL_FRAMEWORK=C:\dev\IfcPL\Framework)
IF EXIST E:\dev\IfcPL\Framework (set IFCPL_FRAMEWORK=E:\dev\IfcPL\Framework)

IF EXIST C:\dev\IfcPL\examples\Abutment.ifcpl (set IFCPL_SOURCE_FILE=C:\dev\IfcPL\examples\Abutment.ifcpl)
IF EXIST E:\dev\IfcPL\examples\Abutment.ifcpl (set IFCPL_SOURCE_FILE=E:\dev\IfcPL\examples\Abutment.ifcpl)

IF EXIST C:\build\vs2017\x64\tmp\Abutment rd /s /q C:\build\vs2017\x64\tmp\Abutment
mkdir C:\build\vs2017\x64\tmp\Abutment
robocopy %IFCPL_FRAMEWORK% C:\build\vs2017\x64\tmp\Abutment
robocopy %IFCPL_FRAMEWORK%\CMake C:\build\vs2017\x64\tmp\Abutment\CMake
%TRANSPILER_PATH% %IFCPL_SOURCE_FILE% -o C:\build\vs2017\x64\tmp\Abutment
cmake -G"Visual Studio 15 2017 Win64" ^
-HC:\build\vs2017\x64\tmp\Abutment ^
-BC:\build\vs2017\x64\tmp\Abutment\build ^
-DEIGEN3_INCLUDE_DIR="C:\thirdparty\vs2017\x64\Eigen_3.3.4" ^
-DBOOST_ROOT="C:\thirdparty\vs2017\x64\boost_1_65_1" ^
-DCMAKE_PREFIX_PATH="C:\Qt\5.10.1\msvc2017_64" ^
-DBlueFramework_DIR=%BLUEFRAMEWORK_ROOT_DIR%
C:
cd C:\build\vs2017\x64\tmp\Abutment\build
cmake --build . --config Debug
cd Debug
REM cls
Abutment
cd C:\dev\IfcPL\examples