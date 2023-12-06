file(GLOB SW_RES src/*.c)
include_directories(..)
add_library(sw_res ${SW_RES})
