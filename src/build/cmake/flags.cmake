macro(setup_windows_flags)
  # Enable color
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")

  # Base flags - enable warnings
  list(APPEND PROJECT_COMPILE_OPTIONS /W4 /clang:-Wall /clang:-Wextra /clang:-Wpedantic)

  if(ENABLE_WARNINGS_AS_ERRORS)
    list(APPEND PROJECT_COMPILE_OPTIONS /WX /clang:-Werror)
  endif()

  # Debug configuration
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od /Zi /DDEBUG /ZH:SHA_256")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG /ZH:SHA_256")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG")

  # Release configuration
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /DNDEBUG /Gw /Gy /Zc:dllexportInlines- /clang:-fno-exceptions /clang:-funroll-loops")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG /Gw /Gy /Zc:dllexportInlines- /clang:-fno-exceptions /clang:-funroll-loops")

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /permissive- /Zc:__cplusplus /Zc:inline /Zc:strictStrings /Zc:alignedNew /Zc:sizedDealloc /Zc:threadSafeInit")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive- /Zc:__cplusplus /Zc:inline /Zc:strictStrings /Zc:alignedNew /Zc:sizedDealloc /Zc:threadSafeInit")

  # set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  # Link time optimization
  if(ENABLE_LTO)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Gw /Gy /clang:-flto=thin")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Gw /Gy /clang:-flto=thin")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /OPT:REF /OPT:ICF /MAP:${CMAKE_BINARY_DIR}/link.map")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG /OPT:REF /OPT:ICF /MAP:${CMAKE_BINARY_DIR}/link.map")
  endif()

  # Native architecture optimization
  if(ENABLE_NATIVE_ARCH)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /arch:AVX2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /arch:AVX2")
  endif()

  if(ENABLE_BUILD_REPORT)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /clang:-ftime-trace")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clang:-ftime-trace")
  endif()

  if(ENABLE_OPTIMIZATION_REPORT)
    list(APPEND PROJECT_COMPILE_OPTIONS "/clang:-fsave-optimization-record;/clang:-fdebug-compilation-dir=.;/clang:-Rpass='.*';/clang:-Rpass-missed='.*';/clang:-Rpass-analysis='.*'")
  endif()

  list(APPEND WINDOWS_LINK_LIBRARIES "dbghelp")

  list(APPEND PROJECT_COMPILE_DEFINITIONS NOMINMAX=1)
endmacro()

