/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#include "features/oneshot.h"
#include "features/swapper.h"
#include "layout.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    #define LAYER_X(LAYER) \
    [U_##LAYER] = U_MACRO_VA_ARGS(LAYOUT, LAYER_##LAYER),
    LAYER_LIST
    #undef LAYER_X
};
// clang-format on

#pragma region PERSISTENT_STATE
typedef union {
    uint32_t raw;
    struct {
        uint8_t root_layer;
        uint8_t a_algr_active : 1; // 1 bit
        uint8_t a_shft_active : 1; // 1 bit
        uint8_t _reserved : 6;

        uint16_t _pad; // keep struct at 4 bytes
    };
} user_config_t;

_Static_assert(sizeof(user_config_t) == 4, "user_config_t must be 4 bytes");

static user_config_t user_config;

static void save_user_config(void) {
    eeconfig_update_user(user_config.raw); // write to EEPROM
}

void eeconfig_init_user(void) {
    user_config.raw             = 0;
    user_config.root_layer      = U_BASE;
    user_config.a_algr_active   = false;
    user_config.a_shft_active   = false;

    eeconfig_update_user(user_config.raw);
}

#pragma endregion

// clang-format off
#pragma region ONESHOT
bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
        #define OSLAYER_X(OSKEY, OSLAYER) \
        case OSL_##OSKEY:
        OSLAYER_LIST
        #undef OSLAYER_X
        case OS_CNCL:
            return true;
        default:
            return false;
    }
}

bool is_oneshot_ignored_key(uint16_t keycode) {
    switch (keycode) {
        #define OSLAYER_X(OSKEY, OSLAYER) \
        case OSL_##OSKEY:
        OSLAYER_LIST
        #undef OSLAYER_X

        #define OSMOD_X(OSMOD) \
        case OS_##OSMOD:
        OSMOD_LIST
        #undef OSMOD_X
            return true;
        default:
            return false;
    }
}

bool is_oneshot_layer_cancel_key(uint16_t keycode) {
    switch (keycode) {
        #define OSLAYER_X(OSKEY, OSLAYER) \
        case OSL_##OSKEY:
        OSLAYER_LIST
        #undef OSLAYER_X
        case OS_CNCL:
            // we cancel ONLY one shot behaviour(queued buttons, hold stays), so
            // pressing and holding osl button once always means we are on pressed layer
            return true;
        default:
            return false;
    }
}

bool is_oneshot_layer_ignored_key(uint16_t keycode) {
    // non ignoring osl keys prevents trilayer activation without holding one of
    // layer keys
    return false;
}

#define OSMOD_X(OSMOD) \
oneshot_key_t os_##OSMOD##_key = {os_up_unqueued, 0};
OSMOD_LIST
#undef OSMOD_X

#define OSLAYER_X(OSKEY, OSLAYER) \
oneshot_key_t os_##OSKEY##_key = {os_up_unqueued, 0};
OSLAYER_LIST
#undef OSLAYER_X

#pragma endregion
// clang-format on

// clang-format off
#pragma region SWAPPERS

#define SW_X(SWNAME, SWMOD, SWKEY) \
bool sw_##SWNAME##_active = false;
SW_LIST
#undef SW_X

// hardcoded for niri. mapped key should be default processable
uint16_t swapper_mapping(uint16_t keycode) {
    switch (keycode) {
        // exit swapper. same place as q
        case U_STAB:
            return KC_ESC;

        // swap modes. same order as in niri
        case KC_TAB:
        case U_UNDO:
            return KC_S;
        case U_CUT:
            return KC_A;
        case U_COPY:
            return KC_W;
        case U_PSTE:
            return KC_O;
    }
    return KC_NO;
}
bool swapper_intercept(uint16_t keycode, keyrecord_t *record) {
    if (
        #define SW_X(SWNAME, SWMOD, SWKEY) \
        sw_##SWNAME##_active ||
        SW_LIST
        #undef SW_X
        false) {
        uint16_t mapped_key = swapper_mapping(keycode);
        if (mapped_key != KC_NO) {
            if (record->event.pressed)
                register_code(mapped_key);
            else
                unregister_code(mapped_key);
            return true;
        }
    }
    return false;
}
bool swapper_ignored(uint16_t keycode) {
    switch (keycode) {
        case OS_LSFT:
        case KC_LEFT:
        case KC_RIGHT:
        case KC_UP:
        case KC_DOWN:
        case KC_HOME:
        case KC_END:
        case KC_ENT:
        case KC_ESC:
            return true;
    }
    return false;
}
#pragma endregion
// clang-format on

#pragma region AUTO_SHIFT
bool is_auto_shift_key(uint16_t keycode) {
    if (get_mods()) return false;
    switch (keycode) {
        case KC_F1 ... KC_F12:
            return true;
    }
    return false;
}

