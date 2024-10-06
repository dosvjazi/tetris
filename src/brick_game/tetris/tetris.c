#include "tetris.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

const int speed_list[10] = {60, 45, 30, 20, 10, 5, 4, 3, 2, 1};
const int tetromine_list[7][2][4] = {
    {{2, 2, 3, 2}, {0, 0, 0, 0}}, {{0, 2, 0, 0}, {0, 2, 3, 2}},
    {{0, 0, 0, 2}, {0, 2, 3, 2}}, {{0, 2, 2, 0}, {0, 2, 2, 0}},
    {{0, 2, 3, 0}, {0, 0, 2, 2}}, {{0, 0, 2, 0}, {0, 2, 3, 2}},
    {{0, 0, 3, 2}, {0, 2, 2, 0}},
};

void user_input(user_action_t action, bool hold) { fsm(NULL, action, hold); }

game_info_t update_current_state() {
  static game_info_t game_info;

  if (game_info.field == NULL) init_game_info(&game_info);
  fsm(&game_info, 0, false);

  return game_info;
}

void fsm(game_info_t *game_info, user_action_t user_action, bool input) {
  static int timer = 0;
  static user_action_t action = -1;
  static fsm_t fsm = GameStart;

  if (input) {
    action = user_action;
  } else {
    fsm_t last_fsm = fsm;
    switch (fsm) {
      case GameStart:
        game_start(&fsm, game_info, action);
        break;
      case Spawn:
        spawn(&fsm, game_info);
        break;
      case Move:
        moving(&fsm, game_info, action, &timer);
        break;
      case Shift:
        shifting(&fsm, game_info);
        break;
      case Attach:
        attach(&fsm, game_info);
        break;
      case GameOver:
        game_over(&fsm, game_info, action);
        break;
    }

    if (action == Pause && game_info->pause == PLAYING)
      game_info->pause = PAUSE_FRAMES;

    if (action == Terminate) {
      free_memory(game_info);
      game_info->pause = EXIT;
    }

    if (last_fsm == Move && action != 0) action = -1;
  }
}

void init_game_info(game_info_t *game_info) {
  srand(time(NULL));

  game_info->field = calloc(TOTAL_Y_COUNT, sizeof(int *));
  for (int i = TOTAL_Y_START; i < TOTAL_Y_COUNT; i++)
    game_info->field[i] = calloc(FIELD_X_COUNT, sizeof(int));

  game_info->next = calloc(TETRA_Y_COUNT, sizeof(int *));
  for (int i = TETRA_A_START; i < TETRA_Y_COUNT; i++)
    game_info->next[i] = calloc(TETRA_X_COUNT, sizeof(int));

  game_info->score = 0;
  game_info->high_score = 0;
  game_info->level = 0;
  game_info->speed = speed_list[0];
  game_info->pause = READY_TO_START;

  load_high_score(game_info);
}

void load_high_score(game_info_t *game_info) {
  FILE *file = fopen("high_score.txt", "r");
  if (file != NULL) {
    fscanf(file, "%d", &game_info->high_score);
    fclose(file);
  }
}

void save_high_score(game_info_t *game_info) {
  FILE *file = fopen("high_score.txt", "w");
  fprintf(file, "%d", game_info->high_score);
  fclose(file);
}

void game_start(fsm_t *fsm, game_info_t *game_info, user_action_t action) {
  if (action == Start) {
    game_info->pause = PLAYING;
    generate_tetromine(game_info);
    *fsm = Spawn;
  }
}

void spawn(fsm_t *fsm, game_info_t *game_info) {
  place_tetromine(game_info);
  generate_tetromine(game_info);
  *fsm = Move;
}

void moving(fsm_t *fsm, game_info_t *game_info, user_action_t action,
            int *timer) {
  if (action == Right) move_right(game_info);
  if (action == Left) move_left(game_info);
  if (action == Action) rotate(game_info);

  *timer = *timer + 1;

  if (game_info->pause > PLAYING) game_info->pause = game_info->pause - 1;

  if (action == Down) {
    while (*fsm != Attach) shifting(fsm, game_info);
    *timer = 0;
  }

  if (*timer == game_info->speed) {
    *timer = 0;
    *fsm = Shift;
  }
}

