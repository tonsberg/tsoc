#include <string.h>

#include "time.h"

#include "menu.h"
#include "pcd8544.h"

char input_buffer[4] = {'\0', '\0', '\0', '\0'};
char clock[6] = {'0', '0', ':', '0', '0', '\0'};
uint8_t current_i = 0;

void menu_init(void) {
    pcd8544_clear();
    menu_title("This is the main menu");
}

static inline void menu_redraw_clock(const char * clock) {
    pcd8544_place_cursor(9, 6);
    pcd8544_write_string(clock);
}

static inline bool input_past_twelve(void) {
    return input_buffer[0] > '2' || (input_buffer[0] == '2' && input_buffer[1] > '4');
}

static inline void clear_input(void) {
    memset(input_buffer, 0x00, 4);
    sprintf(clock, "00:00");
    current_i = 0;
}

static inline void update_drawn_clock_from_input(void) {
    uint8_t i = current_i % 4;
    if (i == 0)
        sprintf(clock, "00:00");
    else if (i == 1)
        sprintf(clock, "0%c:00", input_buffer[0]);
    else if (i == 2) {
        if (input_past_twelve())
            sprintf(clock, "0%c:0%c", input_buffer[0], input_buffer[1]);
        else
            sprintf(clock, "%c%c:00", input_buffer[0], input_buffer[1]);
    } else if (i == 3) {
        if (input_past_twelve()) {
            if (input_buffer[1] > '5')
                clear_input();
            else
                sprintf(clock, "0%c:%c%c", input_buffer[0], input_buffer[1], input_buffer[2]);
        } else {
            if (input_buffer[2] > '5')
                sprintf(clock, "%c%c:0%c", input_buffer[0], input_buffer[1], input_buffer[2]);
            else
                sprintf(clock, "%c%c:%c0", input_buffer[0], input_buffer[1], input_buffer[2]);
        }
    } else {
        if (input_past_twelve() || input_buffer[2] > '5')
            clear_input();
        else
            sprintf(clock, "%c%c:%c%c", input_buffer[0], input_buffer[1], input_buffer[2], input_buffer[3]);
    }
}

static inline void persist_clock_as_alarm(void) {
    time_t alarm = (clock[4] - '0') * SECS_PER_MIN;
    alarm += (clock[3] - '0') * SECS_PER_MIN * 10;
    alarm += (clock[1] - '0') * SECS_PER_HOUR;
    alarm += (clock[0] - '0') * SECS_PER_HOUR * 10;

    time_t elapsed_now = elapsed_seconds_today(current_timestamp);
    if (elapsed_now > alarm)
        alarm += SECS_PER_DAY;
    alarm_set_default_timestamp(current_timestamp - elapsed_now + alarm);
}

void menu_action(char button) {
    last_menu_action_time = current_timestamp;
    if ('0' <= button && button <= '9') {
        input_buffer[current_i++ % 4] = button;
        update_drawn_clock_from_input();
        pcd8544_draw_big_clock(clock);
    } else {
        // TODO make an actual menu?
        char data[27];
        snprintf(data, 27, "The button '%c' was pressed", button);
        menu_content(data);
    }
}

void menu_title(const char * title) {
    last_menu_action_time = current_timestamp;
    pcd8544_place_cursor(0, 0);
    pcd8544_write_string(title);
}

void menu_content(const char * content) {
    last_menu_action_time = current_timestamp;
    pcd8544_place_cursor(3, 3);
    pcd8544_write_string(content);
}

void menu_check_state(void) {
    if (!current_i)
        sprintf(clock, "%02lu:%02lu", get_hour(current_timestamp), get_minute(current_timestamp));

    if (last_menu_action_time > (current_timestamp - 15)) {
        pcd8544_set_backlight_state(true);
        if (current_i) {
            pcd8544_draw_big_clock(clock);
            if (last_menu_action_time < (current_timestamp - 10))
                persist_clock_as_alarm();
        } else
            menu_redraw_clock(clock);
    } else {
        pcd8544_draw_big_clock(clock);
        if (backlight_blinking)
            pcd8544_set_backlight_state(current_timestamp%2);
        else
            pcd8544_set_backlight_state(false);
    }

}
