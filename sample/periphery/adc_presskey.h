#ifndef ADC_PRESSKEY_H_
#define ADC_PRESSKEY_H_
#include "xm_middleware_def.h"

typedef enum {
	KEY_UP=1,
	KEY_MODE,
	KEY_DOWN,
	KEY_MENU,
	KEY_NONE,
}KEY_VALUE_E;


#define count_max 3


void key_init();
void PressKeyQuery();
int getKeyValue();
void SetKeyValue(int key);
int shutDown(ShutDownMode shutdownmode);
int UsbAccGsensorCheckShutDown(ShutDownMode ShutType);

#endif//end ADC_PRESSKEY_H_