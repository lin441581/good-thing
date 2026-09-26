#ifndef __KEY_H
#define __KEY_H
#include "main.h"

typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_NEXT = 1,      // PA6 短按
    KEY_EVENT_OK = 2,        // PA4 短按
    KEY_EVENT_OK_LONG = 3,   // PA4 长按
    KEY_EVENT_NEXT_LONG = 4  // PA6 长按
} KeyEvent_t;

void Key_Init(void);
void Key_Tick10ms(void);
KeyEvent_t Key_GetEvent(void);
#endif 
