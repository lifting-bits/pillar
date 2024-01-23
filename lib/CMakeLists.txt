#
# Copyright (c) 2023-present, Trail of Bits, Inc.
# All rights reserved.
#
# This source code is licensed in accordance with the terms specified in
# the LICENSE file found in the root directory of this source tree.
#

set(source_include_dir "${PROJECT_SOURCE_DIR}/include/pillar")


if (MLIR_LINK_MLIR_DYLIB)
  set(MLIR_LIBS MLIR)
else()
  get_property(MLIR_DIALECT_LIBS GLOBAL PROPERTY MLIR_DIALECT_LIBS)
  get_property(MLIR_CONVERSION_LIBS GLOBAL PROPERTY MLIR_CONVERSION_LIBS)
  get_property(MLIR_EXTENSION_LIBS GLOBAL PROPERTY MLIR_EXTENSION_LIBS)
  get_property(MLIR_TRANSLATION_LIBS GLOBAL PROPERTY MLIR_TRANSLATION_LIBS)

  set(MLIR_LIBS
    MLIRAnalysis
    MLIRDialect
    MLIRExecutionEngine
    MLIRIR
    MLIRParser
    MLIRPass
    MLIRSupport
    MLIRTransforms
    MLIRTransformUtils

    ${MLIR_DIALECT_LIBS}
    ${MLIR_CONVERSION_LIBS}
    ${MLIR_EXTENSION_LIBS}
    ${MLIR_TRANSLATION_LIBS}
  )

  # `MLIR_DIR` is something like `<prefix>/lib/cmake/mlir`
  cmake_path(GET MLIR_DIR PARENT_PATH lib_cmake_dir)
  cmake_path(GET lib_cmake_dir PARENT_PATH lib_dir)

  foreach(mlir_lib IN LISTS MLIR_LIBS)
    find_library(mlir_lib_path "${mlir_lib}" HINTS "${lib_dir}" "${MX_VENDOR_INSTALL_DIR}/lib" "${MX_INSTALL_LIB_DIR}")
    if (mlir_lib_path STREQUAL "mlir_lib_path-NOTFOUND")
      message(FATAL_ERROR "Cannot locate MLIR library ${mlir_lib}")
    endif()

    cmake_path(GET mlir_lib_path PARENT_PATH MLIR_LIB_DIR)
    break()
  endforeach()

  message(STATUS "Found MLIR libraries in ${MLIR_LIB_DIR}")
endif()


set(VAST_LIBS
  VAST::VASTTargetLLVMIR
  VAST::VASTAliasTypeInterface
  VAST::VASTElementTypeInterface
  VAST::VASTCodeGen
)

add_library("pillar" STATIC
  "AST.h"
  "AST.cpp"
  "ASTDecl.cpp"
  "ASTStmt.cpp"
  "ASTType.cpp"
  "NameProvider.cpp"
  "NameProvider.h"
  
  "${source_include_dir}/VAST.h"
  "VAST.cpp"
  "VAST.h"
  
  "${source_include_dir}/Clang.h"
  "Clang.cpp"
  "Clang.h"
)

if("LLVMAArch64CodeGen" IN_LIST LLVM_AVAILABLE_LIBS)
  set(llvm_aarch64_libs
    LLVMAArch64CodeGen
    LLVMAArch64AsmParser
    LLVMAArch64Desc
    LLVMAArch64Disassembler
    LLVMAArch64Info
    LLVMAArch64Utils
  )
endif()

if("LLVMX86CodeGen" IN_LIST LLVM_AVAILABLE_LIBS)
  set(llvm_x86_libs
    LLVMX86CodeGen
    LLVMX86AsmParser
    LLVMX86Desc
    LLVMX86Disassembler
    LLVMX86Info
  )
endif()

if("LLVMARMCodeGen" IN_LIST LLVM_AVAILABLE_LIBS)
  set(llvm_arm_libs
    LLVMARMCodeGen
    LLVMARMAsmParser
    LLVMARMDesc
    LLVMARMDisassembler
    LLVMARMInfo
    LLVMARMUtils
  )
endif()

target_link_libraries("pillar"
  PUBLIC
    ${VAST_LIBS}
    ${MLIR_LIBS}
    "gap::gap"
    "gap::gap-core"
    "gap::gap-settings"
    "std::coroutines"
    clang
    clangFrontend
    clangDriver
    clangSerialization
    clangParse
    clangAnalysis
    clangEdit
    clangAST
    clangLex
    clangBasic
    clangTooling
    clangFrontendTool
    clangIndex
    clangCodeGen
    clangSema 
    clangStaticAnalyzerFrontend 
    clangStaticAnalyzerCheckers 
    clangStaticAnalyzerCore 
    clangARCMigrate 
    clangRewriteFrontend 
    clangRewrite

    ${llvm_aarch64_libs}
    ${llvm_x86_libs}
    ${llvm_arm_libs}

    LLVMTarget
    LLVMOption
)

target_include_directories("pillar"
  PUBLIC
    "$<BUILD_INTERFACE:${LLVM_INCLUDE_DIRS};${LLVM_INCLUDE_DIR};${CLANG_INCLUDE_DIRS}>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    $"<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_compile_features("pillar"
  PRIVATE
    cxx_std_20
)
