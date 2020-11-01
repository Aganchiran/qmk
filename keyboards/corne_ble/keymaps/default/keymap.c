#include QMK_KEYBOARD_H
#include "app_ble_func.h"

#ifdef SSD1306OLED
#include "ssd1306.h"
#endif

#include "eeprom.h"

#ifdef RGBLIGHT_ENABLE
extern rgblight_config_t rgblight_config; //This line allows macro to read current RGB settings
#endif
extern keymap_config_t keymap_config;



//////////////////////
/////CUSTOM ENUMS/////
//////////////////////

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.

enum layer_number {
    _QWERTY = 0,
    _LOWER,
    _RAISE,
    _FL,
    _ADJUST
};

enum custom_keycodes {
    ENT_SLP = SAFE_RANGE, /* Deep sleep mode*/
    QWERTY,
    LOWER,
    RAISE,
    POWER_TAB,
    TOGGLE_MODS,
    ADJUST
};

enum tapdance_keys {
    FL_WIN
};

enum tapdance_states {
    SINGLE_TAP = 1,
    SINGLE_HOLD = 2,
    DOUBLE_TAP = 3
};

typedef struct {
    bool is_press_action;
    uint8_t state;
} tap;



////////////////
/////KEYMAP/////
////////////////

// Fillers to make layering more clear
#define _______ KC_TRNS
#define XXXXXXX KC_NO

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_QWERTY] = LAYOUT(\
		KC_ESC,		KC_Q,		KC_W,		KC_E,		KC_R,		KC_T,						KC_Y,		KC_U,		KC_I,		KC_O,		KC_P,		KC_TAB, \
		KC_LSFT,	KC_A,		KC_S,		KC_D,		KC_F,		KC_G,						KC_H,		KC_J,		KC_K,		KC_L,		KC_QUOT,	RSFT_T(KC_CAPS), \
		KC_LCTL,	KC_Z,		KC_X,		KC_C,		KC_V,		KC_B,						KC_N,		KC_M,		KC_COMM,	KC_DOT,		KC_SCLN,	KC_LALT, \
											TD(FL_WIN),	KC_SPC,		LT(_RAISE, KC_ENT),			KC_RALT,	KC_BSPC,	MO(_LOWER)\
		),

[_LOWER] = LAYOUT(\
		_______,	KC_DQUO,	KC_LWIN,	KC_EXLM,	KC_QUES,	XXXXXXX,					KC_AMPR,	KC_PIPE,	KC_PSCR,	XXXXXXX,	KC_PERC,	_______, \
		_______,	KC_AT,		KC_DLR,		KC_LPRN,	KC_RPRN,	XXXXXXX,					KC_HASH,	XXXXXXX,	XXXXXXX,	XXXXXXX,	KC_GRV,		_______, \
		_______,	XXXXXXX,	KC_CIRC,	KC_LBRC,	KC_RBRC,	XXXXXXX,					XXXXXXX,	KC_MINS,	XXXXXXX,	XXXXXXX,	XXXXXXX,	TOGGLE_MODS, \
											_______,	_______,	_______,					_______,	_______,	_______\
		),

[_RAISE] = LAYOUT(\
		_______,	KC_PSLS,	KC_7,		KC_8,		KC_9,		KC_PAST,					KC_BSLS,	XXXXXXX,	KC_P7,		KC_P8,		KC_P9,		_______, \
		_______,	KC_PMNS,	KC_4,		KC_5,		KC_6,		KC_PPLS,					XXXXXXX,	XXXXXXX,	KC_P4,		KC_P5,		KC_P6,		_______, \
		_______,	KC_EQL,		KC_1,		KC_2,		KC_3,		KC_0,						XXXXXXX,	KC_P0,		KC_P1,		KC_P2,		KC_P3,		_______, \
											_______,	_______,	_______,					_______,	KC_DEL,		_______\
		),

