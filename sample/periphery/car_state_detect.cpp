#include "car_state_detect.h"
#include "i2c.h"
#include "PeripheryThread.h"
#include "Log.h"
#include "io.h"
#include "xm_middleware_api.h"

#define TEST 0
#define GSENSOR_3 1   //da380-3
#define GSENSOR_2 2    //da380-2

enum XM_GSENSOR_MODE
{
	NORMAL_MODE,
	LOW_POWER_MODE,
	SUSPENDE_MODE
};
enum {
    G_SENSOR_CLOSE = 0,
    G_SENSOR_LOW,
    G_SENSOR_MEDIUM,
    G_SENSOR_HIGH,
    G_SENSOR_SCAN,
    G_SENSOR_LOW_POWER_MODE,
};
int GsensorCompatible();
void InitSensor();
//关机:停车监控模式
void ShutMode()
{
	GsensorCompatible();
   da380_i2c_write(DEVICE_ADDR, MODE_BW, 0X5E);//选择低功耗模式
}

//开机:重力感应模式
void NormalMode()
{
	if(GsensorCompatible()==GSENSOR_3){
	 da380_i2c_write(DEVICE_ADDR, MODE_BW, 0x1E);//选择正常模式
	}else{
	 da380_i2c_write(DEVICE_ADDR, MODE_BW, 0x5E);//选择正常模式	
	}
}

//停车监控和重力感应都关闭，选择悬挂模式
void SuspendMode()
{
	da380_i2c_write(DEVICE_ADDR, MODE_BW, 0x9E);//选择悬挂模式
}

int GsensorMode(int mode)
{
	if (mode == NORMAL_MODE)
	{
		NormalMode();
	}
	else if (mode == LOW_POWER_MODE)
	{
		ShutMode();
	}
	else if (mode == SUSPENDE_MODE)
	{
		SuspendMode();
	}
	return 0;
}
int GsensorCompatible()
{
	static unsigned char init_flag=0;
	unsigned char device_address = da380_i2c_read(DEVICE_ADDR, CHIPID);
	if (device_address == DEVICE_DEFAULT_ADDRESS){
		if(init_flag==0){
         init_flag=1;
		 InitSensor();
		 XMLogI("G-sensor init ");
		}
		XMLogI("G-sensor init success");
	}else
	{
		XMLogI("G-sensor init fail");
		return -1;
	}
	unsigned char distingush_register = da380_i2c_read(DEVICE_ADDR, 0xC0);
	if ((distingush_register & 0x38) >> 3 == 0x03) {
		XMLogI("gsensor-3 0x03\n");
		return GSENSOR_3;
	}
	else if ((distingush_register & 0x38) >> 3 == 0x01) {
		XMLogI("gsensor-2 0x01\n");
		return GSENSOR_2;
	}
	return 0;
}
void da380_resolution_range(unsigned char range)
{
    switch (range) {
    case G_SENSOR_HIGH:
        da380_i2c_write(DEVICE_ADDR,RESOLUTION_RANGE, 0x0D);//14bit +/-2g     对应分辨率4096 LSB/g   高//0x0C
		 da380_i2c_write(DEVICE_ADDR,ACTIVE_THS,0x8F);
		//  da380_i2c_write(DEVICE_ADDR, TAP_THS, 0x0F);//点击中断阈值配置 19
		//  da380_i2c_write(DEVICE_ADDR, INT_SET1, 0x30);//中断使能
        break;
    case G_SENSOR_MEDIUM:
         da380_i2c_write(DEVICE_ADDR,RESOLUTION_RANGE, 0x0E);//14bit +/-4g     对应分辨率2048 LSB/g   中0x0D
		 da380_i2c_write(DEVICE_ADDR,ACTIVE_THS, 0x8F);
		// da380_i2c_write(DEVICE_ADDR, TAP_THS, 0x19);//点击中断阈值配置 19
        break;
    case G_SENSOR_LOW:
         da380_i2c_write(DEVICE_ADDR,RESOLUTION_RANGE, 0x0F);//14bit +/-16g    对应分辨率512 LSB/g0x0F
		 da380_i2c_write(DEVICE_ADDR,ACTIVE_THS, 0x8F);
		// da380_i2c_write(DEVICE_ADDR, TAP_THS, 0x1F);//点击中断阈值配置 19
        break;
    default :
         da380_i2c_write(DEVICE_ADDR,RESOLUTION_RANGE, 0x0F);//14bit +/-2g     对应分辨率4096 LSB/g   高
		 da380_i2c_write(DEVICE_ADDR,ACTIVE_THS, 0xAF);
		// da380_i2c_write(DEVICE_ADDR, TAP_THS, 0x1F);//点击中断阈值配置 19
        break;
    }
}

