SOURCE_FILES="image_lib.c"
HEADER_FILES="image_lib.h"

ADD_SUBDIRECTORY "utils"

COMPILE_SHARED_LIB "image_lib" "$SOURCE_FILES" "$HEADER_FILES" "utils" "z"
