HDR=''
SRC='black_white_plugin.c'

COMPILE_SHARED_LIB "black_white" "$SRC" "$HDR" "rayblib utils" ""
mv ../plugins/libblack_white.so ../plugins/plugin_black_white.so
