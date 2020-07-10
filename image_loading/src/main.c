
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "utils/stb_ds.h"

#include "plugin_config.h"
#include "utils/file_utils.h"
#include "raylib/src/raylib.h"

//TODO: loaded_plugins has to be a hash?
int plugin_index;
char *function_name;

bool show_plugin_params_window = false;
struct plugin_function_param *params = NULL;
static char params_text[256][128] = { 0 };

void DrawTextureEx2(Texture2D texture, Vector2 position, float rotation, float scaleX, float scaleY, Color tint) {
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Rectangle destRec = { position.x, position.y, (float)texture.width*scaleX, (float)texture.height*scaleY };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(texture, sourceRec, destRec, origin, rotation, tint);
}

static void handle_input(Image *images, bool *reload_image) {

	if(!show_plugin_params_window) {
		if(IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown((KEY_LEFT_CONTROL))) {
			if(IsKeyPressed(KEY_Z)) {
				if(arrlen(images) > 1) {
					Image m = arrpop(images);
					UnloadImage(m);
					*reload_image = true;
				}
			}
		}

		if (IsKeyPressed(KEY_S)) {
			show_plugin_params_window = true;
			plugin_index = 1;
			function_name = (char*) "apply_bw";

			if(params != NULL) {
				shfree(params);
				params = NULL;
			}

			return;
		}
		
		if (IsKeyPressed(KEY_G)) {
			show_plugin_params_window = true;
			plugin_index = 0;
			function_name = (char*) "gaussian_filter";

			if(params != NULL) {
				shfree(params);
				params = NULL;
			}

			return;
		}
	}

	return;

}

int main(int argc,  char **argv) {

	if(argc != 2) {
		printf ("Usage: %s image_file\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	const char plugin_dir[] = "./plugins";
	int n_plugins;

	struct plugin_config *all_plugins = init_plugins(plugin_dir, &n_plugins);

	// Initialization
    //--------------------------------------------------------------------------------------
	
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	Image current_image = LoadImage(argv[1]);
    
	InitWindow(current_image.width, current_image.height, "Image loading example");

    SetTargetFPS(60);

    Texture2D texture;
	bool error = true;

	if(current_image.data != NULL) {
		texture = LoadTextureFromImage(current_image); 	    
		error = false;
	}

	float scaleX, scaleY;

	Vector2 position;
	bool reload_image = false;
	Image new_image = { 0 };

	bool apply_plugin = false;

	Image *images = NULL;
	arrput(images, current_image);
	struct function_param_descr *f_pars = NULL;
	while (!WindowShouldClose()) {
		// Draw
		//----------------------------------------------------------------------------------
		//
		handle_input(images, &reload_image);
		BeginDrawing();

		ClearBackground(RAYWHITE);

		if(!error) {

			if(reload_image) {
				UnloadTexture(texture);
				texture = LoadTextureFromImage(arrlast(images)); 	    
				reload_image = false;
			}

			scaleX = ((float)GetScreenWidth()-20)/(float)texture.width;
			scaleY = ((float)GetScreenHeight()-20)/(float)texture.height;
			position = (Vector2){GetScreenWidth()/2.0 - (texture.width/2.0)*scaleX, GetScreenHeight()/2.0 - (texture.height/2.0)*scaleY};
			DrawTextureEx(texture, position, 0, scaleX, WHITE);
		}
		else {
			DrawText("Error loading image", GetScreenWidth()/2, GetScreenHeight()/2, 22, BLACK);
		}


		if(show_plugin_params_window) {
				f_pars = shget(all_plugins[plugin_index].plugin_info.functions, function_name); 
				if(f_pars) {
					show_plugin_params_window = !draw_plugin_params_window(function_name, f_pars, params_text, &apply_plugin);
				}
				else { 
					apply_plugin = true;
					show_plugin_params_window = false;
				}
		}

		if(apply_plugin) {

			shdefault(params, NULL);
			sh_new_arena(params);

			for(int i = 0; i < arrlen(f_pars); i++) {
				shput(params, strdup(f_pars[i].name), strdup(params_text[i]));
			}

			(new_image) = ImageCopy(arrlast(images));
			arrput(images, new_image);

			apply_plugin_fp *apply_function = shget(all_plugins[plugin_index].apply_plugin, function_name);

			(apply_function)(&arrlast(images), params);
			reload_image = true;

			if(params) shfree(params);
			apply_plugin = false;
		}


		EndDrawing();
		//----------------------------------------------------------------------------------
	}

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

	return EXIT_SUCCESS;

}

