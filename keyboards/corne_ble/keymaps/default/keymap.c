#include QMK_KEYBOARD_H
#include "app_ble_func.h"

#ifdef SSD1306OLED
#include "ssd1306.h"
#endif

#include "eeprom.h"

// define variables for reactive RGB
bool TOG_STATUS = false;
int RGB_current_mode;
#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

const uint8_t is_master = IS_LEFT_HAND;

void nrfmicro_power_enable(bool enable);

bool has_usb(void);

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
    AD_WO_L = SAFE_RANGE, /* Start advertising without whitelist  */
    BLE_DIS,              /* Disable BLE HID sending              */
    BLE_EN,               /* Enable BLE HID sending               */
    USB_DIS,              /* Disable USB HID sending              */
    USB_EN,               /* Enable USB HID sending               */
    DELBNDS,              /* Delete all bonding                   */
    ADV_ID0,              /* Start advertising to PeerID 0        */
    ADV_ID1,              /* Start advertising to PeerID 1        */
    ADV_ID2,              /* Start advertising to PeerID 2        */
    ADV_ID3,              /* Start advertising to PeerID 3        */
    ADV_ID4,              /* Start advertising to PeerID 4        */
    BATT_LV,              /* Display battery level in milli volts */
    DEL_ID0,              /* Delete bonding of PeerID 0           */
    DEL_ID1,              /* Delete bonding of PeerID 1           */
    DEL_ID2,              /* Delete bonding of PeerID 2           */
    DEL_ID3,              /* Delete bonding of PeerID 3           */
    DEL_ID4,              /* Delete bonding of PeerID 4           */
    ENT_DFU,              /* Start bootloader                     */
    ENT_SLP,              /* Deep sleep mode                      */
    QWERTY,
    LOWER,
    RAISE,
    POWER_TAB,
    FL_PT,
    KEY_LOCK,
    ADJUST,
    RGBRST
};


extern keymap_config_t keymap_config;

// Fillers to make layering more clear
#define _______ KC_TRNS
#define XXXXXXX KC_NO
#define KC_XXXXX KC_NO
#define KC_LOWER LOWER
#define KC_RAISE RAISE
#define KC_RST   RESET
#define KC_LRST  RGBRST
#define KC_LTOG  RGB_TOG
#define KC_LHUI  RGB_HUI
#define KC_LHUD  RGB_HUD
#define KC_LSAI  RGB_SAI
#define KC_LSAD  RGB_SAD
#define KC_LVAI  RGB_VAI
#define KC_LVAD  RGB_VAD
#define KC_LSMOD RGB_SMOD
#define KC_CTLTB CTL_T(KC_TAB)
#define KC_GUIEI GUI_T(KC_LANG2)
#define KC_ALTKN ALT_T(KC_LANG1)


#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)

#define RBR_RGU MT(MOD_RGUI, KC_RBRC)
#define F12_RGU MT(MOD_RGUI, KC_F12)
#define PLS_LCT MT(MOD_LCTL, KC_PPLS)
#define EQL_LCT MT(MOD_LCTL, KC_PEQL)
#define APP_LCT MT(MOD_LCTL, KC_APP)
#define EQL_RCT MT(MOD_RCTL, KC_PEQL)
#define QUO_RCT MT(MOD_RCTL, KC_QUOT)
#define APP_RCT MT(MOD_RCTL, KC_APP)
#define MIN_RCT MT(MOD_RCTL, KC_MINS)
#define EQL_LAL MT(MOD_LALT, KC_EQL)
#define BSL_RAL MT(MOD_RALT, KC_BSLS)

#define NBS_LCT MT(MOD_LCTL, KC_NUBS)
#define BSH_LAL MT(MOD_LALT, KC_BSLS)
#define APP_RAL MT(MOD_RALT, KC_APP)

#define BSP_LSH MT(MOD_LSFT, KC_BSPC)
#define SPC_RSH MT(MOD_RSFT, KC_SPC)
#define DEL_RSE LT(_RAISE, KC_DEL)
#define TAB_RSE LT(_RAISE, KC_TAB)
#define ENT_LWR LT(_LOWER, KC_ENT)
#define ESC_LWR LT(_LOWER, KC_ESC)

#define SH_TG KC_TRNS


