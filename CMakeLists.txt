
cmake_minimum_required(VERSION 3.16)

# Đặt tên cho project
project(DataStructureCS163)

# Thêm tất cả file .cpp trong thư mục src/
file(GLOB_RECURSE SOURCES src/*.cpp)

# Thêm tất cả file .h trong thư mục inc/
include_directories(inc/)

# Tạo file thực thi từ các source file
add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory("raylib")
target_link_libraries(${PROJECT_NAME} raylib)

