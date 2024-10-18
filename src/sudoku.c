#define OLIVEC_IMPLEMENTATION
#include "olive.c"

#include "themes/frappe.c"
#include "themes/mocha.c"
#include "themes/latte.c"
#include "themes/macchiato.c"

#include "assets/heart.c"
#include "assets/font.c"

#define TRUE 1
#define FALSE 0

#define FIELD_WIDTH 9
#define FIELD_HEIGHT 9
#define FIELD_CAP (FIELD_HEIGHT * FIELD_WIDTH)

#define SEP_COUNT 4
#define SEP_SIZE 5

#define CELL_SEP_COUNT 6
#define CELL_SEP_SIZE 2

#define TOP_MARGIN 50
#define CELL_SIZE 60
#define OC_WIDTH  ((CELL_SIZE * FIELD_WIDTH) + (SEP_SIZE * SEP_COUNT) + (CELL_SEP_COUNT * CELL_SEP_SIZE) + 0)
#define OC_HEIGHT ((CELL_SIZE * FIELD_HEIGHT) + (SEP_SIZE * SEP_COUNT) + (CELL_SEP_COUNT * CELL_SEP_SIZE) + TOP_MARGIN)

#define DIGIT_1   1
#define DIGIT_2   2
#define DIGIT_3   3
#define DIGIT_4   4
#define DIGIT_5   5
#define DIGIT_6   6
#define DIGIT_7   7
#define DIGIT_8   8
#define DIGIT_9   9
#define MOVE_UP    10
#define MOVE_DOWN  11
#define MOVE_LEFT  12
#define MOVE_RIGHT 13

int rand(void);

uint32_t pixels[OC_WIDTH*OC_HEIGHT];

static int field[FIELD_WIDTH*FIELD_HEIGHT] = {0};
static int visible_field[FIELD_WIDTH*FIELD_HEIGHT] = {0};

static int cursor = 0;
static int hearts = 3;

typedef enum {
    FRAPPE = 0,
    LATTE,
    MACCHIATO,
    MOCHA
} Colorscheme;

typedef enum {
    BACKGROUND_COLOR = 0,
    SEP_COLOR,
    CELL_SEP_COLOR,
    CURSOR_COLOR,
    CURSOR_SIMILAR,
    SUB_CURSOR_COLOR,
    BASE_TEXT_COLOR,
    SELECTED_TEXT_COLOR,
    WIN_TEXT_COLOR,
    LOSE_TEXT_COLOR,
    HEART_CONTOUR,
    HEART_RED,
    LAVENDER,
    MANTLE
} Colors;

uint32_t *themes[] = {frappe_theme, latte_theme, macchiato_theme, mocha_theme}; 
uint32_t *theme = frappe_theme;

typedef enum {
    EASY = 0,
    MEDIUM,
    HARD,
    DIFFICULTY_COUNT
} Difficulty;

static Difficulty difficulty;
int opened_cells_counts[DIFFICULTY_COUNT] = {38, 30, 23};


typedef enum {
    PLAY,
    LOSE,
    WIN
} state_t;

static state_t state = PLAY;

typedef enum {
    NOTHING,
    MISTAKE,
    OPEN_CELL
} action_t;

int is_save(int cell, int num)
{
    int row = cell / FIELD_WIDTH,
        col = cell % FIELD_WIDTH;

    /* check row for num */
    for (int x = 0; x < FIELD_WIDTH; x++)
        if (field[row*FIELD_WIDTH + x] == num)
            return FALSE;

    /* check column for num*/
    for (int y = 0; y < FIELD_WIDTH; y++)
        if (field[y*FIELD_WIDTH + col] == num)
            return FALSE;

    /* check square */
    int start_row = row - row % 3,
        start_col = col - col % 3;
    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
               if (field[(start_row + y) * FIELD_WIDTH + start_col + x] == num)
                  return FALSE;
    return TRUE;
}


int is_square_checked(int cells[FIELD_WIDTH])
{
    for (int i = 0; i < FIELD_WIDTH; i++)
        if (cells[i] != TRUE)
            return FALSE;
    return TRUE;
}


void clear_field(void)
{
    for (int i = 0; i < FIELD_CAP; i++) {
        field[i] = 0;
        visible_field[i] = FALSE;
    }
}


void clear_field_from_number(int number)
{
    for (int i = 0; i < FIELD_CAP; i++)
        if (field[i] == number)
            field[i] = 0;
}

#if 0
void print_field()
{
    for (int i = 0; i < FIELD_CAP; i++)
    {
        printf("%d ", field[i]);
        if (i > 0 && i % FIELD_WIDTH == 8)
            printf("\n");
    }
    printf("\n");
}
#endif

