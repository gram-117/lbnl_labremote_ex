cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 11)

project(ldrd_28nm_testing)

add_executable(test src/test.cpp)
target_link_libraries(test PUBLIC Com)
target_link_libraries(test PUBLIC DevCom)
target_link_libraries(test PUBLIC EquipConf)

add_executable(testINO src/testINO.cpp)
target_link_libraries(testINO PUBLIC Com)
target_link_libraries(testINO PUBLIC DevCom)
target_link_libraries(testINO PUBLIC EquipConf)
target_link_libraries(testINO PUBLIC Utils)

add_subdirectory(labRemote)
