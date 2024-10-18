#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "sudoku.c"
#include <raylib.h>
#include <math.h>


int keys[][3] = {
    {KEY_ONE,   KEY_KP_1,  1}, 
    {KEY_TWO,   KEY_KP_2,  2}, 
    {KEY_THREE, KEY_KP_3,  3}, 
    {KEY_FOUR,  KEY_KP_4,  4}, 
    {KEY_FIVE,  KEY_KP_5,  5}, 
    {KEY_SIX,   KEY_KP_6,  6}, 
    {KEY_SEVEN, KEY_KP_7,  7}, 
    {KEY_EIGHT, KEY_KP_8,  8}, 
    {KEY_NINE,  KEY_KP_9,  9}, 
    {KEY_W,     KEY_UP,    10}, 
    {KEY_S,     KEY_DOWN,  11}, 
    {KEY_A,     KEY_LEFT,  12}, 
    {KEY_D,     KEY_RIGHT, 13}, 
};


void process_input(void)
{
    for (size_t i = 0; i <= sizeof(keys); i++) {
        if (IsKeyPressed(keys[i][0]) || IsKeyPressed(keys[i][1])) {
            keydown(keys[i][2]);
        }       
    }
    
    if (IsKeyPressed(KEY_R)) {
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