uint32_t layer_state_set_user(uint32_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_QWERTY] = LAYOUT(\
		KC_ESC,		KC_Q,		KC_W,		KC_E,		KC_R,		KC_T,						KC_Y,		KC_U,		KC_I,		KC_O,		KC_P,		KC_TAB, \
		KC_LSFT,	KC_A,		KC_S,		KC_D,		KC_F,		KC_G,						KC_H,		KC_J,		KC_K,		KC_L,		KC_QUOT,	RSFT_T(KC_CAPS), \
		KC_LCTL,	KC_Z,		KC_X,		KC_C,		KC_V,		KC_B,						KC_N,		KC_M,		KC_COMM,	KC_DOT,		KC_SCLN,	KC_LALT, \
											FL_PT,		KC_SPC,		LT(_RAISE, KC_ENT),			KC_RALT,	KC_BSPC,	MO(_LOWER)\
		),

[_LOWER] = LAYOUT(\
		_______,	KC_DQUO,	KC_LWIN,	KC_EXLM,	KC_QUES,	XXXXXXX,					KC_AMPR,	KC_PIPE,	KC_PSCR,	XXXXXXX,	KC_PERC,	_______, \
		_______,	KC_AT,		KC_DLR,		KC_LPRN,	KC_RPRN,	XXXXXXX,					KC_HASH,	XXXXXXX,	XXXXXXX,	XXXXXXX,	KC_GRV,		_______, \
		_______,	XXXXXXX,	KC_CIRC,	KC_LBRC,	KC_RBRC,	XXXXXXX,					XXXXXXX,	KC_MINS,	XXXXXXX,	XXXXXXX,	XXXXXXX,	KEY_LOCK, \
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
		RGB_TOG,	XXXXXXX,	XXXXXXX,	XXXXXXX,	XXXXXXX,	XXXXXXX,					RGB_M_P,	RGB_M_B,	RGB_M_R,	RGB_M_SW,	RGB_M_SN,	_______,	 \
		_______,	RGB_MOD,	RGB_SAI,	RGB_HUI,	XXXXXXX,	RGB_VAI,					RGB_M_K,	RGB_M_X,	RGB_M_G,	RGB_M_T,	XXXXXXX,	_______, \
		_______,	RGB_RMOD,	RGB_SAD,	RGB_HUD,	XXXXXXX,	RGB_VAD,					XXXXXXX,	XXXXXXX,	XXXXXXX,	XXXXXXX,	XXXXXXX,	_______, \
		                                    _______,	_______,	_______,					_______,	_______,	_______\
		)
};

void persistent_default_layer_set(uint16_t default_layer) {
    eeconfig_update_default_layer(default_layer);
    default_layer_set(default_layer);
    layer_state_set(default_layer);
}

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
    if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
#ifdef RGBLIGHT_ENABLE
        //rgblight_mode(RGB_current_mode);
#endif
        layer_on(layer3);
    } else {
        layer_off(layer3);
    }
}

// When add source files to SRC in rules.mk, you can use functions.
const char *read_layer_state(void);
const char *read_logo(void);
void set_keylog(uint16_t keycode, keyrecord_t *record);
const char *read_keylog(void);
const char *read_keylogs(void);

static bool shift_down = false;
static bool ctrl_down = false;
static bool alt_down = false;
static bool power_tab = false;

static bool toggle_enable = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    char str[16];

    //NRF_LOG_INFO("process_record_user, keycode: %d\n", keycode);

#ifdef SSD1306OLED
    iota_gfx_flush(); // wake up screen
