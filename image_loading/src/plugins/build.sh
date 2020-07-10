COMPILE_PLUGIN() {
	local PLUGIN_NAME=$1
	COMPILE_SHARED_LIB "$@"
	ECHO_AND_EXEC_COMMAND "mv ../../plugins/lib${PLUGIN_NAME}.so ../../plugins/plugin_${PLUGIN_NAME}.so"
}

HDR=''
SRC='black_white_plugin.c'
COMPILE_PLUGIN "black_white" "$SRC" "$HDR" "rayblib utils" ""

HDR=''
SRC='convolution.c'
COMPILE_PLUGIN "convolution" "$SRC" "$HDR" "rayblib utils" ""
