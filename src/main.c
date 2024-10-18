#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "sudoku.c"
#include <raylib.h>
#include <math.h>

void process_input(void)
{
    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
        keydown(DIGIT_1);  
    } else if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
        keydown(DIGIT_2); 
    } else if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) {
        keydown(DIGIT_3);
    } else if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) {
        keydown(DIGIT_4);
    } else if (IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_KP_5)) {
        keydown(DIGIT_5);
    } else if (IsKeyPressed(KEY_SIX) || IsKeyPressed(KEY_KP_6)) {
        keydown(DIGIT_6);
    } else if (IsKeyPressed(KEY_SEVEN) || IsKeyPressed(KEY_KP_7)) {
        keydown(DIGIT_7);
    } else if (IsKeyPressed(KEY_EIGHT) || IsKeyPressed(KEY_KP_8)) {
        keydown(DIGIT_8);
    } else if (IsKeyPressed(KEY_NINE) || IsKeyPressed(KEY_KP_9)) {
        keydown(DIGIT_9);
    } else if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        keydown(MOVE_UP); 
    } else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
        keydown(MOVE_RIGHT); 
    } else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        keydown(MOVE_DOWN); 
    } else if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
        keydown(MOVE_LEFT); 
    } else if (IsKeyPressed(KEY_R)) {
        reset_field();
    }
}


int main(void)
{
    srand(time(NULL));
    InitWindow(OC_WIDTH, OC_HEIGHT, "Wudoku native");
    SetTargetFPS(60);
    init_game();
    change_colorscheme(MOCHA); 
    
    Shader shader = LoadShader(NULL, "invert_color.fs"); 

    Image image = GenImageColor(OC_WIDTH, OC_HEIGHT, WHITE);
    Texture texture = LoadTextureFromImage(image);
    UnloadImage(image);

    while (!WindowShouldClose()) {
        BeginDrawing();
            render_game();
            process_input(); 
            UpdateTexture(texture, &pixels);

            int x = GetScreenWidth()/2 - texture.width/2;
            int y = GetScreenHeight()/2 - texture.height/2;
            BeginShaderMode(shader);
                DrawTexture(texture, x, y, RED);
            EndShaderMode();

        EndDrawing(); 
    }
    UnloadTexture(texture);
    UnloadShader(shader);
    CloseWindow();
    return 0;
}
