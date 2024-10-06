#include "cli.h"

#include <stdio.h>
#include <unistd.h>

int main(void) {
  initscr();
  keypad(stdscr, TRUE);
  noecho();
  cbreak();
  nodelay(stdscr, TRUE);
  curs_set(0);
  set_escdelay(0);
  timeout(0);

  // setlocale(LC_ALL, "en-US");

  game_info_t game_info;

  clear();
  refresh();

  game_info = update_current_state();

  while (game_info.pause != EXIT) {
    flushinp();

    napms(17);

    if (game_info.pause == PAUSE) {
      update_info(game_info);

      int timer = PAUSE_FRAMES;

      while (getch() != 'p' || timer != 0) {
        flushinp();

        if (timer > 0) timer--;

        napms(17);
      }
      game_info.pause = PAUSE_FRAMES - 1;
    }

    char input = getch();

    if (input != -1) user_input(input_to_action(input), TRUE);

    update_field(game_info);
    update_info(game_info);

    game_info = update_current_state();
  }

  nodelay(stdscr, FALSE);
  mvprintw(20, 25, " PEACE OUT! ");
  refresh();
  // napms(1250);
  endwin();

  return 0;
}

void update_field(game_info_t game_info) {
  print_rectangle(0, 21, 0, 21);
  for (int x = 0; x < 10; x++) {
    for (int y = 0; y < 20; y++) {
      if (game_info.field[y + 1][x] != 0) {
        mvaddch(y + 1, 2 * x + 1, BOX);
        mvaddch(y + 1, 2 * x + 2, BOX);
      } else {
        mvaddch(y + 1, 2 * x + 1, OFF);
        mvaddch(y + 1, 2 * x + 2, OFF);
      }
    }
  }
}

void update_info(game_info_t game_info) {
  print_rectangle(0, 3, 24, 37);

  mvprintw(1, 26, "LEVEL");
  mvprintw(2, 26, "%d", game_info.level + 1);

  print_rectangle(4, 7, 24, 37);

  mvprintw(5, 26, "SCORE");
  mvprintw(6, 26, "%d", game_info.score);

  print_rectangle(8, 11, 24, 37);

  mvprintw(9, 26, "HIGH SCORE");
  mvprintw(10, 26, "%d", game_info.high_score);

  print_rectangle(12, 18, 24, 37);

  mvprintw(13, 29, "NEXT");

  for (int y = 0; y < 2; y++) {
    for (int x = 0; x < 4; x++) {
      if (game_info.next[y][x] != 0) {
        mvaddch(15 + y, 27 + x * 2, BOX);
        mvaddch(15 + y, 27 + x * 2 + 1, BOX);
      } else {
        mvaddch(15 + y, 27 + x * 2, ' ');
        mvaddch(15 + y, 27 + x * 2 + 1, ' ');
      }
    }
  }

  mvprintw(20, 25, "  PLAYING!  ");

  print_rectangle(19, 21, 24, 37);

  if (game_info.pause == GAME_OVER)
    mvprintw(20, 25, " GAME OVER! ");
  else if (game_info.pause == PAUSE)
    mvprintw(20, 25, "   PAUSE!   ");
  else if (game_info.pause == READY_TO_START)
    mvprintw(20, 25, "PRESS START!");
}

void print_rectangle(int top_y, int bottom_y, int left_x, int right_x) {
  mvaddch(top_y, left_x, ACS_ULCORNER);
  mvaddch(bottom_y, right_x, ACS_LRCORNER);
  mvaddch(top_y, right_x, ACS_URCORNER);
  mvaddch(bottom_y, left_x, ACS_LLCORNER);

  for (int i = left_x + 1; i < right_x; i++) {
    mvaddch(top_y, i, ACS_HLINE);
    mvaddch(bottom_y, i, ACS_HLINE);
  }

  for (int i = top_y + 1; i < bottom_y; i++) {
    mvaddch(i, left_x, ACS_VLINE);
    mvaddch(i, right_x, ACS_VLINE);
  }
}

user_action_t input_to_action(int input) {
  user_action_t action = -1;

  if (input == UP_BUTTON)
    action = Up;
  else if (input == DOWN_BUTTON)
    action = Down;
  else if (input == LEFT_BUTTON)
    action = Left;
  else if (input == RIGHT_BUTTON)
    action = Right;
  else if (input == ACTION_BUTTON)
    action = Action;
  else if (input == PAUSE_BUTTON)
    action = Pause;
  else if (input == TERMINATE_BUTTON)
    action = Terminate;
  else if (input == START_BUTTON)
    action = Start;

  return action;
}