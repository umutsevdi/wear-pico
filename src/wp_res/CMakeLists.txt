file(GLOB WP_RES src/*.c)
include_directories(..)
add_library(wp_res ${WP_RES})
