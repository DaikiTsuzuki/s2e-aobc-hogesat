cmake_policy(SET CMP0048 NEW)
project(S2E_AOBC
  LANGUAGES CXX
  DESCRIPTION "S2E_AOBC"
  VERSION 5.0.4
  )

cmake_minimum_required(VERSION 3.13)

# build config
option(USE_HILS "Use HILS" OFF)
option(USE_C2A "Use C2A" ON)

# preprocessor
if(WIN32)
  add_definitions(-DWIN32)
endif()

## set directory path
set(S2E_CORE_DIR ${CMAKE_CURRENT_LIST_DIR}/s2e-core)

if(NOT DEFINED EXT_LIB_DIR)
  set(EXT_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/ExtLibraries)
endif()
set(CSPICE_DIR ${EXT_LIB_DIR}/cspice)
set(NRLMSISE00_DIR ${EXT_LIB_DIR}/nrlmsise00)

if(NOT DEFINED FLIGHT_SW_DIR)
  set(FLIGHT_SW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../FlightSW)
endif()
if(NOT DEFINED C2A_NAME)
  set(C2A_NAME "c2a-aobc-hogesat")
endif()

## Directory path for ini files
if(NOT DEFINED INI_FILE_DIR_FROM_EXE)
  set(INI_FILE_DIR_FROM_EXE ../../data/initialize_files)
endif()

if(NOT DEFINED EXT_LIB_DIR_FROM_EXE)
  set(EXT_LIB_DIR_FROM_EXE ../../ExtLibraries)
endif()

if(NOT DEFINED CORE_DIR_FROM_EXE)
  set(CORE_DIR_FROM_EXE ../../s2e-core)
endif()

## options to use C2A
if(USE_C2A)
  set(C2A_DIR ${FLIGHT_SW_DIR}/${C2A_NAME})
  message("C2A dir: ${C2A_DIR}")
  add_definitions(-DUSE_C2A)
  add_definitions(-DSILS_FW)
  #include_directories of C2A
  include_directories(${C2A_DIR}/src)

  #add subdirectory
  set(BUILD_C2A_AS_CXX ON)
  add_subdirectory(${C2A_DIR} C2A_CORE)
endif()