bool is_auto_algr_key(uint16_t keycode) {
    if (get_mods() & (~MOD_BIT(KC_LSFT))) return false;
    switch (keycode) {
        case KC_A ... KC_Z:
        case KC_1 ... KC_0:
        case KC_MINUS ... KC_SLASH:
        case KC_NUBS:
        case KC_QUES:
        case KC_EXLM:
        case KC_UNDS:
            return true;
    }
    return false;
}

bool get_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
    // if (get_repeat_key_count() != 0) {
    //   return false;
    // }

    bool is_shifted = false;
    if (user_config.a_algr_active) is_shifted = is_shifted || is_auto_algr_key(keycode);

    if (user_config.a_shft_active) is_shifted = is_shifted || is_auto_shift_key(keycode);

    return is_shifted;
}

// bool get_auto_shift_repeat(uint16_t keycode, keyrecord_t *record) {
//   return os_LSFT_key.oneshot_state == os_up_unqueued;
// }

bool     shift_pre_active = false;
uint16_t prev_pressed     = KC_NO;
uint16_t prev_released    = KC_NO;

void process_auto_shift_state(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        if (keycode == A_SHFT) {
            user_config.a_shft_active = !user_config.a_shft_active;
            save_user_config();
            return;
        } else if (keycode == A_ALGR) {
            user_config.a_algr_active = !user_config.a_algr_active;
            save_user_config();
            return;
        }
    }

    // bool is_shifted = get_auto_shifted_key(keycode, record);
    if (record->event.pressed && user_config.a_algr_active && is_auto_algr_key(keycode)) {
        // we check prev value for cases where we didn't release our key, but shift
        // state is gone, but was active when we started
        shift_pre_active = (shift_pre_active && prev_pressed != prev_released) || (os_LSFT_key.oneshot_state != os_up_unqueued);
        // dprintf("process_auto_shift: shifted key keycode %d, shift state %d \n",
        // keycode, shift_pre_active); dprintf("     lsft state %d \n",
        // os_LSFT_key.oneshot_state);
        prev_pressed = keycode;
        tap_code16(keycode);
    }
    // else if (!is_shifted)
    // {
    //   dprintf("process_auto_shift_toggle: non shifted key keycode %d, shift
    //   state %d \n", keycode, shift_pre_active); dprintf("     lsft state %d
    //   \n", os_LSFT_key.oneshot_state); dprintf("     is pressed %d \n",
    //   record->event.pressed);
    //   // shift_pre_active = os_LSFT_key.oneshot_state == os_down_used;
    // }
}

// note that modifiers are disabled so if something outside of shift(modifiers)
// is held - we don't call this
void autoshift_press_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    if (is_auto_shift_key(keycode)) {
        if (shifted) add_weak_mods(MOD_BIT(KC_LSFT));
        register_code16(keycode);
        return;
    }

    // unregister algr and lsft modes
    del_weak_mods(MOD_BIT(KC_LSFT));
    del_weak_mods(MOD_BIT(KC_ALGR));

    // get real auto hold key state, cause regular "shifted" will be set even on
    // shift down(or oneshot) so hold and shift hold are mixed - cause that
    // feature is for shift origanally
    uint16_t timeout   = get_autoshift_timeout(keycode, record);
    bool     timed_out = timer_elapsed(record->event.time) >= timeout;

    // if shift key is pressed before auto shift - then we are shifted
    if (shift_pre_active) add_weak_mods(MOD_BIT(KC_LSFT));

    // consider processing hold only if we process the same key
    shifted = shifted && prev_pressed == keycode;
    if (timed_out && shifted) {
        tap_code16(KC_BSPC);
        add_weak_mods(MOD_BIT(KC_RALT));
        register_code16(keycode);
    }

    // dprintf("autoshift_press_user: keycode %d, shift state %d \n", keycode,
    // shift_pre_active);
    shift_pre_active = os_LSFT_key.oneshot_state == os_down_used;

    prev_released = keycode;
}

void autoshift_release_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    // dprintf("autoshift_release_user: keycode %d, shift state %d \n", keycode,
    // shift_pre_active);
    unregister_code16(keycode);
    del_weak_mods(MOD_BIT(KC_LSFT));
    del_weak_mods(MOD_BIT(KC_ALGR));
}

#pragma endregion

#pragma region TAP_DANCE
void safe_qk_boot(tap_dance_state_t *state, void *user_data) {
    if (state->count < 2) return;

    reset_keyboard();
}

void safe_ee_clr(tap_dance_state_t *state, void *user_data) {
    if (state->count < 2) return;

    eeconfig_init();
}