void InitSensor() {
    da380_i2c_write(DEVICE_ADDR,RESET_DA380, 0x24);
    da380_i2c_write(DEVICE_ADDR,ODR_AXIS,     0x06);//enable X/Y/Z axis,1000Hz
    da380_i2c_write(DEVICE_ADDR,SWAP_POLARITY, 0x00); //remain the polarity of X/Y/Z-axis

    /*INT中断配置， 分别有三种使用方式使能z轴，使能z和y轴，或者使能z,y和x，这三种方式*/
    da380_i2c_write(DEVICE_ADDR,INT_SET1,     0x87);//disable orient interrupt.enable the active interrupt for the  z, y and x,axis  //27
    da380_i2c_write(DEVICE_ADDR,INT_SET2,     0x00);//disable the new data interrupt and the freefall interupt
    da380_i2c_write(DEVICE_ADDR,INT_MAP1,     0x04);
    da380_i2c_write(DEVICE_ADDR,INT_MAP2,     0x00);//doesn't mappint new data interrupt to INT
    da380_i2c_write(DEVICE_ADDR,INT_CONFIG,   0x01);//push-pull output for INT ,selects active level high for pin INT
    da380_i2c_write(DEVICE_ADDR,INT_LTACH,    0x03);//0x0E///Burgess_151210
    da380_i2c_write(DEVICE_ADDR,FREEFALL_DUR, 0x09);//freefall duration time = (freefall_dur + 1)*2ms
    da380_i2c_write(DEVICE_ADDR,FREEFALL_THS, 0x30);//default is 375mg
    da380_i2c_write(DEVICE_ADDR,FREEFALL_HYST, 0x01);
    da380_i2c_write(DEVICE_ADDR,ACTIVE_DUR,   0x02);//Active duration time = (active_dur + 1) ms 0X11 //影响重力灵敏度
    da380_i2c_write(DEVICE_ADDR,ACTIVE_THS,   0x8F);
    da380_i2c_write(DEVICE_ADDR,TAP_DUR,      0x04);//
    da380_i2c_write(DEVICE_ADDR,TAP_THS,      0x0a);
    da380_i2c_write(DEVICE_ADDR,ORIENT_HYST,  0x18);
    da380_i2c_write(DEVICE_ADDR,Z_BLOCK,      0x08);
    da380_i2c_write(DEVICE_ADDR,SELF_TEST,    0x00);//close self_test
    da380_i2c_write(DEVICE_ADDR,CUSTOM_OFF_X, 0x00);
    da380_i2c_write(DEVICE_ADDR,CUSTOM_OFF_Y, 0x00);
    da380_i2c_write(DEVICE_ADDR,CUSTOM_OFF_Z, 0x00);
    da380_i2c_write(DEVICE_ADDR,CUSTOM_FLAG,  0x00);
    da380_i2c_write(DEVICE_ADDR,CUSTOM_CODE,  0X00);
    da380_i2c_write(DEVICE_ADDR,Z_ROT_HODE_TM, 0x09);
    da380_i2c_write(DEVICE_ADDR,Z_ROT_DUR,    0xFF);
    da380_i2c_write(DEVICE_ADDR,ROT_TH_H,     0x45);
    da380_i2c_write(DEVICE_ADDR,ROT_TH_L,     0x35);
 #if 1//关闭IIC的内部上拉 :SA0 cn GNG
    unsigned char _da380_ = 0x00;
    da380_i2c_write(DEVICE_ADDR,0x7f, 0x83);
    da380_i2c_write(DEVICE_ADDR,0x7f, 0x69);
    da380_i2c_write(DEVICE_ADDR,0x7f, 0xbd);
	_da380_ = da380_i2c_read(DEVICE_ADDR, 0x8f); 
	_da380_ = _da380_&0xFD;
	da380_i2c_write(DEVICE_ADDR,0x8f, _da380_);
#endif
}


void InitSensorHighSensitivity() {
	GsensorMode(NORMAL_MODE);
	da380_resolution_range(G_SENSOR_HIGH);
}

void InitSensorMediumSensitivity() {
	GsensorMode(NORMAL_MODE);
	da380_resolution_range(G_SENSOR_MEDIUM);
}

void InitSensorLowSensitivity() {
	GsensorMode(NORMAL_MODE);
	da380_resolution_range(G_SENSOR_LOW);
}

void InitSensorShutDownSensitivity() {
	GsensorMode(SUSPENDE_MODE);
}

