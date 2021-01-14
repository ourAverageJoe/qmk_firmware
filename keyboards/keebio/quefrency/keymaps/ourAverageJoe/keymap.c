#include QMK_KEYBOARD_H

extern keymap_config_t keymap_config;

#define WARMUP_LEVEL_MIN 106
#define WARMUP_LEVEL_MAX 255
#define SUSTAIN_DEFAULT 500

static bool enableWarmup = true;
static uint8_t warmupLevel = WARMUP_LEVEL_MIN;
static uint16_t warmupSustain = SUSTAIN_DEFAULT;
static uint16_t cooldownTimer = 0;
void warmupEffectHandler(void);

const uint8_t RGBLED_SNAKE_INTERVALS[] PROGMEM = { 100, 50, 20 };
const uint8_t RGBLED_BREATHING_INTERVALS[] PROGMEM = { 10, 5, 2, 1};

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _BASE 0
#define _FN1 1

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  RGB_MODE_WARMUP
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT_65_with_macro(
    KC_MPLY,   KC_ESC,   KC_GRV, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_DEL,  KC_BSPC, KC_HOME, \
    KC_MPLY,   KC_MNXT,   KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_PGUP, \
    KC_BRMD,   KC_BRMU,   KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,  KC_PGDN, \
    RGB_MODE_PLAIN,   RGB_MODE_BREATHE,   KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END, \
    RGB_MODE_WARMUP,  RGB_MODE_SNAKE,  KC_LCTL, KC_LALT, MO(_FN1), KC_LGUI, KC_SPC,           KC_SPC,  KC_SPC,  KC_RALT, MO(_FN1), KC_RCTL,KC_LEFT, KC_DOWN, KC_RGHT
  ),

  [_FN1] = LAYOUT_65_with_macro(
    _______, _______, _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,  KC_DEL, RGB_TOG, \
    _______, KC_MPRV, RGB_TOG, RGB_MOD, _______, KC_UP,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______, _______, _______
  )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (enableWarmup && record->event.pressed) {
    cooldownTimer = timer_read();
    warmupSustain = SUSTAIN_DEFAULT;
    if (warmupLevel >= WARMUP_LEVEL_MAX) {
      warmupLevel = WARMUP_LEVEL_MAX;
    } else {
      warmupLevel++;
    }
  }

  switch (keycode) {
    case RGB_MODE_WARMUP:
      if (record->event.pressed) {
        enableWarmup = !enableWarmup;
        warmupLevel = WARMUP_LEVEL_MIN;
      }
      return false;
    default:
      return true;
  }
}

void matrix_scan_user() {
  if (enableWarmup) {
    warmupEffectHandler();
  }
}

void warmupEffectHandler() {
  if (timer_elapsed(cooldownTimer) > warmupSustain) {
    if (warmupSustain > 25) {
      warmupSustain -= 25;
    }

    if (warmupLevel > WARMUP_LEVEL_MIN) {
      warmupLevel--;
    }

    cooldownTimer = timer_read();
  }

  rgblight_sethsv_noeeprom(warmupLevel, 255, 255);

  return;
}

void encoder_update_user(uint8_t index, bool clockwise) {
  if (index == 0) {
    // Left
    switch(biton32(layer_state)) {
      case _BASE:
        if (clockwise) {
          tap_code(KC_VOLU);
        } else {
          tap_code(KC_VOLD);
        }
        break;
      case _FN1:
        if (clockwise) {
          rgblight_increase_val();
        } else {
          rgblight_decrease_val();
        }
    }
  } else if (index == 1) {
    // Right
    switch(biton32(layer_state)) {
      case _BASE:
        if (clockwise) {
          rgblight_increase_hue();
        } else {
          rgblight_decrease_hue();
        }
        break;
      case _FN1:
        if (clockwise) {
          rgblight_increase_sat();
        } else {
          rgblight_decrease_sat();
        }
        break;
    }
  }
}
