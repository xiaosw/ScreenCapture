# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html


cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(src/main/cpp/librtmp)


add_library(
             native-lib
             SHARED
             src/main/cpp/native-lib.c )




target_link_libraries(
                       native-lib
                        rtmp
                      log )