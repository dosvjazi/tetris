#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#define PAUSE_FRAMES 20
#define START_FRAMES 20

#define READY_TO_START -3
#define GAME_OVER -2
#define EXIT -1
#define PLAYING 0
#define PAUSE PAUSE_FRAMES

#define EMPTY 0
#define PLACED 1
#define MOVING 2
#define CENTER 3

#define FIELD_X_START 0
#define FIELD_X_COUNT 10

#define EXTRA_Y_START 0
#define EXTRA_Y_COUNT 1

#define FIELD_Y_START 1
#define FIELD_Y_COUNT 20

#define TOTAL_Y_START 0
#define TOTAL_Y_COUNT 21

#define TETRA_A_START 0
#define TETRA_X_COUNT 4
#define TETRA_Y_COUNT 2
#define TETRA_X_OFFSET 3
#define TETRA_Y_OFFSET 1

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action,
} user_action_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} game_info_t;

typedef enum {
  GameStart = 0,
  Spawn,
  Move,
  Shift,
  Attach,
  GameOver,
} fsm_t;

void user_input(user_action_t action, bool hold);
game_info_t update_current_state();

void fsm(game_info_t *game_info, user_action_t user_action, bool input);

void init_game_info(game_info_t *game_info);

void load_high_score(game_info_t *game_info);
void save_high_score(game_info_t *game_info);

void game_start(fsm_t *fsm, game_info_t *game_info, user_action_t action);
void spawn(fsm_t *fsm, game_info_t *game_info);
void moving(fsm_t *fsm, game_info_t *game_info, user_action_t action,
            int *timer);
void shifting(fsm_t *fsm, game_info_t *game_info);
void attach(fsm_t *fsm, game_info_t *game_info);
void game_over(fsm_t *fsm, game_info_t *game_info, user_action_t action);

void generate_tetromine(game_info_t *game_info);
void place_tetromine(game_info_t *game_info);

void move_right(game_info_t *game_info);
void move_left(game_info_t *game_info);
void rotate(game_info_t *game_info);
bool check_inside(int y, int x, int y_center, int x_center);
bool check_outside(int y, int x, int y_center, int x_center);

bool check_for_game_over(game_info_t *game_info);
void check_for_full_rows(game_info_t *game_info);
void destroy_full_rows(game_info_t *, int start);

void free_memory(game_info_t *game_info);

#endif