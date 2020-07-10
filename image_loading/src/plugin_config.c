#include "plugin_config.h"
#include "utils/stb_ds.h"
#include "utils/file_utils.h"
#include <dlfcn.h>
#include <stdio.h>
#include <ctype.h>

#define RAYGUI_IMPLEMENTATION
#include "raylib/src/raygui.h"
#undef RAYGUI_IMPLEMENTATION

#define GUI_TEXTBOX_EXTENDED_IMPLEMENTATION
#include "raylib/src/gui_textbox_extended.h"

struct plugin_config * init_plugins(const char *plugin_dir, int *num_plugins) {

	string_array available_plugins = list_files_from_dir(plugin_dir, "plugin_");

	*num_plugins = arrlen(available_plugins);

	struct plugin_config *loaded_plugins = (struct plugin_config*) calloc(*num_plugins, sizeof(struct plugin_config));
	
	for(int i = 0; i < arrlen(available_plugins); i++) {

		char full_plugin_path[PATH_MAX];
		sprintf(full_plugin_path, "%s/%s", plugin_dir, available_plugins[i]);

		printf("Loading %s\n", full_plugin_path);

		loaded_plugins[i].handle = dlopen(full_plugin_path, RTLD_LAZY);

		loaded_plugins[i].init_plugin = dlsym(loaded_plugins[i].handle, "init_plugin");
		
		char *error = dlerror();
		
		if (error != NULL)  {
			fprintf(stderr, "init_plugin function not found in plugin %s. Error from dlsym %s\n", full_plugin_path, error);
			loaded_plugins[i].loaded = false;
			free(error);
		}
		else {

			loaded_plugins[i].plugin_info = (loaded_plugins[i].init_plugin)();

			for(size_t f = 0; f < shlen(loaded_plugins[i].plugin_info.functions); f++) {
				char *f_name = loaded_plugins[i].plugin_info.functions[f].key;
				printf("Registering %s function for %s plugin\n", f_name, loaded_plugins[i].plugin_info.name);
				shput(loaded_plugins[i].apply_plugin,  strdup(f_name), dlsym(loaded_plugins[i].handle, f_name));

			}

			error = dlerror();

			if (error != NULL)  {
				fprintf(stderr, "apply_plugin function not found in plugin %s. Error from dlsym %s\n", full_plugin_path, error);
				loaded_plugins[i].loaded = false;
				free(error);
			}
			else {
				loaded_plugins[i].end_plugin = dlsym(loaded_plugins[i].handle, "end_plugin");
			}
		}

	}

	return loaded_plugins;
}

#define SIZEOF(A) (sizeof(A)/sizeof(A[0]))

bool draw_plugin_params_window(char *function_name, struct function_param_descr *params,  char params_text[256][128], bool *apply) {
	
	int w_pos_x = 400;
	int w_pos_y = 400;
	const float text_box_width = 60;
    const float text_box_height = 25;

	int n_params = arrlen(params);

	int w_height = n_params * (text_box_height) + 100;

	bool window_closed = GuiWindowBox((Rectangle){w_pos_x, w_pos_x, 200, w_height}, function_name);
			
	int box_pos = w_pos_x + 10;
	int label_y_dist = 35;
	int text_box_y_dist = 30;

	int font_size = 10;

	for(int i = 0; i < n_params; i++) {		

		if(isalpha(params_text[i][strlen(params_text[i])-1])) {
			params_text[i][strlen(params_text[i])-1] = '\0'; 
		}

		int tw = MeasureText(params[i].name, font_size);                                
	    DrawText(params[i].name, box_pos + 5, w_pos_y + label_y_dist, font_size, BLACK);
	    GuiTextBoxEx((Rectangle){box_pos + 10 + tw, w_pos_y + text_box_y_dist, text_box_width, text_box_height}, params_text[i], SIZEOF(params_text[i]) - 1, true);
	}

	Rectangle btn_ok_pos = (Rectangle){w_pos_x + text_box_width, w_pos_y + w_height - (text_box_height + 10), text_box_width, text_box_height};
	Rectangle btn_cancel_pos = btn_ok_pos;
	btn_cancel_pos.x = btn_cancel_pos.x + btn_ok_pos.width + 10;	

    bool btn_ok_clicked = GuiButton(btn_ok_pos, "OK");
    bool btn_cancel_clicked = GuiButton(btn_cancel_pos, "CANCEL");

	*apply = btn_ok_clicked;

	return window_closed || btn_ok_clicked || btn_cancel_clicked;

}