## options to use HILS
if(USE_HILS AND WIN32)
  add_definitions(-DUSE_HILS)
  ## winsock2
  SET (CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
  find_library(WS2_32_LIB ws2_32.lib)
  message("path for winsock2 is")
  message(${WS2_32_LIB})
endif()

## include directories of S2E-CORE
include_directories(${CSPICE_DIR}/include)
include_directories(${NRLMSISE00_DIR}/src)
include_directories(${S2E_CORE_DIR}/src)

## add_subdirectories
add_subdirectory(${S2E_CORE_DIR}/src/components s2e_core/components)
add_subdirectory(${S2E_CORE_DIR}/src/disturbances s2e_core/disturbances)
add_subdirectory(${S2E_CORE_DIR}/src/dynamics s2e_core/dynamics)
add_subdirectory(${S2E_CORE_DIR}/src/environment/global s2e_core/environment/global)
add_subdirectory(${S2E_CORE_DIR}/src/environment/local s2e_core/environment/local)
add_subdirectory(${S2E_CORE_DIR}/src/library s2e_core/library)
add_subdirectory(${S2E_CORE_DIR}/src/simulation s2e_core/simulation)

# Add all cpp files as SOURCE_FILES
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
# Uncomment the following line to exclude any files that match the REGEX from SOURCE_FILES
list(FILTER SOURCE_FILES EXCLUDE REGEX ${CMAKE_CURRENT_LIST_DIR}/src/component/aocs/cube_wheel.cpp)

# Create executable file
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

## cspice library
if(CYGWIN)
  SET (CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
  find_library(CSPICE_LIB
    NAMES cspice.a csupport.a
    PATHS ${CSPICE_DIR}/cspice_cygwin/lib)
elseif(UNIX)
  find_library(CSPICE_LIB
    NAMES cspice.a csupport.a
    PATHS ${CSPICE_DIR}/cspice_unix/lib)
elseif(WIN32)
  SET (CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
  find_library(CSPICE_LIB
    NAMES cspice.lib csupport.lib
    PATHS ${CSPICE_DIR}/cspice_msvs/lib)
endif()
if(NOT CSPICE_LIB)
  message(FATAL_ERROR "cspice not found in ${EXT_LIB_DIR}")
endif()
#target_link_libraries(${PROJECT_NAME} ${CSPICE_LIB})

## nrlmsise00 library
if(CYGWIN)
  SET (CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  find_library(NRLMSISE00_LIB
    NAMES libnrlmsise00.a
    PATHS ${NRLMSISE00_DIR}/lib)
elseif(UNIX)
  find_library(NRLMSISE00_LIB
    NAMES libnrlmsise00.a
    PATHS ${NRLMSISE00_DIR}/lib)
elseif(WIN32)
  SET (CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
  find_library(NRLMSISE00_LIB
    NAMES libnrlmsise00.lib
    PATHS ${NRLMSISE00_DIR}/lib)
endif()
if(NOT NRLMSISE00_LIB)
  message(FATAL_ERROR "nrlmsise00 not found in ${EXT_LIB_DIR}")
endif()
#target_link_libraries(${PROJECT_NAME} ${NRLMSISE00_LIB})

## Linking libraries
set(S2E_LIBRARIES
  IGRF WRAPPER_NRLMSISE00 INIH SGP4 UTIL OPTICS RELATIVE_ORBIT_MODELS ORBIT_MODELS GEODESY MATH
)
# Initialize link
target_link_libraries(COMPONENT DYNAMICS GLOBAL_ENVIRONMENT LOCAL_ENVIRONMENT LIBRARY)
target_link_libraries(DYNAMICS GLOBAL_ENVIRONMENT LOCAL_ENVIRONMENT SIMULATION LIBRARY)
target_link_libraries(DISTURBANCE DYNAMICS GLOBAL_ENVIRONMENT LOCAL_ENVIRONMENT LIBRARY)
target_link_libraries(SIMULATION DYNAMICS GLOBAL_ENVIRONMENT LOCAL_ENVIRONMENT DISTURBANCE LIBRARY)
target_link_libraries(GLOBAL_ENVIRONMENT ${CSPICE_LIB} LIBRARY)
target_link_libraries(LOCAL_ENVIRONMENT GLOBAL_ENVIRONMENT ${CSPICE_LIB} LIBRARY)
target_link_libraries(LIBRARY ${NRLMSISE00_LIB})

target_link_libraries(${PROJECT_NAME} DYNAMICS)
target_link_libraries(${PROJECT_NAME} DISTURBANCE)
target_link_libraries(${PROJECT_NAME} SIMULATION)
target_link_libraries(${PROJECT_NAME} GLOBAL_ENVIRONMENT LOCAL_ENVIRONMENT)
target_link_libraries(${PROJECT_NAME} COMPONENT)

## C2A integration
if(USE_C2A)
  target_link_libraries(${PROJECT_NAME} C2A)
endif()

## HILS
if(USE_HILS)
  target_link_libraries(${PROJECT_NAME} ${WS2_32_LIB})
  set_target_properties(${PROJECT_NAME} PROPERTIES COMMON_LANGUAGE_RUNTIME "")
  set_target_properties(COMPONENT PROPERTIES COMMON_LANGUAGE_RUNTIME "")
  set_target_properties(DYNAMICS PROPERTIES COMMON_LANGUAGE_RUNTIME "")
  set_target_properties(DISTURBANCE PROPERTIES COMMON_LANGUAGE_RUNTIME "")
  set_target_properties(SIMULATION PROPERTIES COMMON_LANGUAGE_RUNTIME "")
  set_target_properties(GLOBAL_ENVIRONMENT PROPERTIES COMMON_LANGUAGE_RUNTIME "")
  set_target_properties(LOCAL_ENVIRONMENT PROPERTIES COMMON_LANGUAGE_RUNTIME "")
  set_target_properties(HILS_IO PROPERTIES COMMON_LANGUAGE_RUNTIME "")
  set_target_properties(RELATIVE_INFO PROPERTIES COMMON_LANGUAGE_RUNTIME "")
endif()

## Cmake debug
message("Cspice_LIB:  " ${CSPICE_LIB})
message("nrlmsise00_LIB:  " ${NRLMSISE00_LIB})

include(${S2E_CORE_DIR}/common.cmake)
