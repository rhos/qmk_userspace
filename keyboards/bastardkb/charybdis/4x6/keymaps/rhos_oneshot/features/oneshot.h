#pragma once

#include QMK_KEYBOARD_H

typedef bool (*oneshot_key_callback_t)(uint16_t);

typedef enum {
    MODDISH_NONE,
    MODDISH_RELEASED,
    MODDISH_PRESSED,
} moddish_t;

typedef enum {
    os_up_unqueued,
    os_up_queued,
    os_up_queued_used,
    os_down_unused,
    os_down_used,
} oneshot_state_t;

typedef struct {
    oneshot_state_t oneshot_state;
    uint16_t queued_time;
    uint16_t down_unused_time;
} oneshot_key_t;

moddish_t update_oneshot_impl(oneshot_key_t *oskey, uint16_t trigger, uint16_t keycode, bool pressed, oneshot_key_callback_t cancel_callback, oneshot_key_callback_t ignored_callback);
moddish_t scan_oneshot_impl(oneshot_key_t *oskey, uint16_t trigger);

void update_oneshot(
    oneshot_key_t* oskey,
    uint16_t mod,
    uint16_t trigger,
    uint16_t keycode,
    bool pressed
);
void scan_oneshot(
    oneshot_key_t* oskey,
    uint16_t mod,
    uint16_t trigger);
bool is_oneshot_cancel_key(uint16_t keycode);
bool is_oneshot_ignored_key(uint16_t keycode);
void force_cancel(oneshot_key_t* oskey, uint16_t mod);


bool update_oneshot_layer(
    oneshot_key_t* oskey,
    uint8_t layer,
    uint16_t trigger,
    uint16_t keycode,
    bool pressed
);
void scan_oneshot_layer(
    oneshot_key_t* oskey,
    uint8_t layer,
    uint16_t trigger);
bool is_oneshot_layer_cancel_key(uint16_t keycode);
bool is_oneshot_layer_ignored_key(uint16_t keycode);
void force_cancel_layer(oneshot_key_t* oskey, uint8_t layer);