void generate_field(void)
{
    int back_count = 0;

    for (int i = 1; i <= 9; i++)
    {
        if (back_count >= 2) {
            clear_field();
            back_count = 0;
            i = 1;
        }

        int is_leave = FALSE;
        for (int y = 0; y < 9 && !is_leave; y+=3)
        {
            for (int x = 0; x < 9 && !is_leave; x+=3)
            {
                int checked_cells[FIELD_WIDTH] = {0};
                while (!is_leave)
                {
                    int cell_cord = rand() % 9;
                    if (is_square_checked(checked_cells) == TRUE) {
                        clear_field_from_number(i);
                        i -= 1;
                        back_count += 1;
                        is_leave = TRUE;
                        break;
                    }

                    if (checked_cells[cell_cord] != 0)
                        continue;

                    int cell_y = cell_cord / 3,
                        cell_x = cell_cord % 3,
                        cell   = ((y + cell_y) * FIELD_WIDTH) + x + cell_x;

                    if (field[cell] != 0) {
                        checked_cells[cell_cord] = 1;
                    } else {
                        if (is_save(cell, i) == TRUE) {
                            field[cell] = i;
                            break;
                        } else {
                            checked_cells[cell_cord] = 1;
                        }
                    }
                }
            }
        }
    }
}

void open_random_cells(void)
{
    int count = opened_cells_counts[difficulty];

    for (int i = 0; i < FIELD_CAP; i++)
        visible_field[i] = FALSE;

    for (int i = 0; i < count; i++)
    {
        int cell = rand() % FIELD_CAP;
        while (visible_field[cell] != FALSE)
            cell = rand() % FIELD_CAP;

        visible_field[cell] = TRUE;
    }
}

void init_game(void)
{
    clear_field();
    generate_field();
    open_random_cells();
    hearts = 3;
    state = PLAY;
}

void reset_field(void)
{
    clear_field();
    init_game();
}

void change_difficulty(Difficulty dif)
{
    difficulty = dif;
    init_game();
}

void change_colorscheme(Colorscheme cs)
{
    theme = themes[cs];
}

