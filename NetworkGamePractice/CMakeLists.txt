cmake_minimum_required(VERSION 3.15)
project(NetworkGamePractice)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)

# UTF-8 컴파일 옵션
add_compile_options("/utf-8")

# 런타임 라이브러리 설정 (멀티스레드 DLL)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

# 파일 경로 정의
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib)
include_directories(${INCLUDE_DIR})
link_directories(${LIBRARY_DIR})
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 실행 파일 생성
add_executable(NetworkGamePractice ${SOURCES})

# 라이브러리 링크
target_link_libraries(NetworkGamePractice glew32 glfw3 opengl32)

# DLL 파일 복사
add_custom_command(TARGET NetworkGamePractice POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${LIBRARY_DIR}/glew32.dll
    ${LIBRARY_DIR}/glfw3.dll
    $<TARGET_FILE_DIR:NetworkGamePractice>
)