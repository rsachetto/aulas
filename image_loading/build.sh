#!/bin/bash
OPTIONS_FILE=./bsbash/build_functions.sh
FUNCTIONS_FILE=./bsbash/find_functions.sh

if [ -f "$OPTIONS_FILE" ]; then
    # shellcheck disable=SC1090
    source $OPTIONS_FILE
else
    echo "$OPTIONS_FILE not found, aborting compilation"
    exit 1
fi

if [ -f "$FUNCTIONS_FILE" ]; then
	# shellcheck disable=SC1090
	source $FUNCTIONS_FILE
fi

###########User code#####################
GET_BUILD_OPTIONS "$@"

if [ "$BUILD_TYPE" == "release" ]; then
    C_FLAGS="$C_FLAGS -O3"
elif [ "$BUILD_TYPE" == "debug" ]; then
    C_FLAGS="$C_FLAGS -g -DDEBUG_INFO"
else
  	PRINT_ERROR "$BUILD_TYPE is not a valid BUILD_TYPE."
  	PRINT_ERROR "Valid BUILD_TYPE options are: release, debug (-r or -d options)"
  	exit 1
fi

for i in "${BUILD_ARGS[@]}"; do

    if [ "$i" == "clean" ]; then
        echo "Cleaning $BUILD_TYPE"
        CLEAN_PROJECT "$BUILD_TYPE"
        cd - || exit 1;
        exit 0    
    fi

done

DEFAULT_C_FLAGS="-fopenmp -std=gnu99 -fno-strict-aliasing  -Wall -Wno-stringop-truncation -Wno-unused-function -Wno-char-subscripts -Wno-unused-result"
RUNTIME_OUTPUT_DIRECTORY="$ROOT_DIR/bin"
LIBRARY_OUTPUT_DIRECTORY="$ROOT_DIR/shared_libs"

C_FLAGS="$C_FLAGS $DEFAULT_C_FLAGS"

ADD_SUBDIRECTORY "src/raylib/src"
ADD_SUBDIRECTORY "src/utils"
    
COMPILE_EXECUTABLE "image_loader" "src/main.c src/plugin_config.c" "src/plugin_config.h" "raylib utils" "OpenGL GLX GLU pthread X11 rt dl m" "$EXTRA_LIB_PATH $LIBRARY_OUTPUT_DIRECTORY"


LIBRARY_OUTPUT_DIRECTORY="$ROOT_DIR/plugins"
ADD_SUBDIRECTORY "src/plugins/"