#endif

    if (record->event.pressed) {
        set_keylog(keycode, record);
        //set_timelog();
    }

    switch (keycode) {
        case RGB_TOG:
            if (record->event.pressed) {
                // only enable the power for now, never disable
                if (!rgblight_config.enable) {
                    //nrfmicro_power_enable(!rgblight_config.enable);
                }
#ifdef SSD1306OLED
                //iota_gfx_init(!IS_LEFT_HAND); // enable the OLED screen
#endif
            }
            break;

        case QWERTY:
            if (record->event.pressed) {
                persistent_default_layer_set(1UL<<_QWERTY);
            }
            return false;
            break;
        case LOWER:
            if (record->event.pressed) {
                //not sure how to have keyboard check mode and set it to a variable, so my work around
                //uses another variable that would be set to true after the first time a reactive key is pressed.
                if (TOG_STATUS) { //TOG_STATUS checks is another reactive key currently pressed, only changes RGB mode if returns false
                } else {
                    TOG_STATUS = !TOG_STATUS;
#ifdef RGBLIGHT_ENABLE
                    //rgblight_mode(16);
#endif
                }
                layer_on(_LOWER);
                update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
            } else {
#ifdef RGBLIGHT_ENABLE
                //rgblight_mode(RGB_current_mode);   // revert RGB to initial mode prior to RGB mode change
#endif
                TOG_STATUS = false;
                layer_off(_LOWER);
                update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
            }
            return false;
            break;
        case RAISE:
            if (record->event.pressed) {
                //not sure how to have keyboard check mode and set it to a variable, so my work around
                //uses another variable that would be set to true after the first time a reactive key is pressed.
                if (TOG_STATUS) { //TOG_STATUS checks is another reactive key currently pressed, only changes RGB mode if returns false
                } else {
                    TOG_STATUS = !TOG_STATUS;
#ifdef RGBLIGHT_ENABLE
                    //rgblight_mode(15);
#endif
                }
                layer_on(_RAISE);
                update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
            } else {
#ifdef RGBLIGHT_ENABLE
                //rgblight_mode(RGB_current_mode);  // revert RGB to initial mode prior to RGB mode change
#endif
                layer_off(_RAISE);
                TOG_STATUS = false;
                update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
            }
            return false;
            break;
        case ADJUST:
            if (record->event.pressed) {
                layer_on(_ADJUST);
            } else {
                layer_off(_ADJUST);
            }
            return false;
            break;
            //led operations - RGB mode change now updates the RGB_current_mode to allow the right RGB mode to be set after reactive keys are released
        case RGB_MOD:
#ifdef RGBLIGHT_ENABLE
            if (record->event.pressed) {
				rgblight_mode(RGB_current_mode);
				rgblight_step();
				RGB_current_mode = rgblight_config.mode;
				NRF_LOG_INFO("RGB_MOD, RGB_current_mode: %d\n", RGB_current_mode);
			}
#endif
            return false;
            break;
        case RGBRST:
#ifdef RGBLIGHT_ENABLE
            if (record->event.pressed) {
				nrfmicro_power_enable(true);
				eeconfig_update_rgblight_default();
				rgblight_enable();
				RGB_current_mode = rgblight_config.mode;
				NRF_LOG_INFO("RGBRST, RGB_current_mode: %d\n", RGB_current_mode);
			}
#endif

#ifdef SSD1306OLED
            //iota_gfx_init(!IS_LEFT_HAND); // enable the OLED screen
#endif
            break;



 /////////////////
 ///CUSTOM KEYS///
 ////////////////

        case POWER_TAB:
            if (record->event.pressed) {
                power_tab = true;
                register_code(KC_LALT);
                register_code(KC_TAB);
            } else {
                unregister_code(KC_TAB);
            }
            return false;
            break;

        case KC_LSFT:
            if (record->event.pressed) {

                if(!toggle_enable) {
                    shift_down = true;
                }
                register_code(KC_LSFT);

            } else {

                if (!toggle_enable) {

                    unregister_code(KC_LSFT);
                    unregister_code(KC_DEL);
                    shift_down = false;

                } else {
                    if (shift_down) {
                        unregister_code(KC_LSFT);
                        unregister_code(KC_DEL);
                        shift_down = false;
                    } else {
                        shift_down = true;
                        register_code(KC_LSFT);
                    }
                }

            }
            return false;
            break;

        case KC_LCTL:
            if (record->event.pressed) {

                if(!toggle_enable) {
                    ctrl_down = true;
                }
                register_code(KC_LCTL);

            } else {

                if (!toggle_enable) {

                    unregister_code(KC_LCTL);
                    ctrl_down = false;

                } else {
                    if (ctrl_down) {
                        unregister_code(KC_LCTL);
                        ctrl_down = false;
                    } else {
                        ctrl_down = true;
                        register_code(KC_LCTL);
                    }
                }

            }
            return false;
            break;

        case KC_LALT:
            if (record->event.pressed) {

                if(!toggle_enable) {
                    alt_down = true;
                }
                register_code(KC_LALT);

            } else {

                if (!toggle_enable) {

                    unregister_code(KC_LALT);
                    alt_down = false;

                } else {
                    if (alt_down) {
                        unregister_code(KC_LALT);
                        alt_down = false;
                    } else {
                        alt_down = true;
                        register_code(KC_LALT);
                    }
                }

            }
            return false;
            break;

        case KC_BSPC:
            if (record->event.pressed) {
                if (shift_down) {
                    unregister_code(KC_LSFT);
                    register_code(KC_DEL);
                } else {
                    register_code(KC_BSPC);
                }

            } else {
                if (shift_down) {
                    unregister_code(KC_DEL);
                    register_code(KC_LSFT);
                } else {
                    unregister_code(KC_BSPC);
                }
            }
            return false;
            break;

        case FL_PT:
            if (record->event.pressed) {
                layer_on(_FL);
            } else {

                if (power_tab) {
                    unregister_code(KC_LALT);
                    power_tab = false;
                }

                layer_off(_FL);

            }
            return false;
            break;

        case KEY_LOCK:
            if (record->event.pressed) {
                if (toggle_enable) {

                    toggle_enable = false;
                    shift_down = false;
                    unregister_code(KC_LSFT);

                } else {
                    toggle_enable = true;
                }
            }
            return false;
            break;
    }

    if (record->event.pressed) {
        switch(keycode) {
            case DELBNDS:
                //delete_bonds();
                //super weird issue - RGBTOG overlaps DELBNDS
                //not sure why, matrix issue probably
                //use 3 thumb keys + reset button for now
                NRF_LOG_INFO("DELBNDS overlaps RGBTOG, temporarily disabled");
                return false;
            case AD_WO_L:
                restart_advertising_wo_whitelist();
                return false;
            case USB_EN:
                set_usb_enabled(true);
                return false;
                break;
            case USB_DIS:
                set_usb_enabled(false);
                return false;
                break;
            case BLE_EN:
                set_ble_enabled(true);
                return false;
                break;
            case BLE_DIS:
                set_ble_enabled(false);
                return false;
                break;
            case ADV_ID0:
                restart_advertising_id(0);
                return false;
            case ADV_ID1:
                restart_advertising_id(1);
                return false;
            case ADV_ID2:
                restart_advertising_id(2);
                return false;
            case ADV_ID3:
                restart_advertising_id(3);
                return false;
            case ADV_ID4:
                restart_advertising_id(4);
                return false;
            case DEL_ID0:
                delete_bond_id(0);
                return false;
            case DEL_ID1:
                delete_bond_id(1);
                return false;
            case DEL_ID2:
                delete_bond_id(2);
                return false;
            case DEL_ID3:
                delete_bond_id(3);
                return false;
            case BATT_LV:
                sprintf(str, "%4dmV", get_vcc());
                send_string(str);
                return false;
            case ENT_DFU:
                bootloader_jump();
                return false;
        }
    }
    else if (!record->event.pressed) {
        switch (keycode) {
            case ENT_SLP:
                sleep_mode_enter();
                return false;
        }
    }

    return true;
}
;


