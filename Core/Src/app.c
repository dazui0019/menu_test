#include "app.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "stateMachine.h"

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
    .entryAction = NULL,
    .exitAction = NULL,
};

// static struct state idleState = {
//     .parentState = &menuGroupState,
//     .entryState = NULL,
//     .transitions = (struct transition[]){
//         { Event_button, (void *)(intptr_t)'h', &compareKeyboardChar, NULL,
//             &hState },
//     },
//     .numTransitions = 1,
//     .data = "idle",
//     .entryAction = &printEnterMsg,
//     .exitAction = &printExitMsg,
// };

void setup(){
}

void loop(){
    HAL_GPIO_TogglePin(LD0_GPIO_Port, LD0_Pin);
    HAL_Delay(100);
}
