#ifndef PLUGIN_CONFIG_H
#define PLUGIN_CONFIG_H

#include "raylib/src/raylib.h"
#include "utils/stb_ds.h"
#include <limits.h>
#include <stdint.h>

struct rgb_pixel {
	uint8_t r;
	uint8_t g;
	uint8_t b;

}__attribute__((packed));

enum param_type {
	INTEGER,
	REAL,
	STRING,
	BOOLEAN
};

//This is used by the caller to configure the plugin parameters
struct plugin_function_param {
	char *key;
	char *value;
};

//This is used by the plugin to describe its parameters
struct function_param_descr {
	char *name;
	char *description;
	enum param_type type;
};

struct plugin_function {
	char *key; //function name
	struct function_param_descr *value; //parameters
	char *description;
};

struct plugin_info {
	char *name;
	char *description;
	struct plugin_function *functions; //plugin functions hash
};

#define INIT_PLUGIN(name) struct plugin_info name()
typedef INIT_PLUGIN(init_plugin_fp);

#define END_PLUGIN(name) void name()
typedef END_PLUGIN(end_plugin_fp);

#define APPLY_PLUGIN(name) void name(Image *img, struct plugin_function_param *params)
typedef APPLY_PLUGIN(apply_plugin_fp);

#define START_PLUGIN_INFO         		\
struct plugin_info info;          		\
info.functions = NULL;            		\
shdefault(info.functions, NULL);  		\
sh_new_arena(info.functions)      		

#define END_PLUGIN_INFO \
return info

#define ADD_PLUGIN_FUNCTION_PARAM(f, n, d, t)                         \
do {									                              \
	struct function_param_descr __par;                                \
	__par.name = strdup(n);                                           \
 	__par.description = strdup(d);                                    \
	__par.type = t;                        				              \
	struct function_param_descr *__params = shget(info.functions, f); \
	arrput(__params, __par);                                          \
	shput(info.functions, strdup(f), __params);                       \
} while(0)

#define ADD_PLUGIN_FUNCTION(f_name)            \
do {										   \
	struct function_param_descr *__v = NULL;   \
	shput(info.functions, strdup(f_name), __v);\
} while(0)

#define SET_PLUGIN_FUNCTION_DESCRIPTION(f, desc)          \
do {                                                      \
	struct plugin_functions pf = shget(info.functions, f);\
	if(pf) {											  \
		pf.description = strdup(desc);                    \
	}                                                     \
	else {                                                \
		fprintf(stderr, "Function %s was not added to the plugin.\n Use the ADD_PLUGIN_FUNCTION(%s) in the plugin code!\n.");\
	} \
while(0)

#define SET_PLUGIN_NAME(p_name) \
	info.name = strdup(p_name)

#define SET_PLUGIN_DESCRIPTION(p_desc) \
	info.description = strdup(p_desc)

struct apply_plugin_hash {
	char key; //function name
	apply_plugin_fp *value; //function pointer
};

struct plugin_config {
	void *handle;
	init_plugin_fp *init_plugin;
	end_plugin_fp *end_plugin;
	struct apply_plugin_hash *apply_plugin;

	bool loaded;
	struct plugin_info plugin_info;
};

struct plugin_config * init_plugins(const char *plugin_dir, int *num_plugins);
struct plugin_param *configure_plugin_params(struct plugin_info info);
bool draw_plugin_params_window(char *function_name, struct function_param_descr *params,  char params_text[256][128], bool *apply);

#endif
