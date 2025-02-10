cmake_minimum_required(VERSION 3.15)
project(Server)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)

# UTF-8 컴파일 옵션
add_compile_options("/utf-8")

# 런타임 라이브러리 설정 (멀티스레드 DLL)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

# 파일 경로 정의
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 실행 파일 생성
add_executable(Server ${SOURCES})

# 라이브러리 링크
target_link_libraries(Server ws2_32)
