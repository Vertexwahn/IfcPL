IF EXIST C:\build\vs2017\x64\tmp\ImportedTypes rd /s /q C:\build\vs2017\x64\tmp\ImportedTypes
mkdir C:\build\vs2017\x64\tmp\ImportedTypes
robocopy C:\dev\IfcPL\Framework C:\build\vs2017\x64\tmp\ImportedTypes
robocopy C:\dev\IfcPL\Framework\CMake C:\build\vs2017\x64\tmp\ImportedTypes\CMake
C:\build\vs2017\x64\IfcPL\Transpiler\Debug\Transpiler.exe C:\dev\IfcPL\examples\ImportedTypes.ifcpl -o C:\build\vs2017\x64\tmp\ImportedTypes
cmake -G"Visual Studio 15 2017 Win64" ^
-HC:\build\vs2017\x64\tmp\ImportedTypes ^
-BC:\build\vs2017\x64\tmp\ImportedTypes\build ^
-DEIGEN3_INCLUDE_DIR="C:\thirdparty\vs2017\x64\Eigen_3.3.4" ^
-DBOOST_ROOT="C:\thirdparty\vs2017\x64\boost_1_65_1" ^
-DCMAKE_PREFIX_PATH="C:\Qt\5.10.0\msvc2017_64" ^
-DBlueFramework_DIR="C:\dev\BlueFramework"
cd C:\build\vs2017\x64\tmp\ImportedTypes\build
cmake --build . --config Debug
cd Debug
REM cls
ImportedTypes
cd C:\dev\IfcPL\examples