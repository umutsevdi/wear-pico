file(GLOB RESOURCES menu/*.c watch.c font.c)
add_library(resources resources.h ${RESOURCES})
