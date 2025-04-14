/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include "lvgl.h"
#include "global_data.h"
#include "mpp/MppMdl.h"
#ifndef WIN32
#include <sys/time.h>
#endif

/*********************
 *      DEFINES
 *********************/
#define CHECK_RET(express,name) \
		do{ \
			XM_S32 Ret; \
			Ret = express; \
			if (XM_SUCCESS != Ret) \
			{ \
				printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, Ret); \
				return; \
			} \
		}while(0)
			
/**********************
 *      TYPEDEFS
 **********************/
typedef struct SendStreamThreadParam{
	SIZE_S stImageSize;
	PIXEL_FORMAT_E enPixFormat;
	XM_CHAR cFileName[128];
	XM_S32 s32MilliSec;
	VO_LAYER VoLayer;
	XM_S32 s32ChnCnt;
}ThreadParam;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);


/**********************
 *      MACROS
 **********************/

lv_disp_drv_t* lv_port_disp_init()
{
	static lv_disp_draw_buf_t draw_buf_dsc_3;
    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
#ifdef LVGL_ROATE_MODE
	lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, NULL, MY_DISP_HOR_RES * MY_DISP_VER_RES);   /*Initialize the display buffer*/
#else
    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, MY_DISP_VER_RES * MY_DISP_HOR_RES);   /*Initialize the display buffer*/
#endif
    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/
    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

	disp_drv.physical_hor_res = MY_DISP_HOR_RES;
	disp_drv.physical_ver_res = MY_DISP_VER_RES;

	disp_drv.screen_transp    = LV_COLOR_SCREEN_TRANSP;
	disp_drv.dpi              = LV_DPI_DEF;
	disp_drv.color_chroma_key = LV_COLOR_CHROMA_KEY;
    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;
    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_3;
#ifdef LVGL_ROATE_MODE
	disp_drv.rotated = 0;
	disp_drv.sw_rotate = 0;
#else
	disp_drv.rotated = 3;
	disp_drv.sw_rotate = 1;
#endif
	if(disp_drv.rotated == 0){
	    disp_drv.hor_res = MY_DISP_VER_RES;
	    disp_drv.ver_res = MY_DISP_HOR_RES;

		disp_drv.physical_hor_res = MY_DISP_VER_RES;
		disp_drv.physical_ver_res = MY_DISP_HOR_RES;	
	}
    /*Required for Example 3)*/
	disp_drv.full_refresh = 0;
    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);

	return &disp_drv;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
uint8_t redValues[64] = {
	0x10, 0x70, 0x30, 0x50, 0x00, 0x80, 0x20, 0x60,
	0x70, 0x10, 0x50, 0x30, 0x80, 0x00, 0x60, 0x20,
	0x30, 0x50, 0x00, 0x80, 0x20, 0x60, 0x10, 0x70,
	0x50, 0x30, 0x80, 0x00, 0x60, 0x20, 0x70, 0x10,
	0x80, 0x00, 0x60, 0x20, 0x70, 0x10, 0x50, 0x30,
	0x00, 0x80, 0x20, 0x60, 0x10, 0x70, 0x30, 0x50,
	0x20, 0x60, 0x10, 0x70, 0x30, 0x50, 0x00, 0x80,
	0x60, 0x20, 0x70, 0x10, 0x50, 0x30, 0x80, 0x00,
};
uint8_t greenValues[64] = {
	0x10, 0x30, 0x20, 0x20, 0x30, 0x10, 0x20, 0x20,
	0x20, 0x20, 0x00, 0x40, 0x20, 0x20, 0x40, 0x00,
	0x30, 0x10, 0x20, 0x20, 0x10, 0x30, 0x20, 0x20,
	0x20, 0x20, 0x40, 0x00, 0x20, 0x20, 0x00, 0x40,
	0x10, 0x30, 0x20, 0x20, 0x00, 0x40, 0x20, 0x20,
	0x20, 0x20, 0x00, 0x40, 0x20, 0x20, 0x40, 0x00,
	0x30, 0x10, 0x20, 0x20, 0x10, 0x30, 0x20, 0x20,
	0x20, 0x20, 0x40, 0x00, 0x20, 0x20, 0x00, 0x40,
};
uint8_t blueValues[64] = {
	0x63, 0x4D, 0x8F, 0x42, 0x42, 0x37, 0x63, 0x16,
	0x2C, 0x58, 0x37, 0x63, 0x4D, 0x4D, 0x16, 0x6E,
	0xA5, 0x37, 0x58, 0x63, 0x4D, 0x16, 0x42, 0x63,
	0x16, 0xA5, 0x21, 0x79, 0x0B, 0x4D, 0x58, 0x84,
	0x6E, 0x4D, 0x58, 0x16, 0x6E, 0x37, 0x58, 0x42,
	0x37, 0x8F, 0x21, 0x9A, 0x58, 0x79, 0x63, 0xA5,
	0x79, 0x21, 0x58, 0x42, 0x6E, 0x42, 0x8F, 0x16,
	0x37, 0x6E, 0x2C, 0x42, 0x21, 0x58, 0x63, 0x8F,
};

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
	//因放关机画面的时候刷新屏幕会覆盖关机画面，所以停止刷新屏幕
	if (GlobalData::Instance()->stop_screen_refresh_) {
		lv_disp_flush_ready(disp_drv);
		return;
	}

    int32_t y;
	if(GlobalData::Instance()->g_pImgVirAttr==NULL||area->x2<0||area->y2<0||area->x1>disp_drv->hor_res-1||area->y1 >disp_drv->ver_res-1)
	{        
		lv_disp_flush_ready(disp_drv);
		return;    
	}
