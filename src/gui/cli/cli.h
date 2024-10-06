#ifndef CLI_H
#define CLI_H

#include <ncurses.h>

#include "tetris.h"

#define BOX (char)0x2588
#define OFF ' '

#define UP_BUTTON 3
#define DOWN_BUTTON 2
#define LEFT_BUTTON 4
#define RIGHT_BUTTON 5
#define ACTION_BUTTON ' '
#define PAUSE_BUTTON 'p'
#define TERMINATE_BUTTON '\e'
#define START_BUTTON '\n'

void update_field(game_info_t game_info_t);
void update_info(game_info_t game_info_t);
void print_rectangle(int top_y, int bottom_y, int left_x, int right_x);
user_action_t input_to_action(int input);

#endif