[_FL] = LAYOUT(\
		POWER_TAB,	XXXXXXX,	KC_HOME,	KC_UP,		KC_END,		XXXXXXX,					KC_F1,		KC_F2,		KC_F3,		KC_F4,		KC_F5,		KC_F6, \
		_______,	XXXXXXX,	KC_LEFT,	KC_DOWN,	KC_RGHT,	XXXXXXX,					KC_F7,		KC_F8,		KC_F9,		KC_F10,		KC_F11,		KC_F12, \
		_______,	XXXXXXX,	XXXXXXX,	XXXXXXX,	XXXXXXX,	XXXXXXX,					XXXXXXX,	XXXXXXX,	XXXXXXX,	XXXXXXX,	XXXXXXX,	_______, \
											_______,	_______,	_______,					_______,	_______,	_______\
		),

[_ADJUST] = LAYOUT(\
		RGB_TOG,	XXXXXXX,	XXXXXXX,	XXXXXXX,	XXXXXXX,	XXXXXXX,					RGB_M_P,	RGB_M_B,	RGB_M_R,	RGB_M_SW,	RGB_M_SN,	_______, \
		_______,	RGB_MOD,	RGB_SAI,	RGB_HUI,	XXXXXXX,	RGB_VAI,					RGB_M_K,	RGB_M_X,	RGB_M_G,	RGB_M_T,	XXXXXXX,	_______, \
		_______,	RGB_RMOD,	RGB_SAD,	RGB_HUD,	XXXXXXX,	RGB_VAD,					XXXXXXX,	XXXXXXX,	XXXXXXX,	XXXXXXX,	ENT_SLP,	_______, \
		                                    _______,	_______,	_______,					_______,	_______,	_______\
		)
};



//////////////////////////
/////GLOBAL VARIABLES/////
//////////////////////////

static bool power_tab = false;
static bool toggle_enable = false;
static bool is_oneshot_enable = false;
static uint16_t oneshot_key = KC_NO;



///////////////////
/////FUNCTIONS/////
///////////////////

void nrfmicro_power_enable(bool enable);
bool has_usb(void);

uint32_t layer_state_set_user(uint32_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
    if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
        layer_on(layer3);
    } else {
        layer_off(layer3);
    }
}

bool is_mod_down(uint16_t modifier) {
    return get_mods() & MOD_BIT(modifier);
}

void normal_key(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        register_code(keycode);
    } else {
        unregister_code(keycode);
    }
}

void normal_key_without_mod(uint16_t keycode, uint16_t modifier, keyrecord_t *record) {
    if (record->event.pressed) {
        unregister_code(modifier);
        register_code(keycode);
        register_code(modifier);
    } else {
        unregister_code(keycode);
    }
}

void toggle_mod(uint16_t modifier, keyrecord_t *record) {
    if (!record->event.pressed) {

        if (is_mod_down(modifier)) {
            unregister_code(modifier);
        } else {
            register_code(modifier);
        }

    }
}

void startPowerTab() {
    if (!power_tab) {
        power_tab = true;
        register_code(KC_LALT);
    }
}

void endPowerTab() {
    if (power_tab) {
        power_tab = false;
        unregister_code(KC_LALT);
    }
}

void startOneShot(uint16_t keycode) {
    if (!is_oneshot_enable) {
        is_oneshot_enable = true;
        oneshot_key = keycode;
        register_code(keycode);
    }
}

void endOneShot() {
    if (is_oneshot_enable) {
        unregister_code(oneshot_key);
        is_oneshot_enable = false;
        oneshot_key = KC_NO;
    }
}

///////////////////////////////
/////CUSTOM KEY BEHAVIOURS/////
///////////////////////////////

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

#ifdef SSD1306OLED
    iota_gfx_flush(); // wake up screen
