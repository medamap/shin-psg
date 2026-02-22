cmake_minimum_required(VERSION 3.16)
project(shin_psg_milestone_1_2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# ビルドタイプが指定されていなければ Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ライブラリパス
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# -----------------------------------------------------------------------------
# ayumi (C ライブラリ)
# -----------------------------------------------------------------------------
add_library(ayumi STATIC
    ${LIBS_DIR}/ayumi/ayumi.c
)
target_include_directories(ayumi PUBLIC ${LIBS_DIR}/ayumi)

# -----------------------------------------------------------------------------
# emu2149 (C ライブラリ)
# -----------------------------------------------------------------------------
add_library(emu2149 STATIC
    ${LIBS_DIR}/emu2149/emu2149.c
)
target_include_directories(emu2149 PUBLIC ${LIBS_DIR}/emu2149)

# -----------------------------------------------------------------------------
# ymfm (C++ ライブラリ)
# -----------------------------------------------------------------------------
file(GLOB YMFM_SRCS ${LIBS_DIR}/ymfm/src/*.cpp)
add_library(ymfm STATIC ${YMFM_SRCS})
target_include_directories(ymfm PUBLIC ${LIBS_DIR}/ymfm/src)
target_compile_features(ymfm PUBLIC cxx_std_14)

# -----------------------------------------------------------------------------
# 共通インクルード (wav_writer.h, miniaudio.h)
# -----------------------------------------------------------------------------
set(COMMON_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src ${LIBS_DIR})

# -----------------------------------------------------------------------------
# 検証プログラム: psg_ayumi_test
# -----------------------------------------------------------------------------
add_executable(psg_ayumi_test
    src/psg_ayumi_test.cpp
)
target_include_directories(psg_ayumi_test PRIVATE ${COMMON_INCLUDE})
target_link_libraries(psg_ayumi_test PRIVATE ayumi)
if(UNIX)
    target_link_libraries(psg_ayumi_test PRIVATE m)
endif()

# -----------------------------------------------------------------------------
# 検証プログラム: psg_emu2149_test
# -----------------------------------------------------------------------------
add_executable(psg_emu2149_test
    src/psg_emu2149_test.cpp
)
target_include_directories(psg_emu2149_test PRIVATE ${COMMON_INCLUDE})
target_link_libraries(psg_emu2149_test PRIVATE emu2149)
if(UNIX)
    target_link_libraries(psg_emu2149_test PRIVATE m)
endif()

# -----------------------------------------------------------------------------
# 検証プログラム: fm_ymfm_test
# -----------------------------------------------------------------------------
add_executable(fm_ymfm_test
    src/fm_ymfm_test.cpp
)
target_include_directories(fm_ymfm_test PRIVATE ${COMMON_INCLUDE})
target_link_libraries(fm_ymfm_test PRIVATE ymfm)
if(UNIX)
    target_link_libraries(fm_ymfm_test PRIVATE m)
endif()

# -----------------------------------------------------------------------------
# 検証プログラム: fm_ymfm_opm_test (YM2151 / X1 + FM board, X1turboZ, X68000)
# -----------------------------------------------------------------------------
add_executable(fm_ymfm_opm_test
    src/fm_ymfm_opm_test.cpp
)
target_include_directories(fm_ymfm_opm_test PRIVATE ${COMMON_INCLUDE})
target_link_libraries(fm_ymfm_opm_test PRIVATE ymfm)
if(UNIX)
    target_link_libraries(fm_ymfm_opm_test PRIVATE m)
endif()

# -----------------------------------------------------------------------------
# ビルド情報を表示
# -----------------------------------------------------------------------------
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Targets: psg_ayumi_test, psg_emu2149_test, fm_ymfm_test, fm_ymfm_opm_test")
message(STATUS "Outputs: output_ayumi.wav, output_emu2149.wav, output_ymfm_opn.wav, output_ymfm_opm.wav")