void shifting(fsm_t *fsm, game_info_t *game_info) {
  bool can_shift = true;

  for (int x = FIELD_X_START; x < FIELD_X_COUNT; x++) {
    for (int y = TOTAL_Y_COUNT - 2; y >= TOTAL_Y_START; y--) {
      if (game_info->field[y][x] >= MOVING && game_info->field[y + 1][x] == 1)
        can_shift = false;
    }
    for (int y = TOTAL_Y_COUNT - 1; y >= TOTAL_Y_START + 1; y--) {
      if (game_info->field[y][x] >= MOVING && y == TOTAL_Y_COUNT - 1)
        can_shift = false;
    }
  }

  if (can_shift) {
    for (int y = TOTAL_Y_COUNT - 1; y > TOTAL_Y_START; y--) {
      for (int x = FIELD_X_START; x < FIELD_X_COUNT; x++) {
        if (game_info->field[y - 1][x] >= MOVING) {
          game_info->field[y][x] = game_info->field[y - 1][x];
          game_info->field[y - 1][x] = EMPTY;
        }
      }
    }

    *fsm = Move;
  } else {
    *fsm = Attach;
  }
}

void attach(fsm_t *fsm, game_info_t *game_info) {
  for (int x = FIELD_X_START; x < FIELD_X_COUNT; x++) {
    for (int y = TOTAL_Y_START; y < TOTAL_Y_COUNT; y++) {
      if (game_info->field[y][x] >= MOVING) game_info->field[y][x] = 1;
    }
  }

  check_for_full_rows(game_info);

  if (!check_for_game_over(game_info))
    *fsm = Spawn;
  else
    *fsm = GameOver;
}

void game_over(fsm_t *fsm, game_info_t *game_info, user_action_t action) {
  if (game_info->high_score < game_info->score) {
    game_info->high_score = game_info->score;
    save_high_score(game_info);
  }

  game_info->pause = GAME_OVER;

  if (action == Start) {
    free_memory(game_info);
    init_game_info(game_info);
    game_info->pause = GAME_OVER;
    *fsm = GameStart;
  }
}

void generate_tetromine(game_info_t *game_info) {
  float r = (float)rand() / RAND_MAX;
  int i = (int)(r * 7);

  for (int y = TETRA_A_START; y < TETRA_Y_COUNT; y++) {
    for (int x = TETRA_A_START; x < TETRA_X_COUNT; x++) {
      game_info->next[y][x] = tetromine_list[i][y][x];
    }
  }
}

void place_tetromine(game_info_t *game_info) {
  for (int y = TETRA_A_START; y < TETRA_Y_COUNT; y++) {
    for (int x = TETRA_A_START; x < TETRA_X_COUNT; x++) {
      game_info->field[y + TETRA_Y_OFFSET][x + TETRA_X_OFFSET] =
          game_info->next[y][x];
    }
  }
}

void move_right(game_info_t *game_info) {
  bool can_move = true;

  for (int x = FIELD_X_START; x < FIELD_X_COUNT; x++) {
    for (int y = TOTAL_Y_START; y < TOTAL_Y_COUNT; y++) {
      if (game_info->field[y][x] >= MOVING && x == FIELD_X_COUNT - 1)
        can_move = false;
      if (game_info->field[y][x] >= MOVING && x < FIELD_X_COUNT - 1 &&
          game_info->field[y][x + 1] == 1)
        can_move = false;
    }
  }

  if (can_move) {
    for (int x = FIELD_X_COUNT - 1; x > FIELD_X_START; x--) {
      for (int y = TOTAL_Y_START; y < TOTAL_Y_COUNT; y++) {
        if (game_info->field[y][x - 1] >= MOVING) {
          game_info->field[y][x] = game_info->field[y][x - 1];
          game_info->field[y][x - 1] = EMPTY;
        }
      }
    }
  }
}

void move_left(game_info_t *game_info) {
  bool can_move = true;

  for (int x = FIELD_X_START; x < FIELD_X_COUNT; x++) {
    for (int y = TOTAL_Y_START; y < TOTAL_Y_COUNT; y++) {
      if (game_info->field[y][x] >= MOVING && x == FIELD_X_START)
        can_move = false;
      if (game_info->field[y][x] >= MOVING && x > FIELD_X_START &&
          game_info->field[y][x - 1] == 1)
        can_move = false;
    }
  }

  if (can_move) {
    for (int x = FIELD_X_START; x < FIELD_X_COUNT - 1; x++) {
      for (int y = TOTAL_Y_START; y < TOTAL_Y_COUNT; y++) {
        if (game_info->field[y][x + 1] >= MOVING) {
          game_info->field[y][x] = game_info->field[y][x + 1];
          game_info->field[y][x + 1] = EMPTY;
        }
      }
    }
  }
}

