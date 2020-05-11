#include "plugin_config.h"
#include "utils/stb_ds.h"
#include "utils/file_utils.h"
#include <dlfcn.h>
#include <stdio.h>

struct plugin_config * init_plugins(const char *plugin_dir, int *num_plugins) {

	string_array available_plugins = list_files_from_dir(plugin_dir, "plugin_");

	*num_plugins = arrlen(available_plugins);

	struct plugin_config *loaded_plugins = (struct plugin_config*) calloc(*num_plugins, sizeof(struct plugin_config));
	
	for(int i = 0; i < arrlen(available_plugins); i++) {

		char *full_plugin_path = malloc(strlen(plugin_dir)+strlen(available_plugins[i]+2));
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

			loaded_plugins[i].apply_plugin = dlsym(loaded_plugins[i].handle, loaded_plugins[i].plugin_info.function_name);

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

		free(full_plugin_path);
	}

	return loaded_plugins;
}

struct plugin_param *configure_plugin_params(struct plugin_info info) {

	struct plugin_param *params = NULL;
	sh_new_arena(params);
	shdefault(params, NULL);

	int n = arrlen(info.params);

	for(int i = 0; i < n; i++) {
		printf("Param %s (%s): ", info.params[i].name, info.params[i].description);
		char v[10];
		scanf("%s", v);
	
		shput(params, strdup(info.params[i].name), strdup(v));
	}

	//TODO: percorrer vetor de parametros e criar uma janela para o usuário passar o parametro

	return params;

}
