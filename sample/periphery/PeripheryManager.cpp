#include "PeripheryManager.h"
#include "Log.h"
#ifndef WIN32
#include "PeripheryThread.h"
#include "car_state_detect.h"
#include "power.h"
#include "setgpio.h"
#endif

PeripheryManager* PeripheryManager::instance_ = 0;
PeripheryManager* PeripheryManager::Instance()
{
    if (0 == instance_) {
		instance_ = new PeripheryManager();
	}
	return instance_;
}

void PeripheryManager::Uninstance()
{
    if (0 != instance_) {
		delete instance_;
		instance_ = 0;
	}
}

PeripheryManager::PeripheryManager(/* args */)
{
}

PeripheryManager::~PeripheryManager()
{
}

int PeripheryManager::Periphery()
{
#ifndef WIN32
	PeripheryThreadStart();
#endif
    return 0;
}

int PeripheryManager::GsensorCheck()
{
	int ret = 0;
#ifndef WIN32
    ret = CarCollisionDetectPowerOn();
#endif
	return ret;
}

int PeripheryManager::SensitivitySet(int degree)
{
	int ret = 0;
#ifndef WIN32
	ret = SensitivityDegreeSet(degree);
#endif
	return ret;
}

int PeripheryManager::CarStopSensitivitySet(int degree)
{
	int ret = 0;
#ifndef WIN32
	ret = CarStopSensitivityDegreeSet(degree);
#endif
	return ret;
}

int PeripheryManager::CameraLedControl(bool led_state)
{
	int ret = 0;
#ifndef WIN32
	ret = RecordCameraLedControl(led_state);
#endif
	return ret;
}

int PeripheryManager::GetAdcBattery()
{
	int ret = 0;
#ifndef WIN32
	ret = adc_battery();
#endif
	return ret;
}

int PeripheryManager::GsensorModeSelect(int mode)
{
	int ret = 0;
#ifndef WIN32
	ret = GsensorMode(mode);
#endif
	return ret;
}

int PeripheryManager::GetKeyValue()
{
	int ret = 0;
#ifndef WIN32
	ret = getKeyValue();
#endif
	return ret;
}

int PeripheryManager::SetLight(int value)
{
	int ret = 0;
#ifndef WIN32
	ret = SetLightValue(value);
#endif
	return ret;
}

int PeripheryManager::Shutdown(ShutDownMode shutdownmode)
{
	int ret = 0;
#ifndef WIN32
	ret = shutDown(shutdownmode);
#endif
	return ret;
}

int PeripheryManager::SpeakerEnable(bool enable)
{
#if 0
#ifndef WIN32	
	unsigned int gpio_addr = GPIO_BASE + 168*0x4;
	if(enable) {
		XMLogW("ao_en enable");
		static bool init = false;
		if (!init) {
			if(reg_ctrl_.WriteL(GPIO_OUT_EN | GPIO_OUT_HIGH, gpio_addr)) {
				XMLogE("Enabel AO_EN failed");
				return -1;
			}
			init = true;
		}
		reg_ctrl_.WriteL(0x10, 0x100680ac);
		reg_ctrl_.WriteL(0x1b, 0x100680ac);
	}
	else {
		XMLogW("ao_en disable");
		reg_ctrl_.WriteL(0x10, 0x100680ac);
		reg_ctrl_.WriteL(0x0, 0x100680ac);
	}
#endif
#endif
	return 0;
}

int PeripheryManager::DecodeEnable(int enable)
{
	int ret = 0;
#ifndef WIN32
	ret = reg_ctrl_.VidecDecCtrl(enable);
#endif
	return ret;
}