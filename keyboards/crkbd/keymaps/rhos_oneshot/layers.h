#pragma once

#undef OSM
#undef OS_LSFT
#undef OS_LCTL
#undef OS_LALT
#undef OS_LGUI
#undef OS_ALGR

#define LAYER_LIST \
LAYER_X(BASE) \
/* root layers ↑ */ \
LAYER_X(LFTB) \
LAYER_X(NAV) \
LAYER_X(NUM) \
LAYER_X(FUN) \
LAYER_X(LFT)

#define OSMOD_LIST \
OSMOD_X(LSFT) \
OSMOD_X(LCTL) \
OSMOD_X(LALT) \
OSMOD_X(LGUI) \
OSMOD_X(ALGR)

#define OSLAYER_LIST \
OSLAYER_X(NAV, NAV) \
OSLAYER_X(NUM, NUM) \
OSLAYER_X(LFT, LFT)

#define SW_LIST \
/*in app switcher - usually tabs*/\
SW_X(APP,   KC_LCTL, KC_TAB) \
/*recent windows switcher*/\
SW_X(WIN,   KC_LALT, KC_TAB)

/*recent workspace switcher*/
// SW_X(WRK,   KC_LGUI, KC_TAB)
#define SW_WRK  G(KC_TAB)
/*scratch workspace switcher*/
// SW_X(SCRT,  KC_LGUI, KC_GRV)
#define SW_SCRT  G(KC_GRV)

enum keycodes {
    OS_CNCL = SAFE_RANGE,

    FN_LCK, // lock top number level
    A_ALGR, // auto algr
    A_SHFT, // auto shft

    #define SW_X(SWNAME, SWMOD, SWKEY) SW_##SWNAME,
    SW_LIST
    #undef SW_X

    #define OSMOD_X(OSMOD) OS_##OSMOD,
    OSMOD_LIST
    #undef OSMOD_X

    #define OSLAYER_X(OSKEY, OSLAYER) OSL_##OSKEY,
    OSLAYER_LIST
    #undef OSLAYER_X
};

enum {
    TD_BOOT,
    TD_EECLR,
    TD_TOOL,
};

#define T_BOOT   TD(TD_BOOT)
#define T_EECLR  TD(TD_EECLR)
#define T_TOOL   TD(TD_TOOL)

#define U_UNDO  C(KC_Z)
#define U_CUT   C(KC_X)
#define U_COPY  C(KC_C)
#define U_PSTE  C(KC_V)

#define U_LNG   G(KC_SPC)
#define U_STAB  S(KC_TAB)

#define U_MMUTE KC_F20 // mic mute

#define TG_LFT TG(U_LFTB)

#define LAYER_BASE \
    OSL_LFT , KC_Q    , KC_W    , KC_E    , KC_R    , KC_T    ,           KC_Y    , KC_U    , KC_I    , KC_O    , KC_P    , KC_LBRC ,  \
    OS_CNCL , KC_A    , KC_S    , KC_D    , KC_F    , KC_G    ,           KC_H    , KC_J    , KC_K    , KC_L    , KC_SCLN , KC_QUOT ,  \
    OSL_NUM , KC_Z    , KC_X    , KC_C    , KC_V    , KC_B    ,           KC_N    , KC_M    , KC_COMM , KC_DOT  , KC_SLSH , KC_RBRC ,  \
                                    KC_ESC  , OSL_NAV , KC_SPC  ,       OS_LSFT , OSL_NUM , OS_ALGR


#define LAYER_NAV \
    _______ , U_STAB  , KC_TAB  , KC_ENT  , SW_APP  , SW_SCRT ,           KC_WBAK , KC_WFWD , KC_PGUP , KC_PGDN , KC_APP  , XXXXXXX ,  \
    _______ , OS_LALT , OS_LSFT , OS_LCTL , OS_LGUI , SW_WIN  ,           KC_LEFT , KC_DOWN , KC_UP   , KC_RGHT , KC_BSPC , XXXXXXX ,  \
    _______ , U_UNDO  , U_CUT   , U_COPY  , U_PSTE  , SW_WRK  ,           U_LNG   , KC_DEL  , KC_HOME , KC_END  , KC_INS  , XXXXXXX ,  \
                                    _______ , _______ , _______ ,       _______ , _______ , _______

#define LAYER_NUM \
    _______ , KC_MINS , KC_7    , KC_8    , KC_9    , KC_GRV  ,           KC_LCBR , KC_RCBR , KC_LPRN , KC_RPRN , KC_UNDS , XXXXXXX ,  \
    _______ , KC_0    , KC_4    , KC_5    , KC_6    , KC_EQL  ,           KC_EXLM , OS_LGUI , OS_LCTL , OS_LSFT , OS_LALT , XXXXXXX ,  \
    _______ , KC_DOT  , KC_1    , KC_2    , KC_3    , KC_BSLS ,           KC_LBRC , KC_RBRC , KC_LT   , KC_GT   , KC_QUES , XXXXXXX ,  \
                                    _______ , _______ , _______ ,       _______ , _______ , _______

#define LAYER_FUN \
    _______ , KC_F12  , KC_F7   , KC_F8   , KC_F9   , XXXXXXX ,           XXXXXXX , A_SHFT  , KC_PAUS , KC_SCRL , XXXXXXX , T_BOOT  ,  \
    _______ , KC_F11  , KC_F4   , KC_F5   , KC_F6   , XXXXXXX ,           KC_CAPS , OS_LGUI , OS_LCTL , OS_LSFT , OS_LALT , T_EECLR ,  \
    _______ , KC_F10  , KC_F1   , KC_F2   , KC_F3   , XXXXXXX ,           NK_TOGG , A_ALGR  , XXXXXXX , XXXXXXX , XXXXXXX , XXXXXXX ,  \
                                    _______ , _______ , _______ ,       _______ , _______ , _______

#define LAYER_LFT \
    _______ , XXXXXXX , KC_PGUP , KC_UP   , KC_PGDN , XXXXXXX ,           KC_VOLD , KC_VOLU , KC_MUTE , U_MMUTE , KC_PSCR , T_TOOL  ,  \
    _______ , KC_BSPC , KC_LEFT , KC_DOWN , KC_RGHT , TG_LFT  ,           KC_BRID , KC_BRIU , KC_MSTP , KC_MPLY , KC_MPRV , KC_MNXT ,  \
    _______ , XXXXXXX , KC_HOME , KC_END  , KC_DEL  , XXXXXXX ,           XXXXXXX , XXXXXXX , XXXXXXX , XXXXXXX , XXXXXXX , XXXXXXX ,  \
                                    _______ , _______ , _______ ,       _______ , _______ , _______

#define LAYER_LFTB \
    LAYER_LFT
