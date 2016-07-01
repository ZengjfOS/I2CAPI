LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := I2CAPI
LOCAL_SRC_FILES := I2CAPI.c i2c_data.c
LOCAL_LDLIBS    := -llog

include $(BUILD_SHARED_LIBRARY)
