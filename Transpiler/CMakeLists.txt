cmake_minimum_required(VERSION 3.9.1)
project(IfcPLTranspiler)

if (WIN32)
    set(BISON_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/Tools/win_flex_bison-2.5.5/win_bison.exe)
    set(FLEX_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/Tools/win_flex_bison-2.5.5/win_flex.exe)
endif()

find_package(BISON)
find_package(FLEX)

set(LLVM_DIR CACHE FILEPATH "BlueFramework-deps root dir")

macro (link_against_llvm target)
    set (LLVM_LIBS
            LLVMAArch64AsmParser
            LLVMAArch64AsmPrinter
            LLVMAArch64CodeGen
            LLVMAArch64Desc
            LLVMAArch64Disassembler
            LLVMAArch64Info
            LLVMAArch64Utils
            LLVMAMDGPUAsmParser
            LLVMAMDGPUAsmPrinter
            LLVMAMDGPUCodeGen
            LLVMAMDGPUDesc
            LLVMAMDGPUInfo
            LLVMAMDGPUUtils
            LLVMAnalysis
            LLVMARMAsmParser
            LLVMARMAsmPrinter
            LLVMARMCodeGen
            LLVMARMDesc
            LLVMARMDisassembler
            LLVMARMInfo
            LLVMAsmParser
            LLVMAsmPrinter
            LLVMBitReader
            LLVMBitWriter
            LLVMBPFAsmPrinter
            LLVMBPFCodeGen
            LLVMBPFDesc
            LLVMBPFInfo
            LLVMCodeGen
            LLVMCore
            #LLVMCppBackendCodeGen
            #LLVMCppBackendInfo
            LLVMDebugInfoDWARF
            LLVMDebugInfoPDB
            LLVMExecutionEngine
            LLVMHexagonCodeGen
            LLVMHexagonDesc
            LLVMHexagonDisassembler
            LLVMHexagonInfo
            LLVMInstCombine
            LLVMInstrumentation
            LLVMInterpreter
            #LLVMipa
            LLVMipo
            LLVMIRReader
            LLVMLibDriver
            LLVMLineEditor
            LLVMLinker
            LLVMLTO
            LLVMMC
            LLVMMCDisassembler
            LLVMMCJIT
            LLVMMCParser
            LLVMMipsAsmParser
            LLVMMipsAsmPrinter
            LLVMMipsCodeGen
            LLVMMipsDesc
            LLVMMipsDisassembler
            LLVMMipsInfo
            LLVMMIRParser
            LLVMMSP430AsmPrinter
            LLVMMSP430CodeGen
            LLVMMSP430Desc
            LLVMMSP430Info
            LLVMNVPTXAsmPrinter
            LLVMNVPTXCodeGen
            LLVMNVPTXDesc
            LLVMNVPTXInfo
            LLVMObjCARCOpts
            LLVMObject
            LLVMOption
            LLVMOrcJIT
            LLVMPasses
            LLVMPowerPCAsmParser
            LLVMPowerPCAsmPrinter
            LLVMPowerPCCodeGen
            LLVMPowerPCDesc
            LLVMPowerPCDisassembler
            LLVMPowerPCInfo
            LLVMProfileData
            LLVMRuntimeDyld
            LLVMScalarOpts
            LLVMSelectionDAG
            LLVMSparcAsmParser
            LLVMSparcAsmPrinter
            LLVMSparcCodeGen
            LLVMSparcDesc
            LLVMSparcDisassembler
            LLVMSparcInfo
            LLVMSupport
            LLVMSystemZAsmParser
            LLVMSystemZAsmPrinter
            LLVMSystemZCodeGen
            LLVMSystemZDesc
            LLVMSystemZDisassembler
            LLVMSystemZInfo
            LLVMTableGen
            LLVMTarget
            LLVMTransformUtils
            LLVMVectorize
            LLVMX86AsmParser
            LLVMX86AsmPrinter
            LLVMX86CodeGen
            LLVMX86Desc
            LLVMX86Disassembler
            LLVMX86Info
            LLVMX86Utils
            LLVMXCoreAsmPrinter
            LLVMXCoreCodeGen
            LLVMXCoreDesc
            LLVMXCoreDisassembler
            LLVMXCoreInfo
    )

    foreach (lib ${LLVM_LIBS})
        target_link_libraries (${target}
            debug       ${LLVM_DIR}/Debug/lib/${lib}.lib
            optimized   ${LLVM_DIR}/Release/lib/${lib}.lib
        )
    endforeach ()
endmacro ()

BISON_TARGET(Parser ${CMAKE_CURRENT_SOURCE_DIR}/src/OpenInfraPlatform/IfcPL/parser.y  ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
FLEX_TARGET(Scanner ${CMAKE_CURRENT_SOURCE_DIR}/src/OpenInfraPlatform/IfcPL/tokens.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories( ${LLVM_DIR}/include)

file(GLOB OpenInfraPlatform_IfcPL_Source 				src/OpenInfraPlatform/IfcPL/*.*)

source_group(OpenInfraPlatform\\IfcPL\\					FILES ${OpenInfraPlatform_IfcPL_Source})

include_directories(
    src
)

add_executable(Transpiler
    ${OpenInfraPlatform_IfcPL_Source}
    ${FLEX_Scanner_OUTPUTS} 
    ${BISON_Parser_OUTPUTS}
)

link_against_llvm(Transpiler)