// TODO: Rewrite this shit
void render_play_screen(Olivec_Canvas *oc)
{
    #define SQUARE_SIZE (3 * CELL_SIZE + SEP_SIZE + 2 * CELL_SEP_SIZE)

    /* draw hearts */
    {
        uint32_t heart_pixels[HEART_WIDTH*HEART_HEIGHT];
        for (int i = 0; i < HEART_HEIGHT*HEART_WIDTH; i++)
            if (heart_template[i] == 0x00000000)
                heart_pixels[i] = theme[BACKGROUND_COLOR];
            else if (heart_template[i] == 0xFF000000)
                heart_pixels[i] = theme[HEART_CONTOUR];
            else if (heart_template[i] == 0xFF0000FF)
                heart_pixels[i] = theme[HEART_RED];

        Olivec_Canvas heart_sprite = olivec_canvas(heart_pixels, HEART_WIDTH, HEART_HEIGHT, HEART_WIDTH);
        int start_x = 5;
        int margin = 45;
        int y = 5;
        for (int i = 0; i < hearts; i++)
            olivec_sprite_copy((*oc), i * margin + start_x, y, 40, 40, heart_sprite);
    }

    /* draw cursor */
    {
        int cursor_line = cursor / FIELD_WIDTH;
        int cursor_column = cursor % FIELD_WIDTH;

        int cursor_vert_square = cursor_line / 3;
        int cursor_hor_square = cursor_column / 3;

        int cursor_x = cursor_hor_square * SQUARE_SIZE + cursor_column % 3 * (CELL_SIZE + CELL_SEP_SIZE) + SEP_SIZE;
        int cursor_y = cursor_vert_square * SQUARE_SIZE + cursor_line % 3 * (CELL_SIZE + CELL_SEP_SIZE) + SEP_SIZE + TOP_MARGIN;

        for (int i = 0; i < FIELD_WIDTH; i++)
        {
            int hor_square = i / 3;
            int x = hor_square * SQUARE_SIZE + i % 3 * (CELL_SIZE + CELL_SEP_SIZE) + SEP_SIZE;
            olivec_rect((*oc), x, cursor_y, CELL_SIZE, CELL_SIZE, theme[SUB_CURSOR_COLOR]);
        }

        for (int i = 0; i < FIELD_HEIGHT; i++)
        {
            int vert_square = i / 3;
            int y = vert_square * SQUARE_SIZE + i % 3 * (CELL_SIZE + CELL_SEP_SIZE) + SEP_SIZE + TOP_MARGIN;
            olivec_rect((*oc), cursor_x, y, CELL_SIZE, CELL_SIZE, theme[SUB_CURSOR_COLOR]);
        }

        int start_line = cursor_line - cursor_line % 3,
            start_column = cursor_column - cursor_column % 3;
        for (int line = start_line; line < start_line + 3; line++)
        {
            for (int column = start_column; column < start_column + 3; column++)
            {
                int vert_square = line / 3;
                int hor_square = column / 3;
                int x = hor_square * SQUARE_SIZE + column % 3 * (CELL_SIZE + CELL_SEP_SIZE) + SEP_SIZE;
                int y = vert_square * SQUARE_SIZE + line % 3 * (CELL_SIZE + CELL_SEP_SIZE) + SEP_SIZE + TOP_MARGIN;
                olivec_rect((*oc), x, y, CELL_SIZE, CELL_SIZE, theme[SUB_CURSOR_COLOR]);
            }
        }

        for (int line = 0; line < FIELD_HEIGHT; line++)
        {
            for (int column = 0; column < FIELD_WIDTH; column++)
            {
                int cell = line * FIELD_WIDTH + column;
                if (!visible_field[cell] || !visible_field[cursor])
                    continue;

                if (field[cell] != field[cursor])
                    continue;

                int vert_square = line / 3;
                int hor_square = column / 3;
                int x = hor_square * SQUARE_SIZE + column % 3 * (CELL_SIZE + CELL_SEP_SIZE) + SEP_SIZE;
                int y = vert_square * SQUARE_SIZE + line % 3 * (CELL_SIZE + CELL_SEP_SIZE) + SEP_SIZE + TOP_MARGIN;
                olivec_rect((*oc), x, y, CELL_SIZE, CELL_SIZE, theme[CURSOR_SIMILAR]);
            }
        }
        olivec_rect((*oc), cursor_x, cursor_y, CELL_SIZE, CELL_SIZE, theme[CURSOR_COLOR]);
    }

    /* draw numbers */
    int font_size = 5;
    int font_height = olivec_default_font.height * font_size;
    int font_width = olivec_default_font.width * font_size;
    char *labels[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    uint32_t text_colors[2] = {theme[BASE_TEXT_COLOR], theme[SELECTED_TEXT_COLOR]};

    for (int line = 0; line < FIELD_HEIGHT; line++)
    {
        for (int column = 0; column < FIELD_WIDTH; column++)
        {
            int cell = line * FIELD_WIDTH + column;

            if (visible_field[cell] == FALSE)
                continue;

            uint32_t text_color = text_colors[field[cell] == field[cursor] * visible_field[cursor]];
            int vert_square = line / 3;
            int hor_square = column / 3;
            int x = (hor_square * SQUARE_SIZE) + column % 3 * (CELL_SIZE + CELL_SEP_SIZE) + SEP_SIZE + ((CELL_SIZE - font_width) / 2) + (font_width / 5);
            int y = (vert_square * SQUARE_SIZE) + line % 3 * (CELL_SIZE + CELL_SEP_SIZE) + SEP_SIZE + (CELL_SIZE - font_height) / 2 + TOP_MARGIN;

            olivec_text((*oc), labels[field[cell]], x, y, olivec_default_font, font_size, text_color);
        }
    }

    /* draw cell's borders */
    for (int s = 0; s < 3; s++)
    {
        for (int i = 0; i < 2; i++)
        {
            /* vertical */
            int x = i * (CELL_SEP_SIZE + CELL_SIZE) + (SEP_SIZE + CELL_SIZE) + s * SQUARE_SIZE;
            int height = (FIELD_HEIGHT * CELL_SIZE) + (SEP_COUNT * SEP_SIZE) + (CELL_SEP_COUNT * CELL_SEP_SIZE);
            olivec_rect((*oc), x, TOP_MARGIN, CELL_SEP_SIZE, height, theme[CELL_SEP_COLOR]);
            /* horizontal */
            int y = i * (CELL_SEP_SIZE + CELL_SIZE) + (SEP_SIZE + CELL_SIZE) + s * (3 * CELL_SIZE + SEP_SIZE + 2 * CELL_SEP_SIZE) + TOP_MARGIN;
            int width= (FIELD_HEIGHT * CELL_SIZE) + (SEP_COUNT * SEP_SIZE) + (CELL_SEP_COUNT * CELL_SEP_SIZE);
            olivec_rect((*oc), 0, y, width, CELL_SEP_SIZE, theme[CELL_SEP_COLOR]);
        }
    }

    /* draw square's borders */
    for (int i = 0; i < SEP_COUNT; i++)
    {
        /* vertical */
        int x = i * ((CELL_SIZE * 3) + SEP_SIZE + (CELL_SEP_SIZE * 2));
        int height = (FIELD_HEIGHT * CELL_SIZE) + (SEP_COUNT * SEP_SIZE) + (CELL_SEP_COUNT * CELL_SEP_SIZE);
        olivec_rect((*oc), x, TOP_MARGIN, SEP_SIZE, height, theme[SEP_COLOR]);
        /* horizontal */
        int y = i * ((CELL_SIZE * 3) + SEP_SIZE + (CELL_SEP_SIZE * 2)) + TOP_MARGIN;
        int width = (FIELD_WIDTH * CELL_SIZE) + (SEP_COUNT * SEP_SIZE) + (CELL_SEP_COUNT * CELL_SEP_SIZE);
        olivec_rect((*oc), 0, y, width, SEP_SIZE, theme[SEP_COLOR]);
    }
}

void render_win_screen(Olivec_Canvas *oc) {
        int width = OC_WIDTH;
        int height = 200;
        olivec_rect((*oc), (OC_WIDTH - width) / 2, (OC_HEIGHT - height) / 2, width, height, theme[MANTLE]);
        olivec_frame((*oc), (OC_WIDTH - width) / 2, (OC_HEIGHT - height) / 2, width, height, 5, theme[LAVENDER]);

        int font_size = 5;
        const char *win_text = "You win :)";
        Olivec_Vector2 text_size = olivec_measure_text(win_text, sudoku_font, font_size);
        olivec_text((*oc), win_text, (OC_WIDTH - text_size.x) /2 , (OC_HEIGHT - text_size.y) / 2, sudoku_font, font_size, theme[WIN_TEXT_COLOR]);
}

void render_lose_screen(Olivec_Canvas *oc) {
        int width = OC_WIDTH;
        int height = OC_HEIGHT;
        olivec_frame((*oc), 0 + (float)5/2, (OC_HEIGHT - height) / 2, width - 1, height, 5, theme[LAVENDER]);

        int font_size = 5;
        const char *lose_text = "You lose :(";
        Olivec_Vector2 text_size = olivec_measure_text(lose_text, sudoku_font, font_size);
        olivec_text((*oc), "You lose:(", (OC_WIDTH - text_size.x) /2 , (OC_HEIGHT - text_size.y) / 2, sudoku_font, font_size, theme[LOSE_TEXT_COLOR]);
}

Olivec_Canvas render_game(void)
{
    Olivec_Canvas oc = olivec_canvas(pixels, OC_WIDTH, OC_HEIGHT, OC_WIDTH);
    olivec_fill(oc, theme[BACKGROUND_COLOR]);

    switch (state) {
        case PLAY: {
            render_play_screen(&oc);
            break;
        }
        case WIN: {
            render_win_screen(&oc);
            break;
        }
        case LOSE: {
            render_lose_screen(&oc);
            break;
        }
    }

    return oc;
}


void update_state(action_t action)
{
    switch(action) {
        case MISTAKE:
            if (hearts == 0)
                state = LOSE;
            break;
        case OPEN_CELL:
            for (int i = 0; i < FIELD_CAP; i++)
                if (visible_field[i] == FALSE)
                    return;
            state = WIN;
        default:
            break;
    }
}

void keydown(int key)
{
    if (state != PLAY)
        return;

    if (key == MOVE_UP) {
        if (cursor - FIELD_WIDTH >= 0)
            cursor -= FIELD_WIDTH;
    } else if (key == MOVE_DOWN) {
        if (cursor + FIELD_WIDTH < FIELD_CAP)
            cursor += FIELD_WIDTH;
    } else if (key == MOVE_LEFT) {
        if (cursor - 1 >= 0 && (cursor - 1) % FIELD_WIDTH != FIELD_WIDTH - 1)
            cursor -= 1;
    } else if (key == MOVE_RIGHT) {
        if (cursor + 1 < FIELD_CAP && (cursor + 1) % FIELD_WIDTH != 0)
            cursor += 1;
    } else if (key >= DIGIT_1 && key <= DIGIT_9) {
        if (visible_field[cursor] != FALSE)
            return;

        int number = key;
        if (field[cursor] == number) {
            visible_field[cursor] = TRUE;
            update_state(OPEN_CELL);
        } else {
            hearts -= 1;
            update_state(MISTAKE);
        }
        return;
    }
}
