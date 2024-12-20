cmake_minimum_required(VERSION 3.18)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/vcpkg_triplets/ DESTINATION $ENV{VCPKG_ROOT}/triplets/community/)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/toolchains/ DESTINATION $ENV{VCPKG_ROOT}/scripts/toolchains/)

project(arm_cmake_template C CXX)

add_executable(${PROJECT_NAME})
add_subdirectory(src)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME}
PRIVATE
    Boost::boost
    Threads::Threads
)

add_custom_target(TARGET ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/strip)

# Strip debug information
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}> -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/strip/${PROJECT_NAME})
