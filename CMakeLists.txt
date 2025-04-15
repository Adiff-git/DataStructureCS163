cmake_minimum_required(VERSION 3.16)

# Đặt tên cho project
project(DataStructureCS163)

# Thêm tất cả file .cpp trong thư mục src/
file(GLOB_RECURSE SOURCES src/*.cpp)

# Thêm tinyfiledialogs vào danh sách nguồn
set(TINYFILEDIALOGS_SRC thirdparty/tinyfiledialogs/tinyfiledialogs.c)
list(APPEND SOURCES ${TINYFILEDIALOGS_SRC})

# Thêm tất cả file .h trong thư mục inc/
include_directories(inc/)
include_directories(thirdparty/tinyfiledialogs) # Thêm thư mục chứa tinyfiledialogs.h

# Tạo file thực thi từ các source file
add_executable(${PROJECT_NAME} ${SOURCES})

# Thêm thư viện raylib
add_subdirectory("raylib")
target_link_libraries(${PROJECT_NAME} raylib)

# Copy thư mục resources vào build để giữ hình ảnh, âm thanh...
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})