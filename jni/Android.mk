# =========================================================
#
# Android.mk
# AMCOP make file.
# Created by Wooram Hong 2013/02/06
# This file is part of the AMCOP project.
#
# =========================================================

# path
LOCAL_PATH := $(call my-dir)
BASE_PATH := ..
ENGINE_PATH := $(BASE_PATH)/engine

OGL_PATH := $(ENGINE_PATH)/ogl

# main project build
include $(CLEAR_VARS)

LOCAL_MODULE    	:= native-activity
LOCAL_DEFAULT_CPP_EXTENSION := cpp

#LOCAL_C_INCLUDES := \

#LOCAL_CFLAGS := \

LOCAL_SRC_FILES 	:= main.cpp \
$(foreach F, $(LOCAL_PATH), $(addprefix $(OGL_PATH)/,$(notdir $(wildcard $(F)/$(OGL_PATH)/*.cpp)))) \

LOCAL_LDLIBS   		:= -llog -landroid -lEGL -lGLESv1_CM
LOCAL_LDLIBS 		+= -lOpenSLES
LOCAL_STATIC_LIBRARIES := android_native_app_glue

LOCAL_CFLAGS += -DIS_ANDROID -DIS_ANDROID_PROJECT -DNO_USE_EXCEPTIONS

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
