#include <string>
#include "osd_user.h"
#include "xm_middleware_api.h"
#include "xm_middleware_network.h"
#include "xm_ia_td_interface.h"
#include "Log.h"
#include "lvgl.h"
#include "global_data.h"
#include "mpp/MppMdl.h"
#include "DemoDef.h"
#define OSDWidth	240//前路水印图片的宽度
#define OSDHeight	64//前路水印图片的高度

#define R_Width     1920
#define R_Heigth    1080

int osd_data_init(void)
{
	XM_MW_OSD_INFOS osd_infos;

    bool enable=false;
	int i = 0;
	int width_v = 0;
	int height_v = 0;
	int j = 0;
	int move = 0; //偏移
	int osdx = 0; //logo水印x
	int osdy = 0;  //logo水印y
	int osd_x = 0;//时间水印x
	int osd_y = 0;//时间水印y
	 XM_CONFIG_VALUE cfg_value;
	int ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Resolution, cfg_value);
	if (ret >= 0) {
       if(cfg_value.int_value==1080){
          width_v=1920;
		  height_v=1080;
		  move=20;
	   }else if(cfg_value.int_value==1440){
          width_v=2560;
		  height_v=1440;
		  move=18;
	   }else if(cfg_value.int_value==k3KHeight || cfg_value.int_value==k4KHeight){
        //   if(!MppMdl::Instance()->AdLoss()){
        //    width_v=2560;
		//    height_v=1440;
		//    move=18;
		//   }else{
			if(cfg_value.int_value==k3KHeight){
			 width_v=3008;
		     height_v=k3KHeight;
			 move=16;
			}else{
			 width_v=3840;
		     height_v=k4KHeight-1024; 
			 move=16;
			}
		//   }
	   }
	}else{
      width_v=2560;
	  height_v=1440;
	}

	  osd_x=128;//2048;  
	  osd_y=8192*(height_v-60-(height_v/360)*8)/height_v;


	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Date_Watermark, cfg_value);
	enable=cfg_value.bool_value;

  // MppMdl::Instance()->EnableOsdTime(0,false, osd_x, osd_y);
   MppMdl::Instance()->EnableOsdTime(0,enable, osd_x, osd_y); //前摄时间水印


   MppMdl::Instance()->EnableOsdTime(1,enable, osd_x, 8192*(R_Heigth-60-(R_Heigth/360)*8)/R_Heigth);

   MppMdl::Instance()->EnableOsdTime(4,enable, osd_x, 8192*(kSubStreamHeight-60-(kSubStreamHeight/360)*8)/kSubStreamHeight); //APP时间水印

