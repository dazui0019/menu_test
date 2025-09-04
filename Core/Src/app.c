#include "app.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "stateMachine.h"
#include "multi_button.h"
#include "printf.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"
#include "time.h"
#include "EventRecorder.h"
#include "st7735.h"
#include "ugui.h"

enum LCD_STATE{
    LCD_GROUP,
    LCD_IDLE,
    LCD_MENU,
    LCD_CUR_SEL,
    LCD_DLY_SEL,
    LCD_CUR_SET,
    LCD_DLY_SET,
};

struct stateData{
    uint32_t num;
    void* str;
};

void timer_5ms_interrupt_handler(TIM_HandleTypeDef *htim);
static bool compareButtonVal( void *btn_val, struct event *event );

void ui_init();

// stateMachine
void settingAction( void *currentStateData, struct event *event, void *newStateData );
void pageAction( void *currentStateData, struct event *event, void *newStateData );
static void printEnterMsg( void *stateData, struct event *event );
static void printExitMsg( void *stateData, struct event *event );
static void printErrMsg( void *stateData, struct event *event );

/* Types of events */
enum eventType {
    Event_lcd_init,
    Event_button,
    Event_refresh,
};

static Button btn0;

struct stateMachine m;
static struct state lcdGroupState, idleState, menuGroupState, selCurState, selDlyState, setCurState, setDlyState;

static struct state lcdGroupState = {
   .parentState = NULL,
    .entryState = &idleState,
    .transitions = (struct transition[]){
        { Event_lcd_init, NULL, NULL, pageAction, &idleState},
        { Event_button, NULL, NULL, pageAction, &idleState},
    },
    .numTransitions = 1,
    .data = &(struct stateData){.num=LCD_GROUP, .str="lcd"},
    .entryAction = printEnterMsg,
    .exitAction = printExitMsg,
};

static struct state idleState = {
    .parentState = &lcdGroupState,
    .entryState = NULL,
    .transitions = (struct transition[]){
        { Event_lcd_init, NULL, NULL, pageAction, &idleState},
        { Event_button, (void *)BTN_LONG_PRESS_START, compareButtonVal, pageAction, &selCurState },
        {Event_button, NULL, NULL, NULL, &idleState}
    },
    .numTransitions = 3,
    .data = &(struct stateData){.num=LCD_IDLE, .str="idle"},
    .entryAction = printEnterMsg,
    .exitAction = printExitMsg,
};

static struct state menuGroupState = {
    .parentState = &lcdGroupState,
    .entryState = NULL,
    .transitions = (struct transition[]){
        { Event_button, (void *)BTN_LONG_PRESS_START, compareButtonVal, pageAction, &selCurState },
        {Event_button, NULL, NULL, NULL, &menuGroupState},
    },
    .numTransitions = 2,
    .data = &(struct stateData){.num=LCD_MENU, .str="menu"},
    .entryAction = printEnterMsg,
    .exitAction = printExitMsg,
};

static struct state selCurState = {
    .parentState = &menuGroupState,
    .entryState = NULL,
    .transitions = (struct transition[]){
        { Event_button, (void *)BTN_LONG_PRESS_START, compareButtonVal, pageAction, &idleState },
        { Event_button, (void *)BTN_SINGLE_CLICK, compareButtonVal, pageAction, &selDlyState },
        { Event_button, (void *)BTN_DOUBLE_CLICK, compareButtonVal, pageAction, &setCurState },
        { Event_button, NULL, NULL, NULL, &selCurState },
    },
    .numTransitions = 4,
    .data = &(struct stateData){.num=LCD_CUR_SEL, .str="cur selcted"},
    .entryAction = printEnterMsg,
    .exitAction = printExitMsg,
};

