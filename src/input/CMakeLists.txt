set(InputSources
        InputContext.cpp
        )

set(InputcHeaders
        InputContext.hpp
        )

add_library(DeInput       SHARED ${InputSources})
add_library(DeInputStatic STATIC ${InputSources})

target_link_libraries(DeInput       X11 gainput DeBase)
target_link_libraries(DeInputStatic X11 gainput DeBase)

target_include_directories(DeInput PRIVATE ../base)
target_include_directories(DeInput PRIVATE ../graphics)

target_include_directories(DeInputStatic PRIVATE ../base)
target_include_directories(DeInputStatic PRIVATE ../graphics)