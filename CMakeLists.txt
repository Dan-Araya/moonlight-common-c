cmake_minimum_required(VERSION 3.1)
project(moonlight-common-c LANGUAGES C)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Usamos MbedTLS exclusivamente
set(USE_MBEDTLS ON)
option(CODE_ANALYSIS "Run code analysis during compilation" OFF)

set(CMAKE_C_STANDARD 11)

# Compilación de enet con código posición-independiente
set(CMAKE_POSITION_INDEPENDENT_CODE_BACKUP ${CMAKE_POSITION_INDEPENDENT_CODE})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_subdirectory(enet)
set(CMAKE_POSITION_INDEPENDENT_CODE ${CMAKE_POSITION_INDEPENDENT_CODE_BACKUP})
unset(CMAKE_POSITION_INDEPENDENT_CODE_BACKUP)

# Archivos fuente
aux_source_directory(src SRC_LIST)
aux_source_directory(reedsolomon SRC_LIST)

# Configuración de la librería
if (NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON)
  set(BUILD_SHARED_LIBS_OVERRIDE ON)
endif()

add_library(moonlight-common-c ${SRC_LIST})

if (BUILD_SHARED_LIBS_OVERRIDE)
  unset(BUILD_SHARED_LIBS)
  unset(BUILD_SHARED_LIBS_OVERRIDE)
endif()

# Linkeo enet
target_link_libraries(moonlight-common-c PRIVATE enet)

# Opciones por plataforma
if(MSVC)
  target_compile_options(moonlight-common-c PRIVATE /W3 /wd4100 /wd4232 /wd5105 /WX)
  target_link_libraries(moonlight-common-c PRIVATE ws2_32.lib winmm.lib)
elseif(MINGW)
  target_link_libraries(moonlight-common-c PRIVATE -lws2_32 -lwinmm)
else()
  target_compile_options(moonlight-common-c PRIVATE -Wall -Wextra -Wno-unused-parameter -Werror)
  if (CODE_ANALYSIS AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(moonlight-common-c PRIVATE -fanalyzer)
  endif()
endif()

# ✅ SOLO MbedTLS (sin OpenSSL)
target_compile_definitions(moonlight-common-c PRIVATE USE_MBEDTLS)
target_link_libraries(moonlight-common-c PRIVATE mbedcrypto)
target_include_directories(moonlight-common-c SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/mbedtls/include)

# Configuración según tipo de build
string(TOUPPER "x${CMAKE_BUILD_TYPE}" BUILD_TYPE)
if("${BUILD_TYPE}" STREQUAL "XDEBUG")
  target_compile_definitions(moonlight-common-c PRIVATE LC_DEBUG)
else()
  target_compile_definitions(moonlight-common-c PRIVATE NDEBUG)
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(moonlight-common-c PRIVATE -Wno-maybe-uninitialized)
  endif()
endif()

# Includes
target_include_directories(moonlight-common-c SYSTEM PUBLIC src)
target_include_directories(moonlight-common-c PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/reedsolomon
)

# Para evitar redefiniciones con socklen_t
target_compile_definitions(moonlight-common-c PRIVATE HAS_SOCKLEN_T)
