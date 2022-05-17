#include QMK_KEYBOARD_H
#include "features/mouse_turbo_click.h"

#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
rgblight_config_t RGB_current_config;
#endif

enum layer_number {
    _QWERTY = 0,
    _ADJUST,
    _ARROWS,
    _MOUSE
};

enum custom_keycodes {
  G_TURBO = SAFE_RANGE,
  ALT_TAB
};

enum {
  TD_G_TURBO = 0
};

void turbo_g_each(qk_tap_dance_state_t *state, void *user_data) {
  register_code16(KC_G);
}

#define CLICK_PERIOD_MS 300
static deferred_token click_token = INVALID_DEFERRED_TOKEN;
static bool click_registered = false;
static uint32_t turbo_click_callback(uint32_t trigger_time, void* cb_arg) {
  if (click_registered) {
    unregister_code16(KC_G);
    click_registered = false;
  } else {
    click_registered = true;
    register_code16(KC_G);
  }
  return (CLICK_PERIOD_MS) / 2;  // Execute callback again in half a period.
}
void turbo_g_finish(qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 2) {
    uint32_t next_delay_ms = turbo_click_callback(0, NULL);
    click_token = defer_exec(next_delay_ms, turbo_click_callback, NULL);
  }
}

void turbo_g_reset(qk_tap_dance_state_t *state, void *user_data) {
  unregister_code16(KC_G);
  if (click_token != INVALID_DEFERRED_TOKEN) {
    cancel_deferred_exec(click_token);
    click_token = INVALID_DEFERRED_TOKEN;
    if (click_registered) {
      // If mouse button is currently registered, release it.
      unregister_code16(KC_MS_BTN1);
      click_registered = false;
    }
  }
}


qk_tap_dance_action_t tap_dance_actions[] = {
  [TD_G_TURBO] = ACTION_TAP_DANCE_FN_ADVANCED(turbo_g_each, turbo_g_finish, turbo_g_reset)
};

