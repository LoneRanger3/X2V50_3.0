#include "lv_port_indev_template.h"
#include <linux/input.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

static char DEV_TOUCH[32] = {0};
#define DATA_SIZE  16

static int fd_touch = -1;
/*鼠标默认为PS2类型无滚轮，下面两种都含滚轮*/
static unsigned char ex_seq_id[6] = { 0xf3, 200, 0xf3, 100, 0xf3, 80 };
//static unsigned char ps_seq_id[6] = { 0xf3, 200, 0xf3, 100, 0xf3, 80 };

static MOUSE_DATA mouse_data[DATA_SIZE];
static int read_flag = 0, write_flag = 0;
static pthread_mutex_t mouse_lock = PTHREAD_MUTEX_INITIALIZER;
static int no_btnup_flag = 0;
int g_haveTouch = -1;

static int find_touch_input(char *path)
{
	char line[128];
	char flag;
	FILE *fp = fopen("/proc/bus/input/devices", "r");
	if(fp == NULL)
		return 0;
	while(fgets(line, 127, fp))
	{
		if(flag == 1 && strstr(line, "Handlers=event"))
		{
			strcpy(path, "/dev/input/");
			memcpy(path+11, line+12, 6);
			fclose(fp);
			return 1;
		}
		if(strstr(line, "Name=\"touch\""))
			flag = 1;
		if(strstr(line, "Bus=0000 Vendor=0000 Product=0000 Version=0000"))
			no_btnup_flag = 1;
	}
	fclose(fp);
	return 0;
}

int LibXmDvr_Input_getGDICallInterface(void)
{
	if(g_haveTouch < 0)
    	g_haveTouch = find_touch_input(DEV_TOUCH);
    return g_haveTouch;
}

static void write_data(MOUSE_DATA *data)
{
	pthread_mutex_lock(&mouse_lock);
	while(read_flag == ((write_flag + 1) % DATA_SIZE)) usleep(10*1000);
	memcpy(&mouse_data[write_flag], data, sizeof(MOUSE_DATA));
	write_flag = (write_flag + 1) % DATA_SIZE;
	pthread_mutex_unlock(&mouse_lock);
}

void *touch_thread(void *arg)
{
	struct input_event s;
	char abs_flag = 0, sync_flag = 0, touch_flag = 0;
	unsigned short last_ax = 0xffff, last_ay = 0xffff;
	MOUSE_DATA touch_data;

	fd_touch = open(DEV_TOUCH, O_RDONLY);
	if(fd_touch < 0)
	{
		perror("open touch error\n");
		printf("%s\n", DEV_TOUCH);
		return NULL;
	}

	memset(&touch_data, 0, sizeof(touch_data));
	while(1)
	{
		if (read(fd_touch, &s, sizeof(s)) > 0)
		{
			if((abs_flag == 0 && (s.type == 3 && s.code == 57)) || (s.type == 1 && s.code == 330 && s.value == 1))
			{
				abs_flag = 1;
				sync_flag = 0;
			}
			else if(s.type == 1 && s.code == 330 && s.value == 0)
			{
				abs_flag = 0;
				if(touch_flag == 1)
				{
					touch_data.key = 0x80;
					write_data(&touch_data);
					touch_flag = 0;
				}
			}
			else if(abs_flag > 0 && s.type == 3)
			{
				sync_flag = 0;
				if(s.code == 53)
				{
					touch_data.ax = s.value & 0xFFFF;
					abs_flag |= 0x2;
				}
				else if(s.code == 54)
				{
					touch_data.ay = s.value & 0xFFFF;
					abs_flag |= 0x4;
				}
				/*else if(abs_flag == 3 && s.code == 50 && s.value == 1)
				{
					abs_flag++;
				}*/
				if(abs_flag == 0x7 && !(touch_flag && touch_data.ax == last_ax && touch_data.ay == last_ay))
				{
					touch_data.key	= 0x81;
					write_data(&touch_data);
					last_ax = touch_data.ax;
					last_ay = touch_data.ay;
					touch_flag = 1;
				}
			}
			else if(s.type == 0)
			{
				if(no_btnup_flag)
				{
					abs_flag = 0;
					if(s.code == 2)
					{
						sync_flag++;
					}
					if(sync_flag == 2 && touch_flag == 1)
					{
						touch_data.key = 0x80;
						write_data(&touch_data);
						touch_flag = 0;
					}
				}
			}
			else
			{
				abs_flag = 0;
				sync_flag = 0;
			}
		}
		else
		{
			perror("read touch error\n");
		}
	}
	return NULL;
}

int LibXmDvr_Input_init(void)
{
	pthread_t tid;

	if(fd_touch < 0)
	{
		if(pthread_create(&tid, NULL, touch_thread, NULL))
		{
			printf("create touch thread failed\n");
			return -1;
		}
	}
	
	return 0;
}

int LibXmDvr_Input_getData(MOUSE_DATA *pData)
{
	if (fd_touch < 0)
	{
		return -1;
	}
	
	while(read_flag == write_flag) usleep(10*1000);
	memcpy(pData, &mouse_data[read_flag], sizeof(MOUSE_DATA));
	read_flag = (read_flag + 1) % DATA_SIZE;

	return 0;

} /* end MouseGetData */

int MouseGetData_NonBlock(MOUSE_DATA *pData)
{
	if (fd_touch < 0)
	{
		return -1;
	}
	
	if(read_flag == write_flag)
		return -1;
	memcpy(pData, &mouse_data[read_flag], sizeof(MOUSE_DATA));
	read_flag = (read_flag + 1) % DATA_SIZE;

	return 0;
}

int LibXmdvr_TouchScreen_enable(int enable)
{
#define TOUCHSCREEN_SUSPEND 0x22
#define TOUCHSCREEN_RESUME 0x33
	static int dev_touch = -1;
	if(dev_touch < 0)
	{
		dev_touch = open("/dev/touch", 0);
		if(dev_touch < 0)
		{
			perror("open /dev/touch error\n");
			return -1;
		}
	}
	if(enable == 1)
	{
		ioctl(dev_touch, TOUCHSCREEN_RESUME, NULL);
	}
	else
	{
		ioctl(dev_touch, TOUCHSCREEN_SUSPEND, NULL);
	}
	return 0;
}
