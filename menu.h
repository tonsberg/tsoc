#ifndef MENU_H
#define MENU_H

#include <stdio.h>

#include "time.h"

time_t last_menu_action_time = 0;

void menu_init(void);
void menu_redraw_clock(struct time_data *);
void menu_action(char);
void menu_title(const char * title);
void menu_content(const char * content);
void menu_draw_big_clock(struct time_data *);
#endif
