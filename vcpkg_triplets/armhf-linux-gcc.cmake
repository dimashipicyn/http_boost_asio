set(VCPKG_TARGET_ARCHITECTURE arm)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)
set(VCPKG_CMAKE_SYSTEM_PROCESSOR arm)

set(VCPKG_C_FLAGS " -march=armv7-a -mfpu=neon")
set(VCPKG_CXX_FLAGS " -march=armv7-a -mfpu=neon")
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../../scripts/toolchains/gcc_arm.cmake)