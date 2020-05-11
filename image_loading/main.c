
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "utils/stb_ds.h"

#include "plugin_config.h"
#include "utils/file_utils.h"
#include "raylib/src/raylib.h"

void DrawTextureEx2(Texture2D texture, Vector2 position, float rotation, float scaleX, float scaleY, Color tint) {
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Rectangle destRec = { position.x, position.y, (float)texture.width*scaleX, (float)texture.height*scaleY };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(texture, sourceRec, destRec, origin, rotation, tint);
}

static void handle_input(Image *img, Image *new_img, struct plugin_config *plugin_configs, int plugin_index, bool *reload_image) {

	if(IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown((KEY_LEFT_CONTROL))) {
        if(IsKeyPressed(KEY_Z)) {
			(*new_img) = ImageCopy(*img);
			*reload_image = true;
		}
	}

	if (IsKeyPressed(KEY_S)) {

		if((*new_img).data) {
			free((*new_img).data);
		}

		(*new_img) = ImageCopy(*img);

		//TODO: configurar os parametros para o plugin escolhido
		struct plugin_param *params = configure_plugin_params(plugin_configs[plugin_index].plugin_info);

		(plugin_configs[plugin_index].apply_plugin)(new_img, params);
		*reload_image = true;
		return;
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

	Image image = LoadImage(argv[1]);
    
	InitWindow(image.width, image.height, "Image loading example");

    Texture2D texture;
	bool error = true;

	if(image.data != NULL) {
		texture = LoadTextureFromImage(image); 	    
		error = false;
	}

	float scaleX, scaleY;

	Vector2 position;
	bool reload_image = false;
	Image new_image = { 0 };

    while (!WindowShouldClose()) {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

			handle_input(&image, &new_image, all_plugins, 0, &reload_image);

            ClearBackground(RAYWHITE);

			if(!error) {

				if(reload_image) {
    				UnloadTexture(texture);
					texture = LoadTextureFromImage(new_image); 	    
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