// Fillers to make layering more clear
#define TURBO_G TD(TD_G_TURBO)
#define SEMICOL LT(_ARROWS, KC_SCLN)
#define ADJUST  MO(_ADJUST)
#define MOUSE  MO(_MOUSE)
#define F_CLOSE  LALT(KC_F4)
#define TSK_MNG LALT(LCTL(KC_DELETE))
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* QWERTY
     * ,---------------------------------------------------------.                 ,-------------------------------------------------------.
     * |  F1  |   `  |   1  |   2  |   3  |   4  |   5   |   6   |                 |   6  |   7  |   8  |   9  |   0  |   -  |   =  | Bksp |
     * |------+------+------+------+------+------+-------+-------|                 |------+------+------+------+------+------+------+------|
     * |  F3  | Tab  |   Q  |   W  |   E  |   R  |   T   |   Y   |                 |   Y  |   U  |   I  |   O  |   P  |   [  |   ]  |   \  |
     * |------+------+------+------+------+------+-------+-------|                 |------+------+------+------+------+------+------+------|
     * |  F5  |Adjust|   A  |   S  |   D  |   F  |   G   |   H   |                 |   H  |   J  |   K  |   L  |   ;  |   '  | Enter| Enter|
     * |------+------+------+------+------+------+-------+-------|                 |------+------+------+------+------+------+------+------|
     * |  F7  | Shift|   Z  |   X  |   C  |   V  |   B   |   N   |                 |   N  |   M  |   ,  |   .  |   /  | Shift|  Up  | Shift|
     * |------+------+------+------+------+------+-------+-------|                 |------+------+------+------+------+------+------+------|
     * |  F9  | Ctrl | GUI  | Alt  |Adjust|Adjust| Space |  Del  |                 | Bksp | Enter|Adjust|Adjust|Adjust| Left | Down |Right |
     * `---------------------------------------------------------'                 `-------------------------------------------------------'
     */
  [_QWERTY] = LAYOUT(
      KC_F1,   KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,         KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,
      KC_F3,   KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,         KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
      KC_F5,   MOUSE,   KC_A,    KC_S,    KC_D,    KC_F,    TURBO_G, KC_H,         KC_H,    KC_J,    KC_K,    KC_L,    SEMICOL, KC_QUOT, KC_ENT,  KC_ENT,
      KC_F7,   KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,         KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_RSFT,
      KC_F9,   KC_LCTL, KC_LGUI, KC_LALT, ADJUST,  ADJUST,  KC_SPC,  KC_DEL,       KC_BSPC, KC_ENT,  ADJUST,  ADJUST,  ADJUST,  KC_LEFT, KC_DOWN, KC_RGHT
      ),

    /* Adjust
     * ,-------------------------------------------------------.                                     ,-------------------------------------------------------.
     * |      |  ESC |   F1 |   F2 |   F3 |   F4 |   F5 |   F6 |                                     |   F6 |   F7 |   F8 |   F9 |  F10 |  F11 |  F12 |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |      |      |      |                                     |      |   7  |   8  |   9  |      |      |      |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |      |      |      |                                     |      |   4  |   5  |   6  |      |      |      |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |      |      |      |                                     |      |   1  |   2  |   3  |      |      | PGUP |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |      |      |      |                                     |      |      |   0  |      |      | HOME | PGDN | END  |
     * `-------------------------------------------------------'                                     `-------------------------------------------------------'
     */
  [_ADJUST] =  LAYOUT(
      _______, KC_ESC,  KC_F1,     KC_F2,   KC_F3,   KC_F4,   KC_F5, KC_F6,                KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,    _______,
      _______, ALT_TAB, _______, _______, _______, _______, _______, _______,              _______, KC_7,    KC_8,    KC_9,    _______, _______, _______,   _______,
      _______, F_CLOSE, _______, _______, _______, _______, _______, _______,              _______, KC_4,    KC_5,    KC_6,    _______, _______, _______,   _______,
      _______, _______, _______, _______, _______, _______, _______, _______,              _______, KC_1,    KC_2,    KC_3,    _______, _______, KC_PGUP,   _______,
      _______, _______, _______, _______, _______, _______, _______, _______,              _______, _______, KC_0,    _______, _______, KC_HOME, KC_PGDOWN, KC_END
      ),

    /* Adjust
     * ,-------------------------------------------------------.                                     ,-------------------------------------------------------.
     * |      |  ESC |   F1 |   F2 |   F3 |   F4 |   F5 |   F6 |                                     |   F6 |   F7 |   F8 |   F9 |  F10 |  F11 |  F12 |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      |RGBRST|  M↑  |      |  MW↑ |      |                                     |      |   7  |   8  |   9  |      |      |      |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      |  <M  |  M↓  |  M>  |  MW↓ |      |                                     |      |   4  |   5  |   6  |      |      |      |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      |  MOD |  HUD |  SAD |  VAD |      |                                     |      |   1  |   2  |   3  |      |      | PGUP |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |  M1  |  M2  |TSKMNG|                                     |      |      |   0  |      |      | HOME | PGDN | END  |
     * `-------------------------------------------------------'                                     `-------------------------------------------------------'
     */
  [_MOUSE] =  LAYOUT(
      _______, KC_ESC,  KC_F1,   KC_F2,      KC_F3,      KC_F4,       KC_F5,         KC_F6,                KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,    _______,
      _______, _______, _______, _______,    KC_MS_UP,   _______,     KC_MS_WH_UP,   _______,              _______, KC_7,    KC_8,    KC_9,    _______, _______, _______,   _______,
      _______, _______, _______, KC_MS_LEFT, KC_MS_DOWN, KC_MS_RIGHT, KC_MS_WH_DOWN, _______,              _______, KC_4,    KC_5,    KC_6,    _______, _______, _______,   _______,
      _______, _______, _______, RGB_MOD,    RGB_HUD,    RGB_SAD,     _______,       _______,              _______, KC_1,    KC_2,    KC_3,    _______, _______, KC_PGUP,   _______,
      _______, _______, _______, _______,    _______,    KC_MS_BTN1,  KC_MS_BTN2,    TSK_MNG,              _______, _______, KC_0,    _______, _______, KC_HOME, KC_PGDOWN, KC_END
      ),

    /* _ARROWS
     * ,-------------------------------------------------------.                                     ,-------------------------------------------------------.
     * |      |  ESC |   F1 |   F2 |   F3 |   F4 |   F5 |   F6 |                                     |   F6 |   F7 |   F8 |   F9 |  F10 |  F11 |  F12 |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      |RGBRST|  UP  |      |      |      |                                     |      |      |  UP  |      |      |      |      |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      | LEFT | DOWN | RIGHT|  VAI |      |                                     |      | LEFT | DOWN | RIGHT|      |      |      |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      |  MOD |  HUD |  SAD |  VAD |      |                                     |      |      |      |      |      |      |      |      |
     * |------+------+------+------+------+------+------+------|                                     |------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |      |      |      |                                     |      |      |      |      |      |      |      |      |
     * `-------------------------------------------------------'                                     `-------------------------------------------------------'
     */
  [_ARROWS] =  LAYOUT(
      _______, KC_ESC,  KC_F1,   KC_F2,      KC_F3,      KC_F4,       KC_F5,   KC_F6,                KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______,
      _______, _______, _______, _______,    KC_MS_UP,   _______,     _______, _______,              _______, _______, KC_UP,   _______, _______, _______, _______, _______,
      _______, _______, _______, KC_MS_LEFT, KC_MS_DOWN, KC_MS_RIGHT, _______, _______,              _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______, _______, _______,
      _______, _______, _______, RGB_MOD,    RGB_HUD,    RGB_SAD,     _______, _______,              _______, _______, _______, _______, _______, _______, _______, _______,
      _______, _______, _______, _______,    _______,    _______,     _______, _______,              _______, _______, _______, _______, _______, _______, _______, _______
      )
};

static int8_t alt_tab_layer = -1;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  process_mouse_turbo_click(keycode, record, G_TURBO);
  switch (keycode) {
    /* Alt+Tab that holds Alt until current layer is released: */
    case ALT_TAB:
      if (record->event.pressed) {
        if (alt_tab_layer < 0) {
          alt_tab_layer = layer_switch_get_layer(record->event.key);
          register_code(KC_LALT);
        }
        register_code(KC_TAB);
      } else {
          unregister_code(KC_TAB);
      }
      return false;
  } 
  return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    if (alt_tab_layer >= 0 && !layer_state_cmp(state, alt_tab_layer)) {
        unregister_code(KC_LALT);
        alt_tab_layer = -1;
    }
    return state;
}