LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := Handworks
LOCAL_SRC_FILES := Handworks.cpp

LOCAL_LDLIBS	:=-L$(SYSROOT)/usr/lib -llog

include $(BUILD_SHARED_LIBRARY)
