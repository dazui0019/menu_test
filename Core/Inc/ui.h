#pragma once

typedef enum{
    UI_GROUP = 0,
    UI_IDLE,
    UI_MENU,
    UI_CUR_SEL,
    UI_DLY_SEL,
    UI_CUR_SET,
    UI_DLY_SET,
}UI_StateTypeDef;

void ui_init();
void ui_print_state(UI_StateTypeDef state);
void ui_clear_state();
void ui_disp_page(UI_StateTypeDef state);
