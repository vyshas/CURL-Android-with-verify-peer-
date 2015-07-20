LOCAL_PATH := $(call my-dir)

#cURL prebuilt
include $(CLEAR_VARS)
LOCAL_MODULE := curl-prebuilt
#LOCAL_SRC_FILES := $(LOCAL_PATH)/../prebuilt-with-ssl/android/$(TARGET_ARCH_ABI)/libcurl.a
LOCAL_SRC_FILES := android/$(TARGET_ARCH_ABI)/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)