#endif

    switch (keycode) {

        case POWER_TAB:
            if (record->event.pressed) {
                startPowerTab();
            }
            normal_key(KC_TAB, record);
            return false;
            break;

        case KC_BSPC:
            if (is_mod_down(KC_LSFT)) {
                normal_key_without_mod(KC_DEL, KC_LSFT, record);
            } else {
                normal_key(KC_BSPC, record);
            }
            return false;
            break;

        case KC_LSFT:
            if (!record->event.pressed) {
                unregister_code(KC_DEL);
            }

            if (toggle_enable) {
                toggle_mod(KC_LSFT, record);
            } else {
                normal_key(KC_LSFT, record);
            }
            return false;
            break;

        case KC_LCTL:
            if (toggle_enable) {
                toggle_mod(KC_LCTL, record);
            } else {
                normal_key(KC_LCTL, record);
            }
            return false;
            break;

        case KC_LALT:
            if (toggle_enable) {
                toggle_mod(KC_LALT, record);
            } else {
                normal_key(KC_LALT, record);
            }
            return false;
            break;

        case TOGGLE_MODS:
            if (record->event.pressed) {
                if (toggle_enable) {

                    toggle_enable = false;
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LCTL);
                    unregister_code(KC_LALT);

                } else {
                    toggle_enable = true;
                }
            }
            return false;
            break;

        case RGB_TOG:
            nrfmicro_power_enable(rgblight_config.enable);
            break;
            return false;

        case ENT_SLP:
            if (!record->event.pressed) {
                sleep_mode_enter();
            }
            return false;
    }

    if (!record->event.pressed) {
        endOneShot();
    }

    return true;
}



///////////////////
/////TAP DANCE/////
///////////////////

// Initialize tap structure associated with fl_win tap dance key
static tap fl_win_tap_state = {
        .is_press_action = true,
        .state = 0
};

uint8_t cur_dance(qk_tap_dance_state_t *state);
void fl_win_finished(qk_tap_dance_state_t *state, void *user_data);
void fl_win_reset(qk_tap_dance_state_t *state, void *user_data);

// Determine the current tap dance state
uint8_t cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed) {
            return SINGLE_TAP;
        } else {
            return SINGLE_HOLD;
        }
    } else if (state->count == 2) {
        return DOUBLE_TAP;
    }
    else return 8;
}

void fl_win_finished(qk_tap_dance_state_t *state, void *user_data) {
    fl_win_tap_state.state = cur_dance(state);
    switch (fl_win_tap_state.state) {
        case SINGLE_TAP:
            register_code(KC_LWIN);
            break;

        case SINGLE_HOLD:
            layer_on(_FL);
            break;

        case DOUBLE_TAP:
            startOneShot(KC_LWIN);
            register_code(KC_LWIN);
            break;
    }
}

void fl_win_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (fl_win_tap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_LWIN);
            break;

        case SINGLE_HOLD:
            layer_off(_FL);
            endPowerTab();
            break;

        case DOUBLE_TAP:
            break;
    }
    fl_win_tap_state.state = 0;
}

qk_tap_dance_action_t tap_dance_actions[] = {
        [FL_WIN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, fl_win_finished, fl_win_reset)
};




/////////////////////
/////OLED SCREEN/////
/////////////////////

#ifdef SSD1306OLED

const char *read_rgb_info(void);

void matrix_render_user(struct CharacterMatrix *matrix) {

    char oled_first_line[64], oled_third_line[32], vc[16];
    int vcc = get_vcc();

    #if (IS_LEFT_HAND)
        sprintf (oled_first_line, "Master: %s%s%s",
            get_usb_enabled() && !get_ble_enabled() ? "USB mode":"",
            get_ble_enabled() && ble_connected() ? "connected":"",
            get_ble_enabled() && !ble_connected() ? "disconnected":""
            );
    #else
        sprintf(oled_first_line, "Slave: %s", ble_connected() ? "connected" : "disconnected");
    #endif

    sprintf(vc, "%4dmV", vcc);
    sprintf(oled_third_line, "Bat: %s USB: %s", vcc<500 || vcc>4400 ? "off   " : vc, has_usb()? "on":"off");

    matrix_write_ln(matrix, oled_first_line);
    matrix_write_ln(matrix, read_rgb_info());
    matrix_write_ln(matrix, oled_third_line);

}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
    if (memcmp(dest->display, source->display, sizeof(dest->display))) {
        memcpy(dest->display, source->display, sizeof(dest->display));
        dest->dirty = true;
    }
}

void iota_gfx_task_user(void) {
	ScreenOffInterval = has_usb() ? 60*10*1000 : 60*5*1000; // ms
	struct CharacterMatrix matrix;
	matrix_clear(&matrix);
	matrix_render_user(&matrix);
	matrix_update(&display, &matrix);
}

#endif
