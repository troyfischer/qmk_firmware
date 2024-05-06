/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include <stdio.h>

/*
BEGIN MY CHANGES
 */
// Tap Dance keycodes
enum td_keycodes {
    ESC_1,
    CAPS_2,
    LAYER_4,
    GUI_LP // `LGUI` when held, `;` when tapped `'` when double tapped
};

// Define a type containing as many tapdance states as you need
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_SINGLE_TAP
} td_state_t;

// Create a global instance of the tapdance state type
static td_state_t td_state;

// Declare your tapdance functions:

// Function to determine the current tapdance state
td_state_t cur_dance(tap_dance_state_t *state);

// `finished` and `reset` functions for each tapdance keycode
void guilp_finished(tap_dance_state_t *state, void *user_data);
void guilp_reset(tap_dance_state_t *state, void *user_data);


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      KC_NO,  TD(LAYER_4),    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   LT(3, KC_P),  KC_NO,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_NO,  GUI_T(KC_A), ALT_T(KC_S), CTL_T(KC_D), SFT_T(KC_F), KC_G,          KC_H,  SFT_T(KC_J), CTL_T(KC_K), ALT_T(KC_L), TD(GUI_LP), KC_NO,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_NO,  KC_Z,    KC_X,    KC_C,     KC_V,    KC_B,                          KC_N,   KC_M, KC_COMM,  KC_DOT, KC_SLSH,  KC_NO,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                   MEH_T(KC_TAB), KC_SPC, LT(1, KC_ESC), TD(CAPS_2), KC_ENT, HYPR_T(KC_BSPC)
                                      //`--------------------------'  `--------------------------
                                      // NOTE: ideas for freeing up thumb keys
                                      // 1. Tab could probably be moved to a layer
                                      // 2. Caps lock could DEFINITELY be moved to a layer

  ),

  [1] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_NO, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                       KC_MINS, KC_7, KC_8, KC_9, KC_EQL, KC_NO,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
       KC_NO, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_BSLS, KC_4, KC_5, KC_6, KC_0, KC_NO,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_NO, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_GRV, KC_1, KC_2, KC_3, KC_SLSH, KC_NO,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          XXXXXXX, XXXXXXX,  KC_LSFT,  KC_RSFT, XXXXXXX, XXXXXXX
                                      //`--------------------------'  `--------------------------'
  ),

  [2] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_NO, XXXXXXX, XXXXXXX, KC_LCBR, KC_RCBR, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_NO,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
       KC_NO, XXXXXXX, XXXXXXX, KC_LPRN, KC_RPRN, XXXXXXX,                      KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, XXXXXXX, KC_NO,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
       KC_NO, XXXXXXX, XXXXXXX, KC_LBRC, KC_RBRC, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_NO,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_TAB,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
                                      //`--------------------------'  `--------------------------'
  ),

  [3] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      XXXXXXX, XXXXXXX, KC_MUTE, KC_VOLD, KC_VOLU, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          XXXXXXX, XXXXXXX,  XXXXXXX,     XXXXXXX, XXXXXXX, XXXXXXX
                                      //`--------------------------'  `--------------------------'
  ),
  /* For pure typing tasks get home row mods out of the way */
  [4] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      KC_NO,    TD(LAYER_4),    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   LT(3, KC_P),  KC_NO,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        KC_NO,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L,  CLN_QUOTE, KC_NO,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_NO,  KC_Z,    KC_X,     KC_C,     KC_V,    KC_B,                          KC_N,   KC_M, KC_COMM,  KC_DOT, KC_SLSH,  KC_NO,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                    MEH_T(KC_TAB), KC_SPC, TD(ESC_1), TD(CAPS_2), KC_ENT, HYPR_T(KC_BSPC)
                                      //`--------------------------'  `--------------------------
  ),
};

/*
This dance allows Layer 2 on hold and toggle caps lock on double tap
 */
void dance_caps_2_finished (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    layer_on (2);
  } else {
    register_code (KC_CAPS);
  }
}
void dance_caps_2_reset (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    layer_off (2);
  } else {}
}


/*
 * Toggle layer 4 on double tap of q
*/
void dance_layer_4_finished (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    tap_code (KC_Q);
  } else if (state->count == 2) {
    layer_invert(4);
  }
}