void rotate(game_info_t *game_info) {
  int x_center = -1;
  int y_center = -1;

  for (int y = FIELD_Y_START; y < TOTAL_Y_COUNT; y++) {
    for (int x = FIELD_X_START; x < FIELD_X_COUNT; x++) {
      if (game_info->field[y][x] == CENTER) {
        x_center = x;
        y_center = y;
      }
    }
  }

  int temp[5][5];

  for (int x = 0; x < 5; x++) {
    for (int y = 0; y < 5; y++) {
      if (check_inside(y, x, y_center, x_center) &&
          game_info->field[y + y_center - 2][x + x_center - 2] >= MOVING)
        temp[x][4 - y] = game_info->field[y + y_center - 2][x + x_center - 2];
      else
        temp[x][4 - y] = 0;
    }
  }

  bool can_rotate = true;

  for (int x = 0; x < 5; x++) {
    for (int y = 0; y < 5; y++) {
      if (temp[y][x] >= MOVING && check_outside(y, x, y_center, x_center))
        can_rotate = false;
      if (check_inside(y, x, y_center, x_center) && temp[y][x] >= MOVING &&
          game_info->field[y + y_center - 2][x + x_center - 2] == PLACED)
        can_rotate = false;
    }
  }

  if (can_rotate) {
    for (int x = 0; x < 5; x++) {
      for (int y = 0; y < 5; y++) {
        if (check_inside(y, x, y_center, x_center) &&
            game_info->field[y + y_center - 2][x + x_center - 2] != PLACED)
          game_info->field[y + y_center - 2][x + x_center - 2] = temp[y][x];
      }
    }
  }
}

bool check_inside(int y, int x, int y_center, int x_center) {
  return y + y_center - 2 >= TOTAL_Y_START &&
         y + y_center - 2 < TOTAL_Y_COUNT &&
         x + x_center - 2 >= FIELD_X_START && x + x_center - 2 < FIELD_X_COUNT;
}
bool check_outside(int y, int x, int y_center, int x_center) {
  return y + y_center - 2 < TOTAL_Y_START ||
         y + y_center - 2 >= TOTAL_Y_COUNT ||
         x + x_center - 2 < FIELD_X_START || x + x_center - 2 >= FIELD_X_COUNT;
}

bool check_for_game_over(game_info_t *game_info) {
  bool is_game_over = false;

  for (int x = FIELD_X_START; x < FIELD_X_COUNT; x++)
    if (game_info->field[FIELD_Y_START][x] == 1) is_game_over = true;

  return is_game_over;
}

void check_for_full_rows(game_info_t *game_info) {
  int full_rows = 0;

  for (int t = 0; t < 4; t++) {
    for (int y = TOTAL_Y_COUNT - 1; y > TOTAL_Y_START; y--) {
      bool full = true;

      for (int x = FIELD_X_START; x < FIELD_X_COUNT; x++) {
        if (game_info->field[y][x] == 0) full = false;
      }

      if (full) {
        full_rows++;

        destroy_full_rows(game_info, y);
      }
    }
  }

  int score = 0;

  for (int i = 1; i <= 4; i++)
    if (full_rows >= i) score = score * 2 + 100;

  game_info->score = game_info->score + score;

  int level = game_info->score / 600 > 10 ? 10 : game_info->score / 600;

  game_info->level = level;
  game_info->speed = speed_list[level];
}

void destroy_full_rows(game_info_t *game_info, int start) {
  for (int j = start; j > 0; j--) {
    for (int i = FIELD_X_START; i < FIELD_X_COUNT; i++) {
      game_info->field[j][i] = game_info->field[j - 1][i];
    }
  }
}

void free_memory(game_info_t *game_info) {
  for (int i = TOTAL_Y_START; i < TOTAL_Y_COUNT; i++) free(game_info->field[i]);
  free(game_info->field);

  for (int i = TETRA_A_START; i < TETRA_Y_COUNT; i++) free(game_info->next[i]);
  free(game_info->next);
}