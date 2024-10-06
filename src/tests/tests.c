#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "tetris.h"

START_TEST(preload_high_score_0) {
  game_info_t game_info;
  game_info.high_score = 100;
  save_high_score(&game_info);
  FILE *file = fopen("high_score.txt", "r");
  ck_assert_msg(file != NULL, "Failed: preload_high_score_0. Code %p", file);
  fclose(file);
}

START_TEST(game_start_0) {
  update_current_state();
  user_input(Start, 1);
  update_current_state();
  game_info_t game_info = update_current_state();
  ck_assert_msg(game_info.pause == 0, "Failed: game_start_0. Code %d",
                game_info.pause);
}

START_TEST(moving_0) {
  user_input(Left, 1);
  update_current_state();
  user_input(Right, 1);
  for (int i = 0; i < 100; i++) update_current_state();
  user_input(Action, 1);
  update_current_state();
  user_input(Down, 1);
  update_current_state();
  game_info_t game_info = update_current_state();
  ck_assert_msg(game_info.pause == 0, "Failed: moving_0. Code %d",
                game_info.pause);
}

START_TEST(pause_0) {
  user_input(Pause, 1);
  game_info_t game_info = update_current_state();
  ck_assert_msg(game_info.pause == 20, "Failed: game_start_0. Code %d",
                game_info.pause);
  user_input(Pause, 1);
  for (int i = 0; i < 25; i++) game_info = update_current_state();
  ck_assert_msg(game_info.pause == 0, "Failed: game_start_0. Code %d",
                game_info.pause);
}

START_TEST(end_0) {
  update_current_state();
  for (int i = 0; i < 100; i++) {
    user_input(Down, 1);
    update_current_state();
  }
  game_info_t game_info = update_current_state();
  ck_assert_msg(game_info.pause == -2, "Failed: end_0. Code %d",
                game_info.pause);
  user_input(Start, 1);
  game_info = update_current_state();
  user_input(Start, 1);
  game_info = update_current_state();
  ck_assert_msg(game_info.pause == 0, "Failed: end_0. Code %d",
                game_info.pause);
}

START_TEST(game_end_0) {
  user_input(Terminate, 1);
  game_info_t game_info = update_current_state();
  ck_assert_msg(game_info.pause == EXIT, "Failed: game_end_0. Code %d",
                game_info.pause);
}

Suite *suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("suite");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, preload_high_score_0);
  tcase_add_test(tc_core, game_start_0);
  tcase_add_test(tc_core, moving_0);
  tcase_add_test(tc_core, pause_0);
  tcase_add_test(tc_core, end_0);
  tcase_add_test(tc_core, game_end_0);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  SRunner *sr = srunner_create(NULL);
  srunner_add_suite(sr, suite());

  srunner_set_fork_status(sr, CK_NOFORK);
  srunner_set_log(sr, "-");
  // CK_SILENT || CK_MINIMAL || CK_NORMAL || CK_VERBOSE
  srunner_run_all(sr, CK_VERBOSE);

  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}