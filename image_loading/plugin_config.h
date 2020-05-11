#ifndef PLUGIN_CONFIG_H
#define PLUGIN_CONFIG_H

#include "raylib/src/raylib.h"

#include "utils/stb_ds.h"

enum plugin_type {
	INTEGER,
	REAL,
	STRING,
	BOOLEAN
};

struct plugin_param_descr {
	char *name;
	char *description;
	enum plugin_type type;
};

struct plugin_param {
	char *key;
	char *value;
};

struct plugin_info {
	char *name;
	char *description;
	char *function_name;
	struct plugin_param_descr *params;
};

#define INIT_PLUGIN(name) struct plugin_info name()
typedef INIT_PLUGIN(init_plugin_fp);

#define END_PLUGIN(name) void name()
typedef END_PLUGIN(end_plugin_fp);

#define APPLY_PLUGIN(name) void name(Image *img, struct plugin_param *params)
typedef APPLY_PLUGIN(apply_plugin_fp);

struct plugin_config {
	void *handle;
	init_plugin_fp *init_plugin;
	end_plugin_fp *end_plugin;
	apply_plugin_fp *apply_plugin;

	bool loaded;
	struct plugin_info plugin_info;
};

struct plugin_config * init_plugins(const char *plugin_dir, int *num_plugins);
struct plugin_param *configure_plugin_params(struct plugin_info info);

#endif