void tool_tap(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        tap_code16(KC_F13);
    } else if (state->count == 2) {
        tap_code16(KC_SLEP);
    }
}
tap_dance_action_t tap_dance_actions[] = {
    [TD_BOOT]  = ACTION_TAP_DANCE_FN_ADVANCED(safe_qk_boot, NULL, NULL),
    [TD_EECLR] = ACTION_TAP_DANCE_FN_ADVANCED(safe_ee_clr, NULL, NULL),
    [TD_TOOL]  = ACTION_TAP_DANCE_FN_ADVANCED(tool_tap, NULL, NULL),
};
#pragma endregion

layer_state_t prev_layer_state = 0;
layer_state_t layer_state_set_user(layer_state_t state) {
    bool nav_was_on = layer_state_cmp(prev_layer_state, U_NAV);
    bool nav_is_on  = layer_state_cmp(state, U_NAV);
    // clang-format off
    if (nav_was_on && !nav_is_on) {
        #define SW_X(SWNAME, SWMOD, SWKEY) \
        unregister_swapper(&sw_##SWNAME##_active, SWMOD, SWKEY, SW_##SWNAME);
        SW_LIST
        #undef SW_X
    }
    // clang-format on

    prev_layer_state = state;
    return update_tri_layer_state(state, U_NAV, U_NUM, U_FUN);
}

/*
bool remember_last_key_user(uint16_t keycode, keyrecord_t* record,
                            uint8_t* remembered_mods) {
    switch (keycode) {
        #define OSLAYER_X(OSKEY, OSLAYER) \
        case OSL_##OSKEY:
        OSLAYER_LIST
        #undef OSLAYER_X

        #define OSMOD_X(OSMOD) \
        case OS_##OSMOD:
        OSMOD_LIST
        #undef OSMOD_X
            return false;
    }
    return true;
}
*/
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // intercept keys during active swapper
    bool intercepted = swapper_intercept(keycode, record);
    if (intercepted) return false;

    // important that it is before any layer change in case OS_CNCL is on one of
    // the layers
    bool is_cncl             = keycode == OS_CNCL;
    bool continue_processing = !is_cncl;

    bool layer_updated = false;

    // clang-format off
    // update oneshot layers. if layer is changed - get new underlying keycode.
    // note | updates all swappers - no short circuit
    if (
        #define OSLAYER_X(OSKEY, OSLAYER) \
        update_oneshot_layer(&os_##OSKEY##_key, U_##OSLAYER, OSL_##OSKEY, keycode, record->event.pressed) |
        OSLAYER_LIST
        #undef OSLAYER_X
            layer_updated) {
        keycode = get_record_keycode(record, true);
    }

    // update oneshot mods.
    #define OSMOD_X(OSMOD) \
    update_oneshot(&os_##OSMOD##_key, KC_##OSMOD, OS_##OSMOD, keycode, record->event.pressed);
    OSMOD_LIST
    #undef OSMOD_X

    if (continue_processing) process_auto_shift_state(keycode, record);

    // swapper land
    if (!swapper_ignored(keycode)) {
        // if we are tapping swapper trigger
        if (
            #define SW_X(SWNAME, SWMOD, SWKEY) \
            (keycode == SW_##SWNAME) ||
            SW_LIST
            #undef SW_X
            false) {
                // if swapper is active AND it's not triggered by our trigger - unregister it
                #define SW_X(SWNAME, SWMOD, SWKEY) \
                if (sw_##SWNAME##_active && keycode != SW_##SWNAME) \
                    unregister_swapper(&sw_##SWNAME##_active, SWMOD, SWKEY, SW_##SWNAME);
                SW_LIST
                #undef SW_X
        }

        #define SW_X(SWNAME, SWMOD, SWKEY) \
        update_swapper(&sw_##SWNAME##_active, SWMOD, SWKEY, SW_##SWNAME, keycode, record);
        SW_LIST
        #undef SW_X
    }
    // clang-format on

    return continue_processing;
};

void matrix_scan_user(void) {
    // clang-format off
    #define OSMOD_X(OSMOD) \
    scan_oneshot(&os_##OSMOD##_key, KC_##OSMOD, OS_##OSMOD);
    OSMOD_LIST
    #undef OSMOD_X

    #define OSLAYER_X(OSKEY, OSLAYER) \
    scan_oneshot_layer(&os_##OSKEY##_key, U_##OSLAYER, OSL_##OSKEY);
    OSLAYER_LIST
    #undef OSLAYER_X
    // clang-format on
}

void keyboard_post_init_user(void) {
    user_config.raw = eeconfig_read_user();
    set_single_default_layer(user_config.root_layer);

    // debug_enable=true;
    // debug_matrix=true;
    // adebug_keyboard=true;
    // debug_mouse=true;
}
