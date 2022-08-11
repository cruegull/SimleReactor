# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.0)

# 项目信息
project (SimpleReactor)
# 项目目录
aux_source_directory(./single LIST1)
# 指定生成目标
add_executable(SimpleReactor.o ${LIST1})

# 项目信息
project (SimpleReactorThread)
# 项目目录
aux_source_directory(./single_thread LIST2)
# 指定生成目标
add_executable(SimpleReactorThread.o ${LIST2})

# 项目信息
project (Test)
# 指定生成目标
add_executable(test.o test.cpp)