#if 0 //logo水印
      osdx = 128;
	  osdy = 8192*(height_v-60-(height_v/360)*26)/height_v;	
	for (i = 0; i < 3; i++) {//固定的LOGO图片水印，osd_rec0.rgb、osd_rec1.rgb、osd_rec2.rgb
		if( i == 0){//前摄主码流

		if(height_v==1440){
			osd_infos.osd[i][j].x = osdx;  //超始位置横坐标，以8192为基准
			osd_infos.osd[i][j].y = 8192*(height_v-60-(height_v/360)*38)/height_v;// 128+64;//8192/128*116;  //超始位置纵坐标，以8192为基准				
		}else if(height_v==1080){
			osd_infos.osd[i][j].x = osdx;  //超始位置横坐标，以8192为基准
			osd_infos.osd[i][j].y = 8192*(height_v-60-(height_v/360)*48)/height_v;// 128+64;//8192/128*116;  //超始位置纵坐标，以8192为基准			
		}else{
			osd_infos.osd[i][j].x = osdx;   //超始位置横坐标，以8192为基准
			osd_infos.osd[i][j].y = osdy;	// 128+64;//8192/128*116;  //超始位置纵坐标，以8192为基准				
		}
			osd_infos.osd[i][j].width =OSDWidth; //不能超过XM_MAX_OSD_WIDTH
			osd_infos.osd[i][j].height =OSDHeight; //不超过XM_MAX_OSD_HEIGHT
			osd_infos.osd[i][j].up = true; //是否放大，true放大1倍
			osd_infos.osd[i][j].enable=enable; //使能

		}else if(i == 1){//后摄主码流
			osd_infos.osd[i][j].x = osdx;  //超始位置横坐标，以8192为基准
			osd_infos.osd[i][j].y = 8192*(height_v-60-(height_v/360)*48)/height_v;	//8192*(R_Heigth-60-(R_Heigth/360)*20)/R_Heigth;   //128+64;//8192/128*116;  //超始位置纵坐标，以8192为基准
			osd_infos.osd[i][j].width =OSDWidth; //不能超过XM_MAX_OSD_WIDTH
			osd_infos.osd[i][j].height = OSDHeight; //不超过XM_MAX_OSD_HEIGHT
			osd_infos.osd[i][j].up = true; //是否放大，true放大1倍
			osd_infos.osd[i][j].enable=enable; //使能

		}else if(i == 2){//前摄子码流，APP直播流
			osd_infos.osd[i][j].x = 128;  //超始位置横坐标，以8192为基准
			osd_infos.osd[i][j].y = 8192*(kSubStreamHeight-130)/kSubStreamHeight;//8192/128*100;  //超始位置纵坐标，以8192为基准
			osd_infos.osd[i][j].width =OSDWidth; //不能超过XM_MAX_OSD_WIDTH
			osd_infos.osd[i][j].height = OSDHeight; //不超过XM_MAX_OSD_HEIGHT
			osd_infos.osd[i][j].up = false; //是否放大，true放大1倍
			osd_infos.osd[i][j].enable=enable; //使能
		}			
		FILE *file=NULL;
		char str[64]={0};
	#if 0	
		memset(str,0,64);
		sprintf(str,"%s","/mnt/tfcard/logo_w.rgb");
		file = fopen(str,"rb");
		if(file){
			//XMLogI("======%d:have %s file",i,str);
			fseek(file,0,SEEK_SET);
			fread(osd_infos.osd[i][j].osd_buf,  1, osd_infos.osd[i][j].width*osd_infos.osd[i][j].height*2, file);
			fclose(file);
			file = NULL;
		}else
	#endif
		{
			if(i==0){//前摄主码流
				memset(str,0,64);
				sprintf(str,"%s","/mnt/custom/Pic/X2_Rear.rgb");					
				file = fopen(str,"rb");
				if(file){
					XMLogI("======%d:have %s file",i,str);
					fseek(file,0,SEEK_SET);
					fread(osd_infos.osd[i][j].osd_buf, 1, osd_infos.osd[i][j].width*osd_infos.osd[i][j].height*2, file);
					fclose(file);
					file = NULL;
				}else{					
					XMLogW("======%d:no have %s file",i,str);
				}
			}else if(i==1){//后摄主码流
				memset(str,0,64);
				sprintf(str,"%s","/mnt/custom/Pic/X2_F.rgb");					
				file = fopen(str,"rb");
				if(file){
					XMLogI("======%d:have %s file",i,str);
					fseek(file,0,SEEK_SET);
					fread(osd_infos.osd[i][j].osd_buf, 1, osd_infos.osd[i][j].width*osd_infos.osd[i][j].height*2, file);
					fclose(file);
					file = NULL;
				}else{					
					XMLogW("======%d:no have %s file",i,str);
				}
			}else if(i==2){//前摄子码流，APP直播流
				memset(str,0,64);
				sprintf(str,"%s","/mnt/custom/Pic/X2_Rear.rgb");					
				file = fopen(str,"rb");
				if(file){
					XMLogI("======%d:have %s file",i,str);
					fseek(file,0,SEEK_SET);
					fread(osd_infos.osd[i][j].osd_buf, 1, osd_infos.osd[i][j].width*osd_infos.osd[i][j].height*2, file);
					fclose(file);
					file = NULL;
				}else{					
					XMLogW("======%d:no have %s file",i,str);
				}
			}

		}
	}
	j = 1;
	for (i = 0; i < 3; i++) {//扩展可变化水印
		memset(osd_infos.osd[i][j].osd_buf,0,XM_MAX_OSD_WIDTH*XM_MAX_OSD_HEIGHT*2);
			osd_infos.osd[i][j].x = 0;  //超始位置横坐标，以8192为基准
			osd_infos.osd[i][j].y = 8192/2;  //超始位置纵坐标，以8192为基准
			osd_infos.osd[i][j].width = OSDWidth; //不能超过XM_MAX_OSD_WIDTH
			osd_infos.osd[i][j].height = OSDHeight; //不超过XM_MAX_OSD_HEIGHT
			osd_infos.osd[i][j].up = false; //是否放大，true放大1倍
			osd_infos.osd[i][j].enable=false; //失能
			
	}
	MppMdl::Instance()->SetOSDInfos(osd_infos);
#endif	
}
