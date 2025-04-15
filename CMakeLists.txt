cmake_minimum_required(VERSION 3.16)

# Đặt tên cho project
project(DataStructureCS163)

# Thêm tất cả file .cpp và .c trong thư mục src/
file(GLOB SOURCES src/*.cpp src/*.c)

# Thêm tinyfiledialogs.c explicitly if it's in src/
list(APPEND SOURCES src/tinyfiledialogs.c)

# Thêm tất cả file .h trong thư mục inc/
include_directories(inc/)

# Tạo file thực thi từ các source file
add_executable(${PROJECT_NAME} ${SOURCES})

# Option A: Raylib as a subdirectory
add_subdirectory("raylib")

# Option B: Raylib installed system-wide (uncomment if needed)
# find_package(raylib REQUIRED)
# if (NOT raylib_FOUND)
#     message(FATAL_ERROR "Raylib library not found. Please ensure Raylib is installed.")
# endif()

# Liên kết với Raylib và các thư viện cần thiết trên Windows
target_link_libraries(${PROJECT_NAME} raylib opengl32 gdi32 winmm)

# Vô hiệu hóa hỗ trợ MOD để tránh cảnh báo buffer overflow trong jar_mod
add_definitions(-DSUPPORT_FILEFORMAT_MOD=0)