file(GLOB SW_RES src/*.c)
include_directories(include/)
add_library(sw_resources ${SW_RES})
