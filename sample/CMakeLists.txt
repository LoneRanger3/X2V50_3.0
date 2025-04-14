cmake_minimum_required(VERSION 3.10)
project(car_demo)

set(SAMPLE_SOURCE_ROOT ${CMAKE_SOURCE_DIR})

if(CMAKE_TOOLCHAIN_FILE)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Installation Directory")
endif()

#platform
#ARCH pc/board8521d/board85xx/board53/board55/board65

### Compiler ###
if(${ARCH} STREQUAL "board8521d")
	set(CMAKE_C_COMPILER arm-xmv2-linux-gcc)
	set(CMAKE_CXX_COMPILER arm-xmv2-linux-g++)
	set(CHIP 8521D)
	set(Lib_ROOT lib/${CHIP})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARCHBOARD -DBOARD8521")
elseif(${ARCH} STREQUAL "board8520dv200")
	set(CMAKE_C_COMPILER arm-xm-linux-gcc)
	set(CMAKE_CXX_COMPILER arm-xm-linux-g++)
	set(CHIP 8520DV200)
	set(Lib_ROOT lib/${CHIP})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARCHBOARD -DBOARD8520DV200")
elseif(${ARCH} STREQUAL "board650v200")
	set(CMAKE_C_COMPILER arm-xm-linux-gcc)
	set(CMAKE_CXX_COMPILER arm-xm-linux-g++)
	set(CHIP 650V200)
	set(Lib_ROOT lib/${CHIP})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARCHBOARD -DBOARD650V200 -DSUPPORT_4K")
elseif(${ARCH} STREQUAL "pc")
	set(Lib_ROOT lib_linux)
endif()

### debug log ###
message("=== source root=${SAMPLE_SOURCE_ROOT}")
message("=== c compiler=${CMAKE_C_COMPILER}")
message("=== cxx compiler=${CMAKE_CXX_COMPILER}")
message("=== arch=${ARCH}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=neon -mno-unaligned-access -fno-aggressive-loop-optimizations -flax-vector-conversions -fsigned-char -fopenmp -g -lstdc++ -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -std=gnu99")

set(CMAKE_C_FLAGS_RELEASE "-s")
set(CMAKE_CXX_FLAGS_RELEASE "-s")

include_directories(${SAMPLE_SOURCE_ROOT}/GUI/porting)
include_directories(${SAMPLE_SOURCE_ROOT}/GUI)
include_directories(${SAMPLE_SOURCE_ROOT}/cJson)
include_directories(${SAMPLE_SOURCE_ROOT})
include_directories(${SAMPLE_SOURCE_ROOT}/../include)
include_directories(${SAMPLE_SOURCE_ROOT}/../lvgl-8.2.0)
include_directories(${SAMPLE_SOURCE_ROOT}/../lvgl-8.2.0/src)
include_directories(${SAMPLE_SOURCE_ROOT}/../include/${CHIP}/mpp)
include_directories(${SAMPLE_SOURCE_ROOT}/../include/${CHIP}/smart)
include_directories(${SAMPLE_SOURCE_ROOT}/../include/${CHIP}/usb)
include_directories(${SAMPLE_SOURCE_ROOT}/../include/${CHIP}/fvideo)
include_directories(${SAMPLE_SOURCE_ROOT}/../include/${CHIP}/isp)
include_directories(${SAMPLE_SOURCE_ROOT}/../include/${CHIP}/comm)

file(GLOB SAMPLE_SOURCE_FILES "*.cpp" "GUI/*.cpp" "cJson/*.cpp" "GUI/porting/*.cpp" "periphery/*.cpp" "log/*.cpp" "mpp/*.cpp" "network/*.cpp")

message("=== sample files=${SAMPLE_SOURCE_FILES}")

link_directories(${SAMPLE_SOURCE_ROOT}/../${Lib_ROOT})
link_directories(${SAMPLE_SOURCE_ROOT}/../${Lib_ROOT}/isp_401)
add_executable (car_demo ${SAMPLE_SOURCE_FILES})

if(USE_TS)
    set(Lib_NAME XM_MiddleWare_api)
else()
    set(Lib_NAME XM_MiddleWare_api_mp4)
endif()
if(${ARCH} STREQUAL "board8521d")
target_link_libraries (car_demo 
    ${Lib_NAME} 
    xmia 
    rtc 
    xmia_kwr 
    xmia_vad 
    fftw3f 
    fdr
    fr
    facnn
    fpcnn
    xmia_fd_cnn
    xmia_adas 
    xmia_td 
    xmia_spc
    xmia_comm
    npu
    ive
    mpi 
    venc
    vdec 
    h265e 
    h265d 
    audio 
    fvideo 
    dvr 
    xmsdk
	avformat 
	avcodec 
	swscale 
	swresample 
	avutil
    json 
    pthread 
    m 
	z
    lvgl)
elseif(${ARCH} STREQUAL "board8520dv200")
target_link_libraries (car_demo 
    ${Lib_NAME}  
	xmaudio3a
    xmia 
    rtc 
    xmia_kwr 
    xmia_vad 
    fftw3f 
    fdr
    fr
    facnn
    fpcnn
    xmia_fd_cnn
    xmia_adas 
    xmia_td 
    xmia_spc
    xmia_comm
    npu
    dpu
    smpi
    ive
    mpi 
    venc
    vdec 
    h265e 
    h265d 
    audio 
    fvideo 
    dvr 
    xmsdk
    xmjpege
	avformat 
	avcodec 
	swscale 
	swresample 
	avutil
    json 
    pthread 
    m 
	z
    lvgl)
elseif(${ARCH} STREQUAL "board650v200")
target_link_libraries (car_demo 
    ${Lib_NAME} 
    xmia 
    xmia_kwr
    fftw3f 
    fr
    xmia_comm
    xmrtsp
    npu
    dpu
    ad
    ispapp
    sns_XAx
    phyvi
    autoscene_fv
    fvideo 
    minIni
    sdnc
    isp
    ae 
    awb
    mpi_all 
    rtc 
    venc
    vdec
    h265e 
    h265d 
    x264_weak
    jpegenc
    jpegdec
    xmaudio3a
    xmsdk
	zzip
    avformat 
	avcodec 
	swscale 
	swresample 
	avutil
    json 
    ive
    iconv
    jpeg
    pthread 
    m 
	z
	cr
    lvgl)
endif()