void CarStopSensorHighSensitivity() {
	GsensorMode(LOW_POWER_MODE);
	da380_resolution_range(G_SENSOR_HIGH);
}

void CarStopSensorMediumSensitivity() {
	GsensorMode(LOW_POWER_MODE);
	da380_resolution_range(G_SENSOR_MEDIUM);
}

void CarStopSensorLowSensitivity() {
	GsensorMode(LOW_POWER_MODE);
	da380_resolution_range(G_SENSOR_LOW);
}

void CarStopSensorShutDownSensitivity() {
	GsensorMode(SUSPENDE_MODE);
}

int CarCollisionDetectPowerOn() {
	//碰撞检测开机
	//static unsigned int Int_gsensor;
	//readl(DA380_GSENSOR_INT1, &Int_gsensor);
	//if (Int_gsensor == HIGH_POWER)
	//	XMLogI("INT1_gsensor=%#X", Int_gsensor);

	static unsigned int interrupt_flag = 0;
	static  bool collision_status = false;
	readl(GSENSOR_INT1_REG, &interrupt_flag);
	if (interrupt_flag!= GSENSOR_INT1_SIGNAL) {
		collision_status = false;
	}
	if (interrupt_flag == GSENSOR_INT1_SIGNAL && !collision_status) {
		XMLogI("Collision make  power  open");
		collision_status = true;
		writel(GSENSOR_INT1_REG, 0x1000);
		return 1;
	}

	XMLogI("Power on  detect no Collision ");
	return 0;
}

int CarCollisionDetect() {

	static unsigned int Int_gsensor;
    readl(DA380_GSENSOR_INT1, &Int_gsensor);
  /*  if (Int_gsensor == HIGH_POWER)
	XMLogI("************INT1_gsensor=%#X", Int_gsensor);*/

	static unsigned char interrupt_signal = 0;
	static  bool collision_flag = false;
	interrupt_signal = da380_i2c_read(DEVICE_ADDR, MOTION_FLAG);
	/*if(interrupt_signal)
	XMLogI("###########Crash  signal=%d", interrupt_signal);*/
	if (interrupt_signal == 0) {
		collision_flag = false;
	}
	if (interrupt_signal != 0 && !collision_flag) {
		XMLogI("Crash  signal");
		collision_flag = true;
		XM_Middleware_Periphery_Notify(XM_EVENT_COLLISION_DETECTED, "", 0);
	}
	return 0;
}

int SensitivityDegreeSet(int degree) {
	XMLogI("gsensor degree=%d", degree);
	switch (degree)
	{
	case 0:InitSensorShutDownSensitivity(); break;
	case 1:InitSensorHighSensitivity(); break;
	case 2:InitSensorMediumSensitivity(); break;
	case 3:InitSensorLowSensitivity(); break;
	case 4: CarStopSensorMediumSensitivity(); break;
	default:XMLogE("Set gsensor level error");
		break;
	}
	return 1;
}

int CarStopSensitivityDegreeSet(int degree) {
	switch (degree)
	{
	case 0:CarStopSensorShutDownSensitivity(); break;
	case 1:CarStopSensorHighSensitivity(); break;
	case 2:CarStopSensorMediumSensitivity(); break;
	case 3:CarStopSensorLowSensitivity(); break;
	default:XMLogE("Set car stop gsensor level error");
		break;
	}
	return 1;
}

int CarBackDetect() {
	static unsigned int car_back_signal = 0;
	static bool car_back_state = false;
	readl(ACR_BACK_DETECT, &car_back_signal);
	//XMLogI("car_back_signal=%#x", car_back_signal);
	if (car_back_signal == HIGH_POWER && car_back_state) {
		usleep(2000*10);
		readl(ACR_BACK_DETECT, &car_back_signal);
		if (car_back_signal == HIGH_POWER && car_back_state) {
			car_back_state = false;
			XMLogI("Car stop backing");
			XM_Middleware_Periphery_Notify(XM_EVENT_REVERSE_END, "", 0);
		}
	}
	if (car_back_signal == LOW_POWER&&!car_back_state) {
		usleep(2000*10);
		readl(ACR_BACK_DETECT, &car_back_signal);
		if (car_back_signal == LOW_POWER && !car_back_state) {
			car_back_state = true;
			XMLogI("Car start backing");
			XM_Middleware_Periphery_Notify(XM_EVENT_REVERSE_DETECTED, "", 0);
		}
	}
	return 0;
}

void CarStateDetect() {
	//CarStopDetect();
	CarCollisionDetect();
	CarBackDetect();
}