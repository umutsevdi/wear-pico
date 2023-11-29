file(GLOB RESOURCES menu/*.c watch.c clockfont.c)
add_library(resources resources.h ${RESOURCES})
