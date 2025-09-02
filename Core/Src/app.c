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

struct stateMachine m;

void timer_5ms_interrupt_handler(TIM_HandleTypeDef *htim);

static void printEnterMsg( void *stateData, struct event *event );
static void printExitMsg( void *stateData, struct event *event );
static void printErrMsg( void *stateData, struct event *event );

/* Types of events */
enum eventType {
   Event_button,
};

static bool compareButton( void *ch, struct event *event );

static struct state menuGroupState, idleState, menuState;

static struct state menuGroupState = {
   .parentState = NULL,
    .entryState = &idleState,
    .transitions = (struct transition[]){
        { Event_button, NULL, NULL, NULL, &idleState},
    },
    .numTransitions = 1,
    .data = "group",
    .entryAction = printEnterMsg,
    .exitAction = printExitMsg,
};

static struct state idleState = {
    .parentState = &menuGroupState,
    .entryState = NULL,
    .transitions = (struct transition[]){
        { Event_button, NULL, NULL, NULL, &menuState },
    },
    .numTransitions = 1,
    .data = "idle",
    .entryAction = printEnterMsg,
    .exitAction = printExitMsg,
};

static struct state menuState = {
    .parentState = &menuGroupState,
    .entryState = NULL,
    .transitions = (struct transition[]){
        { Event_button, (void *)BTN_LONG_PRESS_START, NULL, NULL, &idleState },
    },
    .numTransitions = 1,
    .data = "menu",
    .entryAction = printEnterMsg,
    .exitAction = printExitMsg,
};

static struct state errorState = {
   .entryAction = &printErrMsg
};

static Button btn0;

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
            printf("Button 0: Single Click\r\n");
            stateM_handleEvent( &m, &(struct event){ Event_button,
            (void *)(intptr_t)BTN_SINGLE_CLICK } );
            break;
        case BTN_DOUBLE_CLICK:
            printf("Button 0: Double Click\r\n");
            stateM_handleEvent( &m, &(struct event){ Event_button,
            (void *)(intptr_t)BTN_DOUBLE_CLICK } );
            break;
        case BTN_LONG_PRESS_START:
            printf("Button 0: Long Press Start\r\n");
            stateM_handleEvent( &m, &(struct event){ Event_button,
            (void *)(intptr_t)BTN_LONG_PRESS_START } );
            break;
        default:
            break;
    }
}

void setup()
{
    stateM_init( &m, &idleState, &errorState );

    EventRecorderInitialize (EventRecordAll, 1);
    // 初始化按键 (active_level: 0=低电平有效, 1=高电平有效)
    button_init(&btn0, read_button_gpio, 0, 1);
    printf("Hello, World!\r\n");
    button_attach(&btn0, BTN_SINGLE_CLICK, btn0_handler);
    button_attach(&btn0, BTN_DOUBLE_CLICK, btn0_handler);

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

static void printErrMsg( void *stateData, struct event *event )
{
   printf( "ENTERED ERROR STATE!\n" );
}

static void printEnterMsg( void *stateData, struct event *event )
{
   printf( "Entering %s state\n", (char *)stateData );
}

static void printExitMsg( void *stateData, struct event *event )
{
   printf( "Exiting %s state\n", (char *)stateData );
}
