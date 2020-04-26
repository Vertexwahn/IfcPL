IF EXIST C:\dev\BlueFramework\ (set BLUEFRAMEWORK_ROOT_DIR=C:\dev\BlueFramework\)
IF EXIST E:\dev\BlueFramework\ (set BLUEFRAMEWORK_ROOT_DIR=E:\dev\BlueFramework\)

IF EXIST C:\build\vs2017\x64\IfcPL\Transpiler\Debug\Transpiler.exe (set TRANSPILER_PATH=C:\build\vs2017\x64\IfcPL\Transpiler\Debug\Transpiler.exe)
IF EXIST C:\build\vs2017\x64\Transpiler\Debug\Transpiler.exe (set TRANSPILER_PATH=C:\build\vs2017\x64\Transpiler\Debug\Transpiler.exe)

IF EXIST C:\dev\IfcPL\Framework (set IFCPL_FRAMEWORK=C:\dev\IfcPL\Framework)
IF EXIST E:\dev\IfcPL\Framework (set IFCPL_FRAMEWORK=E:\dev\IfcPL\Framework)

IF EXIST C:\dev\IfcPL\examples\OperatorOverloading.ifcpl (set IFCPL_SOURCE_FILE=C:\dev\IfcPL\examples\OperatorOverloading.ifcpl)
IF EXIST E:\dev\IfcPL\examples\OperatorOverloading.ifcpl (set IFCPL_SOURCE_FILE=E:\dev\IfcPL\examples\OperatorOverloading.ifcpl)

IF EXIST C:\build\vs2017\x64\tmp\OperatorOverloading rd /s /q C:\build\vs2017\x64\tmp\OperatorOverloading
mkdir C:\build\vs2017\x64\tmp\OperatorOverloading
robocopy %IFCPL_FRAMEWORK% C:\build\vs2017\x64\tmp\OperatorOverloading
robocopy %IFCPL_FRAMEWORK%\CMake C:\build\vs2017\x64\tmp\OperatorOverloading\CMake
%TRANSPILER_PATH% %IFCPL_SOURCE_FILE% -o C:\build\vs2017\x64\tmp\OperatorOverloading
cmake -G"Visual Studio 15 2017 Win64" ^
-HC:\build\vs2017\x64\tmp\OperatorOverloading ^
-BC:\build\vs2017\x64\tmp\OperatorOverloading\build ^
-DEIGEN3_INCLUDE_DIR="C:\thirdparty\vs2017\x64\Eigen_3.3.4" ^
-DBOOST_ROOT="C:\thirdparty\vs2017\x64\boost_1_65_1" ^
-DCMAKE_PREFIX_PATH="C:\Qt\5.10.1\msvc2017_64" ^
-DBlueFramework_DIR=%BLUEFRAMEWORK_ROOT_DIR%
C:
cd C:\build\vs2017\x64\tmp\OperatorOverloading\build
cmake --build . --config Debug
cd Debug
REM cls
OperatorOverloading
cd C:\dev\IfcPL\examples