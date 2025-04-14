#!/bin/bash

CURRENT_DIR=$(cd $(dirname $0); pwd)
echo $CURRENT_DIR

#rm -rf $CURRENT_DIR/lvgl-8.2.0/build
#mkdir $CURRENT_DIR/lvgl-8.2.0/build
#cd $CURRENT_DIR/lvgl-8.2.0/build/
#cmake .. -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=arm -DARCH=board8520dv200
#make
#cp $CURRENT_DIR/lvgl-8.2.0/build/lib/liblvgl.a $CURRENT_DIR/lib/650V200

rm -rf $CURRENT_DIR/sample/build
mkdir $CURRENT_DIR/sample/build
cd $CURRENT_DIR/sample/build
#USE_TS=OFF调用libXM_MiddleWare_api_mp4.a，用mp4。USE_TS=ON调用libXM_MiddleWare_api.a，用ts流
cmake .. -DARCH=board650v200 -DCMAKE_BUILD_TYPE=Release -DUSE_TS=ON
make
if test -e $CURRENT_DIR/sample/build/car_demo
then
echo “car_demo file exist”
else
echo “car_demo file no exist”
exit
fi
cp $CURRENT_DIR/sample/build/car_demo $CURRENT_DIR/pack/usr/bin/car_demo


#cd $CURRENT_DIR/uboot/u-boot-2014.04/
#cat boards.cfg
#make xm650v200sd2_spinor_x27_config
#make xm650v200sd2_spinor_x2c3_config
#make xm650v200sd2_spinor_x27w_config
#make -j 4
#cp $CURRENT_DIR/uboot/u-boot-2014.04/u-boot.bin $CURRENT_DIR/pack/uboot/u-boot-x2c3.bin

cd $CURRENT_DIR/pack
make clean
make

#cp $CURRENT_DIR/pack/target/netall_XM650V200_ADR.bin $CURRENT_DIR/netall_XM650V200_ADR.bin
./make_uboot_update_file/merge_file ./make_uboot_update_file/netall_XM650V200_CAR_X2V50_3.bin ./target/update.img ./make_uboot_update_file/uboot_upgrade_480x640.yuv ./target/netall_XM650V200_CAR_X2V50_3.bin
./make_uboot_update_file/merge_file ./make_uboot_update_file/update.img ./target/update.img ./make_uboot_update_file/uboot_upgrade_key_480x640.yuv