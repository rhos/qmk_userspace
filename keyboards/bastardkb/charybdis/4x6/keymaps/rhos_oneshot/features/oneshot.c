#include "oneshot.h"
#include <stdint.h>
#include "debug.h"
#include "timer.h"

#define ONESHOT_ACTIVE_TIMEOUT 500
#define ONESHOT_UNUSED_TIMEOUT 300

moddish_t update_oneshot_impl(oneshot_key_t *oskey, uint16_t trigger, uint16_t keycode, bool pressed, oneshot_key_callback_t cancel_callback, oneshot_key_callback_t ignored_callback) {
    moddish_t moddishChanged = MODDISH_NONE;
    if (keycode == trigger) {
        if (pressed) {
            if (oskey->oneshot_state == os_up_unqueued) {
                moddishChanged = MODDISH_PRESSED;
            }
            oskey->oneshot_state    = os_down_unused;
            oskey->down_unused_time = timer_read();
        } else {
            switch (oskey->oneshot_state) {
                case os_down_unused:
                    oskey->oneshot_state = os_up_queued;
                    oskey->queued_time   = timer_read();
                    break;
                case os_down_used:
                    oskey->oneshot_state = os_up_unqueued;
                    moddishChanged       = MODDISH_RELEASED;
                    break;
                default:
                    break;
            }
        }
    } else {
        if (pressed) {
            if (cancel_callback(keycode) && (oskey->oneshot_state == os_up_queued || oskey->oneshot_state == os_up_queued_used)) {
                oskey->oneshot_state = os_up_unqueued;
                moddishChanged       = MODDISH_RELEASED;
            }
            if (!ignored_callback(keycode)) {
                switch (oskey->oneshot_state) {
                    case os_down_unused:
                        oskey->oneshot_state = os_down_used;
                        break;
                    case os_up_queued:
                        oskey->oneshot_state = os_up_queued_used;
                        break;
                    case os_up_queued_used:
                        oskey->oneshot_state = os_up_unqueued;
                        moddishChanged       = MODDISH_RELEASED;
                        break;
                    default:
                        break;
                }
            }
        } else {
            if (!ignored_callback(keycode)) {
                switch (oskey->oneshot_state) {
                    // case os_down_unused:
                    //     oskey->oneshot_state = os_down_used;
                    //     break;
                    // case os_up_queued:
                    //     oskey->oneshot_state = os_up_unqueued;
                    //     moddishChanged       = MODDISH_RELEASED;
                    //     break;
                    case os_up_queued_used:
                        oskey->oneshot_state = os_up_unqueued;
                        moddishChanged       = MODDISH_RELEASED;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return moddishChanged;
}

moddish_t scan_oneshot_impl(oneshot_key_t *oskey, uint16_t trigger) {
    moddish_t moddishChanged = MODDISH_NONE;

    switch (oskey->oneshot_state) {
        case os_up_queued: {
            uint16_t elapsed = timer_elapsed(oskey->queued_time);
            if (elapsed >= ONESHOT_ACTIVE_TIMEOUT) {
                oskey->oneshot_state = os_up_unqueued;
                oskey->queued_time   = 0;
                moddishChanged       = MODDISH_RELEASED;
            }
            break;
        }
        case os_down_unused: {
            uint16_t elapsed = timer_elapsed(oskey->down_unused_time);
            if (elapsed >= ONESHOT_UNUSED_TIMEOUT) {
                oskey->oneshot_state    = os_down_used;
                oskey->down_unused_time = 0;
            }
            break;
        }
        default:
            break;
    }

    return moddishChanged;
}

void _handle_oneshot(moddish_t result, uint16_t mod) {
    switch (result) {
        case MODDISH_PRESSED:
            register_code(mod);
            break;
        case MODDISH_RELEASED:
            unregister_code(mod);
            break;
        default:
            break;
    }
}

void update_oneshot(oneshot_key_t *oskey, uint16_t mod, uint16_t trigger, uint16_t keycode, bool pressed) {
    moddish_t result = update_oneshot_impl(oskey, trigger, keycode, pressed, is_oneshot_cancel_key, is_oneshot_ignored_key);
    _handle_oneshot(result, mod);
}
void scan_oneshot(oneshot_key_t *oskey, uint16_t mod, uint16_t trigger) {
    moddish_t result = scan_oneshot_impl(oskey, trigger);
    _handle_oneshot(result, mod);
}
void force_cancel(oneshot_key_t *oskey, uint16_t mod)
{
    if(oskey->oneshot_state != os_up_unqueued) {
        oskey->oneshot_state = os_up_unqueued;
        _handle_oneshot(MODDISH_RELEASED, mod);
    }
}

void _handle_oneshot_layer(moddish_t result, uint8_t layer) {
    switch (result) {
        case MODDISH_PRESSED:
            layer_on(layer);
            break;
        case MODDISH_RELEASED:
            layer_off(layer);
            break;
        default:
            break;
    }
}

bool update_oneshot_layer(oneshot_key_t *oskey, uint8_t layer, uint16_t trigger, uint16_t keycode, bool pressed) {
    moddish_t result = update_oneshot_impl(oskey, trigger, keycode, pressed, is_oneshot_layer_cancel_key, is_oneshot_layer_ignored_key);
    _handle_oneshot_layer(result, layer);
    return result != MODDISH_NONE;
}

void scan_oneshot_layer(oneshot_key_t *oskey, uint8_t layer, uint16_t trigger) {
    moddish_t result = scan_oneshot_impl(oskey, trigger);
    _handle_oneshot_layer(result, layer);
}
void force_cancel_layer(oneshot_key_t *oskey, uint8_t layer)
{
    if(oskey->oneshot_state != os_up_unqueued) {
        oskey->oneshot_state = os_up_unqueued;
        _handle_oneshot_layer(MODDISH_RELEASED, layer);
    }
}
