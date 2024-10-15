#include <stdlib.h>
#include <time.h>
#include "sudoku.c"
#include <raylib.h>


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
    }
}


int main(void)
{
    srand(time(NULL));
    InitWindow(OC_WIDTH, OC_HEIGHT, "Wudoku native");
    SetTargetFPS(60);
    init_game();
    while (!WindowShouldClose()) {
        BeginDrawing();
            for (int y = 0; y < OC_HEIGHT; y++) {
                for (int x = 0; x < OC_WIDTH; x++) {
                    uint32_t color = pixels[y*OC_WIDTH+x];
                    char r = OLIVEC_RED(color);
                    char g = OLIVEC_GREEN(color);
                    char b = OLIVEC_BLUE(color);
                    char a = OLIVEC_ALPHA(color);
                    DrawPixel(x, y, CLITERAL(Color){r,g,b,a});
                }
            } 
            process_input(); 
            render_game();
        EndDrawing(); 
    }
    CloseWindow();
    return 0;
}
