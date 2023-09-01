cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 11)

project(ldrd_28nm_testing)

add_executable(test src/test.cpp)

target_link_libraries(test PUBLIC Com)

target_link_libraries(test PUBLIC DevCom)

target_link_libraries(test PUBLIC EquipConf)

add_subdirectory(labRemote)
