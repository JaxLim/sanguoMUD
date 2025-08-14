set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(ui_qt
  main.cpp
  MainWindow.cpp
  MainWindow.h
  MapWidget.cpp
  MapWidget.h
  FullMapWindow.cpp
  FullMapWindow.h
)

target_link_libraries(ui_qt PRIVATE core Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(ui_qt PRIVATE ${CMAKE_SOURCE_DIR}/core)

if (MSVC)
  target_compile_options(ui_qt PRIVATE /utf-8)
endif ()

add_custom_command(TARGET ui_qt POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/data
          $<TARGET_FILE_DIR:ui_qt>/../data)