macro(setup_unix_flags)
  # Enable color and use libc++
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always -fPIC -stdlib=libc++")

  # Base flags - enable warnings
  list(APPEND PROJECT_COMPILE_OPTIONS -Wall -Wextra -Wpedantic -fno-common)

  if(ENABLE_WARNINGS_AS_ERRORS)
    list(APPEND PROJECT_COMPILE_OPTIONS -Werror)
  endif()

  # Debug configuration
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g -fno-inline -fmacro-backtrace-limit=0 -frtti -fno-omit-frame-pointer -funwind-tables")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-inline -fmacro-backtrace-limit=0 -frtti -fno-omit-frame-pointer -funwind-tables")

  # Release configuration
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -ffunction-sections -fdata-sections -fstack-protector-strong -D_FORTIFY_SOURCE=2 -ftrivial-auto-var-init=zero -fno-rtti -fomit-frame-pointer -fno-exceptions -funroll-loops")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffunction-sections -fdata-sections -fstack-protector-strong -D_FORTIFY_SOURCE=2 -ftrivial-auto-var-init=zero -fno-rtti -fomit-frame-pointer -fno-exceptions -funroll-loops")

  # Avoid mingw
  if(NOT MINGW_BUILD)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -rdynamic")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -rdynamic")

    if(NOT TARGET_OS_NAME MATCHES "darwin")
      set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-clash-protection")
      set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fstack-clash-protection")
    endif()
  endif()

  # Hide symbols by default, this will break stack trace.
  # set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  # set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

  # Link time optimization
  if(ENABLE_LTO)
    if(NOT MINGW_BUILD)
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto=thin -funified-lto")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=thin -funified-lto")

      if(NOT TARGET_OS_NAME MATCHES "darwin")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--lto=thin -funified-lto")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--lto=thin -funified-lto")
      endif()
    endif()

    if(TARGET_OS_NAME MATCHES "darwin")
      # for macos (ld64.lld)
      set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,-dead_strip -Wl,-x")
      set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -Wl,-dead_strip")
    else()
      set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections -Wl,--strip-all")
      set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
    endif()
  endif()

  # Native architecture optimization
  if(ENABLE_NATIVE_ARCH)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native -mtune=native")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native")
  endif()

  if(ENABLE_BUILD_REPORT)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ftime-trace")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-trace")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_BINARY_DIR}/link.map")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-Map=${CMAKE_BINARY_DIR}/link.map")
  endif()

  if(ENABLE_OPTIMIZATION_REPORT)
    list(APPEND PROJECT_COMPILE_OPTIONS "-fsave-optimization-record;-fdebug-compilation-dir=.;-Rpass='.*';-Rpass-missed='.*';-Rpass-analysis='.*'")
  endif()

  # Sanitize configuration
  if(ENABLE_SANITIZERS AND BUILD_DEBUG AND NOT MINGW_BUILD)
    set(SAN_FLAGS "-fsanitize=address,undefined")

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${SAN_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SAN_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${SAN_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${SAN_FLAGS}")
  endif()

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmacro-prefix-map=${PROJECT_ROOT_DIR}/=")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmacro-prefix-map=${PROJECT_ROOT_DIR}/=")
endmacro()

macro(setup_mingw_flags)
  set(MINGW_LINK_LIBRARIES "dbghelp")
endmacro()

macro(setup_apple_flags)
  execute_process(
    COMMAND xcrun --sdk macosx --show-sdk-path
    OUTPUT_VARIABLE MACOS_SDK_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  add_compile_options(-isysroot ${MACOS_SDK_PATH} -I${MACOS_SDK_PATH}/usr/include)
endmacro()

macro(setup_common_flags)
  # Profiling with llvm-coverage
  if(ENABLE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
  endif()

  # Profiling with llvm-xray
  if(ENABLE_XRAY)
    list(APPEND PROJECT_COMPILE_OPTIONS -fxray-instrument -fxray-instrumentation-bundle=function -fxray-instruction-threshold=1)
    list(APPEND PROJECT_LINK_OPTIONS -fxray-instrument -fxray-instrumentation-bundle=function -fxray-instruction-threshold=1)

    if(NOT TARGET_OS_NAME MATCHES "darwin")
      list(APPEND PROJECT_COMPILE_OPTIONS -fxray-shared)
      list(APPEND PROJECT_LINK_OPTIONS -fxray-shared)
    endif()

    list(APPEND PROJECT_COMPILE_DEFINITIONS ENABLE_XRAY=1)
  else()
    list(APPEND PROJECT_COMPILE_DEFINITIONS ENABLE_XRAY=0)
  endif()

  if(ENABLE_AVX2)
    include(CheckCXXSourceRuns)

    if(CMAKE_CROSSCOMPILING)
      message(STATUS "Cross-compiling detected. Skipping runtime AVX2 check.")

      set(HAS_AVX2 TRUE)
    else()
      set(AVX2_TEST_CODE "
      #include <immintrin.h>
      int main() {
          __m256i a = _mm256_set1_epi32(1);
          __m256i b = _mm256_add_epi32(a, a);
          return 0;
      }")

      set(CMAKE_REQUIRED_FLAGS "-mavx2")
      check_cxx_source_runs("${AVX2_TEST_CODE}" HAS_AVX2)
    endif()

    if(HAS_AVX2)
      message(STATUS "AVX2 support detected")
      list(APPEND PROJECT_COMPILE_DEFINITIONS ENABLE_AVX2=1)
      list(APPEND PROJECT_COMPILE_OPTIONS -mavx2)
    else()
      list(APPEND PROJECT_COMPILE_DEFINITIONS ENABLE_AVX2=0)
      message(STATUS "AVX2 support not available")
    endif()
  else()
    list(APPEND PROJECT_COMPILE_DEFINITIONS ENABLE_AVX2=0)
  endif()
endmacro()

macro(setup_flags)
  include(CheckCXXCompilerFlag)
  string(TOUPPER ${MAIN_EXECUTABLE_NAME_FROM_CONFIG} UPPER_PROJECT_NAME)

  if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang" OR NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "Unknown compiler: C=${CMAKE_C_COMPILER_ID}, CXX=${CMAKE_CXX_COMPILER_ID}")
  endif()

  if(MSVC AND TARGET_OS_NAME MATCHES "windows")
    setup_windows_flags()
  elseif(NOT MSVC)
    setup_unix_flags()
  endif()

  if(MINGW_BUILD)
    setup_mingw_flags()
  endif()

  if(APPLE)
    setup_apple_flags()
  endif()

  setup_common_flags()
endmacro()

function(print_all_build_flags)
  message("${Cyan}
-=-=-=-= Complete Flag Configuration -=-=-=-=-
CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}
CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}
CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}
CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}
CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}
CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}
CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}
CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}
CMAKE_EXE_LINKER_FLAGS_DEBUG: ${CMAKE_EXE_LINKER_FLAGS_DEBUG}
CMAKE_SHARED_LINKER_FLAGS_DEBUG: ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}
CMAKE_EXE_LINKER_FLAGS_RELEASE: ${CMAKE_EXE_LINKER_FLAGS_RELEASE}
CMAKE_SHARED_LINKER_FLAGS_RELEASE: ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}
PROJECT_COMPILE_OPTIONS: ${PROJECT_COMPILE_OPTIONS}
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
${ColourReset}")
endfunction()

# Helper function to reset all flags (for testing)
function(reset_all_flags)
  set(CMAKE_C_FLAGS "" PARENT_SCOPE)
  set(CMAKE_CXX_FLAGS "" PARENT_SCOPE)
  set(CMAKE_C_FLAGS_DEBUG "" PARENT_SCOPE)
  set(CMAKE_CXX_FLAGS_DEBUG "" PARENT_SCOPE)
  set(CMAKE_C_FLAGS_RELEASE "" PARENT_SCOPE)
  set(CMAKE_CXX_FLAGS_RELEASE "" PARENT_SCOPE)
  set(CMAKE_EXE_LINKER_FLAGS "" PARENT_SCOPE)
  set(CMAKE_SHARED_LINKER_FLAGS "" PARENT_SCOPE)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" PARENT_SCOPE)
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "" PARENT_SCOPE)
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "" PARENT_SCOPE)
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "" PARENT_SCOPE)
  message(STATUS "All flags have been reset")
endfunction()