/*
 * UNUSED
 * This toggles layer 1 when pressed once and esc when pressed twice
*/
void dance_esc_1_finished (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    layer_on (1);
  } else {
    register_code (KC_ESC);
  }
}
void dance_esc_1_reset (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    layer_off (1);
  } else {
    unregister_code (KC_ESC);
  }
}

/*
This dance allows ; on single press and ' on double press
 */
void dance_cln_finished (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    register_code (KC_SCLN);
  } else {
    register_code (KC_QUOT);
  }
}

void dance_cln_reset (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    unregister_code (KC_SCLN);
  } else {
    unregister_code (KC_QUOT);
  }
}

//All tap dance functions would go here. Only showing this one.
tap_dance_action_t tap_dance_actions[] = {
 [ESC_1] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_esc_1_finished, dance_esc_1_reset),
 [CAPS_2] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_caps_2_finished, dance_caps_2_reset),
 [LAYER_4] = ACTION_TAP_DANCE_FN(dance_layer_4_finished),
 [GUI_LP] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, guilp_finished, guilp_reset)
};

// Determine the tapdance state to return
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;
    }

    if (state->count == 2) return TD_DOUBLE_SINGLE_TAP;
    else return TD_UNKNOWN; // Any number higher than the maximum state value you return above
}

// Handle the possible states for each tapdance keycode you define:
void guilp_finished(tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);
    switch (td_state) {
        case TD_SINGLE_TAP:
            register_code16(KC_SCLN);
            break;
        case TD_SINGLE_HOLD:
            register_mods(MOD_BIT(KC_LGUI)); // For a layer-tap key, use `layer_on(_MY_LAYER)` here
            break;
        case TD_DOUBLE_SINGLE_TAP: // Allow nesting of 2 parens `((` within tapping term
            register_code16(KC_QUOT);
            break;
        default:
            break;
    }
}


void guilp_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case TD_SINGLE_TAP:
            unregister_code16(KC_SCLN);
            break;
        case TD_SINGLE_HOLD:
            unregister_mods(MOD_BIT(KC_LGUI)); // For a layer-tap key, use `layer_off(_MY_LAYER)` here
            break;
        case TD_DOUBLE_SINGLE_TAP:
            unregister_code16(KC_QUOT);
            break;
        default:
            break;
    }
}

/*
END MY CHANGES
 */

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (!is_keyboard_master()) {
    return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
  }
  return rotation;
}

#define L_BASE 0
#define L_LOWER 2
#define L_RAISE 4
#define L_ADJUST 8

void oled_render_layer_state(void) {
    oled_write_P(PSTR("Layer: "), false);
    switch (layer_state) {
        case L_BASE:
            oled_write_ln_P(PSTR("Default"), false);
            break;
        case L_LOWER:
            oled_write_ln_P(PSTR("Lower"), false);
            break;
        case L_RAISE:
            oled_write_ln_P(PSTR("Raise"), false);
            break;
        case L_ADJUST:
        case L_ADJUST|L_LOWER:
        case L_ADJUST|L_RAISE:
        case L_ADJUST|L_LOWER|L_RAISE:
            oled_write_ln_P(PSTR("Adjust"), false);
            break;
    }
}


char keylog_str[24] = {};

const char code_to_name[60] = {
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\',
    '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

void set_keylog(uint16_t keycode, keyrecord_t *record) {
  char name = ' ';
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) ||
        (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX)) { keycode = keycode & 0xFF; }
  if (keycode < 60) {
    name = code_to_name[keycode];
  }

  // update keylog
  snprintf(keylog_str, sizeof(keylog_str), "%dx%d, k%2d : %c",
           record->event.key.row, record->event.key.col,
           keycode, name);
}

void oled_render_keylog(void) {
    oled_write(keylog_str, false);
}

void render_bootmagic_status(bool status) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM logo[][2][3] = {
        {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
        {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
    };
    if (status) {
        oled_write_ln_P(logo[0][0], false);
        oled_write_ln_P(logo[0][1], false);
    } else {
        oled_write_ln_P(logo[1][0], false);
        oled_write_ln_P(logo[1][1], false);
    }
}

void oled_render_logo(void) {
    static const char PROGMEM crkbd_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
        0};
    oled_write_P(crkbd_logo, false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_render_layer_state();
        oled_render_keylog();
    } else {
        oled_render_logo();
    }
    return false;
}
#endif // OLED_ENABLE