static struct state selDlyState = {
    .parentState = &menuGroupState,
    .entryState = NULL,
    .transitions = (struct transition[]){
        { Event_button, (void *)BTN_LONG_PRESS_START, compareButtonVal, pageAction, &idleState },
        { Event_button, (void *)BTN_SINGLE_CLICK, compareButtonVal, pageAction, &selCurState },
        { Event_button, (void *)BTN_DOUBLE_CLICK, compareButtonVal, pageAction, &setDlyState },
        { Event_button, NULL, NULL, NULL, &selDlyState },
    },
    .numTransitions = 4,
    .data = &(struct stateData){.num=LCD_DLY_SEL, .str="dly selcted"},
    .entryAction = printEnterMsg,
    .exitAction = printExitMsg,
};

static struct state setCurState = {
    .parentState = &menuGroupState,
    .entryState = NULL,
    .transitions = (struct transition[]){
        { Event_button, (void *)BTN_LONG_PRESS_START, compareButtonVal, pageAction, &selCurState },
        { Event_button, (void *)BTN_SINGLE_CLICK, compareButtonVal, settingAction, &setCurState },
        { Event_button, (void *)BTN_DOUBLE_CLICK, compareButtonVal, NULL, &setCurState },
        { Event_button, NULL, NULL, NULL, &setCurState },
    },
    .numTransitions = 4,
    .data = &(struct stateData){.num=LCD_CUR_SET, .str="cur setting"},
    .entryAction = printEnterMsg,
    .exitAction = printExitMsg,
};

static struct state setDlyState = {
    .parentState = &menuGroupState,
    .entryState = NULL,
    .transitions = (struct transition[]){
        { Event_button, (void *)BTN_LONG_PRESS_START, compareButtonVal, pageAction, &selDlyState },
        { Event_button, (void *)BTN_SINGLE_CLICK, compareButtonVal, settingAction, &setDlyState },
        { Event_button, (void *)BTN_DOUBLE_CLICK, compareButtonVal, NULL, &setDlyState },
        { Event_button, NULL, NULL, NULL, &setDlyState }, 
    },
    .numTransitions = 4,
    .data = &(struct stateData){.num=LCD_DLY_SET, .str="dly setting"},
    .entryAction = printEnterMsg,
    .exitAction = printExitMsg,
};

static struct state errorState = {
   .entryAction = &printErrMsg
};

uint8_t read_button_gpio(uint8_t button_id)
{
    switch (button_id) {
        case 1:
            return HAL_GPIO_ReadPin(BTN0_GPIO_Port, BTN0_Pin);
        default:
            return 0;
    }
}

void btn0_handler(Button* btn_handle)
{
    switch(btn_handle->event){
        case BTN_SINGLE_CLICK:
            // printf("Button 0: Single Click\r\n");
            stateM_handleEvent( &m, &(struct event){ Event_button,
            (void *)(intptr_t)BTN_SINGLE_CLICK } );
            break;
        case BTN_DOUBLE_CLICK:
            // printf("Button 0: Double Click\r\n");
            stateM_handleEvent( &m, &(struct event){ Event_button,
            (void *)(intptr_t)BTN_DOUBLE_CLICK } );
            break;
        case BTN_LONG_PRESS_START:
            // printf("Button 0: Long Press Start\r\n");
            stateM_handleEvent( &m, &(struct event){ Event_button,
            (void *)(intptr_t)BTN_LONG_PRESS_START } );
            break;
        case BTN_PRESS_REPEAT:
            // printf("Button 0: Press Repeat\r\n");
            break;
        case BTN_PRESS_UP:
            // printf("Button 0: Press UP\r\n");
            stateM_handleEvent( &m, &(struct event){ Event_button,
            (void *)(intptr_t)BTN_PRESS_UP } );
            break;
        default:
            break;
    }
}

