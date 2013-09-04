LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    	:= native-activity
LOCAL_DEFAULT_CPP_EXTENSION := cpp

LOCAL_SRC_FILES 	:= main.cpp
LOCAL_SRC_FILES 	+= ../engine/ogl/oglFunctions.cpp 
LOCAL_LDLIBS    		:= -llog -landroid -lEGL -lGLESv1_CM
LOCAL_LDLIBS 		+= -lOpenSLES
LOCAL_STATIC_LIBRARIES := android_native_app_glue

LOCAL_CFLAGS += -DIS_ANDROID

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