#ifdef LVGL_ROATE_MODE//旋转算法处理
	uint8_t* buffer = (uint8_t*)MppMdl::Instance()->SwapImgVirAttr();
	uint16_t w = lv_area_get_width(area);
	for (y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++) {
		uint8_t* ptr_dst = buffer + area->x1*(screen_height+120)*2 + ((screen_height+120)-1-y)*2;  //360分辨率，实际要按480计算
		uint8_t A, Red, GREEN, BLUE = 0;
		uint16_t rgb1555 = 0;
		for (int x = 0; x < w; x++) {
			// alpha=0表示该像素非图标，则A赋值1；否则像素为图标，则A赋值0
			if (color_p->ch.alpha == 0)
			{
				// int offsetX = x % 8;
				// int offsetY = y % 8;
				// color_p->ch.red = redValues[offsetY * 8 + offsetX];
				// color_p->ch.green = greenValues[offsetY * 8 + offsetX];
				// color_p->ch.blue = blueValues[offsetY * 8 + offsetX];
				A = 0;
			}
			else
			{
				A = 1;
			}

			Red = (color_p->ch.red & 0xf8) >> 3;
			GREEN = (color_p->ch.green & 0xf8) >> 3;
			BLUE = (color_p->ch.blue & 0xf8) >> 3;
			// if (Red == 0 && GREEN == 0 && BLUE == 0) {
			// 	Red = 1;
			// 	GREEN = 1;
			// 	BLUE = 1;
			// }
			rgb1555 = (A << 15) + (Red << 10) + (GREEN << 5) + BLUE;
			ptr_dst[0] = rgb1555 & 0xff;
			ptr_dst[1] = rgb1555 >> 8;
			ptr_dst += ((screen_height+120)<<1);   
			color_p++;
		}
	}
	MppMdl::Instance()->CommitSwapImg();
#else
	void* pImgVirAttr = MppMdl::Instance()->SwapImgVirAttr();
	uint8_t src[MY_DISP_VER_RES * 2];
	uint16_t w = lv_area_get_width(area);
	for (y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++) {
		uint8_t* ptr_src = src;
		for (int x = 0; x < w; x++) {
			int offsetX = x % 8;
			int offsetY = y % 8;
			uint8_t A, Red, GREEN, BLUE = 0;
			uint16_t rgb1555 = 0;
			// alpha=0表示该像素非图标，则A赋值1；否则像素为图标，则A赋值0
			if (color_p->ch.alpha == 0)
			{
				// color_p->ch.red = redValues[offsetY * 8 + offsetX];
				// color_p->ch.green = greenValues[offsetY * 8 + offsetX];
				// color_p->ch.blue = blueValues[offsetY * 8 + offsetX];
				A = 0;
			}
			else
			{
				A = 1;
			}

			Red = (color_p->ch.red & 0xf8) >> 3;
			GREEN = (color_p->ch.green & 0xf8) >> 3;
			BLUE = (color_p->ch.blue & 0xf8) >> 3;
			if (Red == 0 && GREEN == 0 && BLUE == 0)
			{
				Red = 1;
				GREEN = 1;
				BLUE = 1;
			}
			rgb1555 = (A << 15) + (Red << 10) + (GREEN << 5) + BLUE;
			ptr_src[0] = rgb1555 & 0xff;
			ptr_src[1] = rgb1555 >> 8;
			ptr_src += 2;
			color_p++;
		}
		uint8_t* ptr_dst = (uint8_t*)pImgVirAttr + y*disp_drv->hor_res*2 + area->x1*2;
		memcpy(ptr_dst, src, w*2);
	}
	MppMdl::Instance()->CommitSwapImg();
    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
 #endif
    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
