LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libopus
LOCAL_SRC_FILES := libs/libopus.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libspeex
LOCAL_SRC_FILES := libs/libspeex.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
APP_DEBUG := false
APP_OPTIM := release
LOCAL_MODULE := samp
LOCAL_STATIC_LIBRARIES := libopus libspeex

LOCAL_C_INCLUDES := $(LOCAL_PATH)/vendor

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/RW/RenderWare.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/gui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/net/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/util/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/util/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/raknet/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/raknet/SAMP/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/raknet/src/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/raknet/src/crypto/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/inih/cpp/INIReader.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/inih/ini.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/imgui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/hash/md5.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/SimpleIni/*.c)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS := -llog  -lz -landroid -lOpenSLES
LOCAL_CPPFLAGS := -w -s -fvisibility=hidden -pthread -Wall -fpack-struct=1 -O2 -std=c++14 -fexceptions -ferror-limit=5
include $(BUILD_SHARED_LIBRARY)