#ifdef SSD1306OLED

void matrix_update(struct CharacterMatrix *dest,
		const struct CharacterMatrix *source) {
		if (memcmp(dest->display, source->display, sizeof(dest->display))) {
			memcpy(dest->display, source->display, sizeof(dest->display));
			dest->dirty = true;
		}
	}

    //const char *read_host_led_state(void);
    //const char *read_mode_icon(bool swap);

	const char *read_rgb_info(void);

	void matrix_render_user(struct CharacterMatrix *matrix) {


        //if (is_master)
		{
		    //matrix_write_ln(matrix, read_layer_state());
		    //matrix_write_ln(matrix, read_keylog());

			{
				char str[64];

#if (IS_LEFT_HAND)
				sprintf (str, "Master: %s%s%s",
					get_usb_enabled() && !get_ble_enabled() ? "USB mode":"",
					get_ble_enabled() && ble_connected() ? "connected":"",
					get_ble_enabled() && !ble_connected() ? "disconnected":""
					);
#else
				sprintf(str, "Slave: %s", ble_connected() ? "connected" : "disconnected");
#endif

				matrix_write_ln(matrix, str);
			}

			matrix_write_ln(matrix, read_rgb_info());

			{
				char vc[16], str[32];
				int vcc = get_vcc();
				sprintf(vc, "%4dmV", vcc);
				sprintf(str, "Bat: %s USB: %s", vcc<500 || vcc>4400 ? "off   " : vc, has_usb()? "on":"off");
				matrix_write_ln(matrix, str);
			}

		    //matrix_write_ln(matrix, read_keylogs());
		    //matrix_write_ln(matrix, read_mode_icon(keymap_config.swap_lalt_lgui));
		    //matrix_write_ln(matrix, read_host_led_state());
		    //matrix_write_ln(matrix, read_timelog());
		}
        //else {
        //   matrix_write_ln(matrix, read_layer_state()); // somehow removes the dead pixel
        //   matrix_write(matrix, read_logo());
        //}

	}

	void iota_gfx_task_user(void) {
	ScreenOffInterval = has_usb() ? 60*10*1000 : 60*5*1000; // ms
	struct CharacterMatrix matrix;
	matrix_clear(&matrix);
	matrix_render_user(&matrix);
	matrix_update(&display, &matrix);
}

#endif