void setup()
{
    ST7735_Init();
    ST7735_FillScreen(ST7735_WHITE);
    ST7735_BLK_CTRL(1);
    ui_init();

    stateM_init( &m, &idleState, &errorState );
    stateM_handleEvent( &m, &(struct event){ Event_lcd_init, NULL } );

    EventRecorderInitialize (EventRecordAll, 1);
    // 初始化按键 (active_level: 0=低电平有效, 1=高电平有效)
    button_init(&btn0, read_button_gpio, 0, 1);
    printf("Hello, World!\r\n");
    button_attach(&btn0, BTN_SINGLE_CLICK, btn0_handler);
    button_attach(&btn0, BTN_DOUBLE_CLICK, btn0_handler);
    button_attach(&btn0, BTN_LONG_PRESS_START, btn0_handler);
    button_attach(&btn0, BTN_PRESS_UP, btn0_handler);

    HAL_TIM_RegisterCallback(&htim10, HAL_TIM_PERIOD_ELAPSED_CB_ID, timer_5ms_interrupt_handler);

    HAL_TIM_Base_Start_IT(&htim10);

    button_start(&btn0);
}

void loop()
{
    HAL_GPIO_TogglePin(LD0_GPIO_Port, LD0_Pin);
    HAL_Delay(100);
}

void timer_5ms_interrupt_handler(TIM_HandleTypeDef *htim)
{
    button_ticks();
}

static bool compareButtonVal( void *btn_val, struct event *event )
{
   if ( event->type != Event_button )
      return false;

   return (intptr_t)btn_val == (intptr_t)event->data;
}

// 页面切换动作
void pageAction( void *currentStateData, struct event *event, void *newStateData )
{
    printf("%s --> %s\r\n", (char *)(((struct stateData*)currentStateData)->str), (char *)(((struct stateData*)newStateData)->str));
    switch((uint32_t)(((struct stateData*)newStateData)->num)){
        case LCD_IDLE:
            UG_PutString(0, 0, "IDLE  ");
            break;
        case LCD_CUR_SEL:
            UG_PutString(0, 0, "CUR SEL");
            break;
        case LCD_DLY_SEL:
            UG_PutString(0, 0, "DLY SEL");
            break;
        case LCD_CUR_SET:
            UG_PutString(0, 0, "CUR SET");
            break;
        case LCD_DLY_SET:
            UG_PutString(0, 0, "DLY SET");
            break;
        default:
            break;
    }
}

// 设置动作
void settingAction( void *currentStateData, struct event *event, void *newStateData )
{
    printf("%s --> %s\r\n", (char *)(((struct stateData*)currentStateData)->str), (char *)(((struct stateData*)newStateData)->str));
    if(((struct stateData* )currentStateData)->num == ((struct stateData* )newStateData)->num){
        if ( (ButtonEvent)((intptr_t)(event->data)) != BTN_SINGLE_CLICK )
            return;
        switch (((struct stateData* )currentStateData)->num) {
            case LCD_DLY_SET:
                printf("dly ++\r\n");
                break;
            case LCD_CUR_SET:
                printf("cur ++\r\n");
                break;
            default:
                printf("unknow ++\r\n");
                break;
        }
    }
}

void refreshAction(void *currentStateData, struct event *event, void *newStateData)
{
    if(((struct stateData* )currentStateData)->num != ((struct stateData* )newStateData)->num)
        return;
    switch (((struct stateData* )newStateData)->num) {
        case LCD_IDLE:
            break;
        case LCD_CUR_SEL:
            break;
        case LCD_DLY_SEL:
            break;
        case LCD_CUR_SET:
            break;
        case LCD_DLY_SET:
            break;
        default:
            break;
    }
}

static void printErrMsg( void *stateData, struct event *event )
{
   printf( "ENTERED ERROR STATE!\n" );
}

static void printEnterMsg( void *stateData, struct event *event )
{
    struct stateData* data = (struct stateData*)stateData;
    printf( "Entering %s state\n", (char *)data->str );
}

static void printExitMsg( void *stateData, struct event *event )
{
    struct stateData* data = (struct stateData*)stateData;
    printf( "Exiting %s state\n", (char *)data->str );
}

UG_GUI gui;
#define UI_FONT FONT_8X14
void ui_init()
{
    UG_Init(&gui, ST7735_DrawPixel, 128, 160);
    UG_FontSelect(&FONT_8X14);
    UG_DriverRegister(DRIVER_FILL_FRAME, ST7789_FillRectangle_XY);
    UG_FillScreen(C_BLACK);
}
