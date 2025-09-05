#include "ui.h"
#include "st7735.h"
#include "ugui.h"

static char* state_str[] = {
    "Home Page", "Home", "Menu", "Setting",
    "Setting", "Current", "Delay"
};

UG_GUI gui;
#define UI_FONT FONT_8X14
void ui_init()
{
    ST7735_Init();

    UG_Init(&gui, ST7735_DrawPixel, 128, 160);
    UG_FontSelect(&FONT_8X14);
    UG_DriverRegister(DRIVER_FILL_FRAME, ST7789_FillRectangle_XY);
    UG_FillScreen(C_BLACK);

    ST7735_BLK_CTRL(1);
}

void ui_print_state(UI_StateTypeDef state)
{
    UG_PutString(0, 0, state_str[state]);
}

void ui_clear_state()
{
    UG_PutString(0, 0, "    ");
}

void ui_disp_page(UI_StateTypeDef state)
{
    switch (state) {
        case UI_IDLE:
            UG_PutString(2, UI_FONT.char_height, "CUR: ");
            UG_PutString(2, UI_FONT.char_height*2, "TIME: ");
            break;
        case UI_CUR_SEL:
            break;
        case UI_DLY_SEL:
            break;
        case UI_CUR_SET:
            break;
        case UI_DLY_SET:
            break;
        default:
            break;
    }
}
