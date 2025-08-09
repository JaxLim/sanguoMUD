add_library(core
  World.cpp World.h
  Entity.cpp Entity.h
  Command.cpp Command.h
  Data.cpp Data.h
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC)
  target_compile_options(core PRIVATE /utf-8)
endif ()
