LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := CONT
LOCAL_SRC_FILES := /home/rodoc/developpement/love/chesslib/Containers/obj/local/arm64-v8a/libcont.a
LOCAL_EXPORT_C_INCLUDES := /home/rodoc/developpement/love/chesslib/Containers/src/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := chesslib
LOCAL_C_INCLUDES := /home/rodoc/developpement/love/chesslib/Containers/src/include
# LOCAL_C_INCLUDE += .
LOCAL_CFLAGS := -D_ANDROID_ -v
# LOCAL_CFLAGS +=
# Add your application source files here...
LOCAL_SRC_FILES := bitboard.c common.c special.c board.c util.c chess.c chesslib.c
LOCAL_SHARED_LIBRARIES := LOVE CONT
# LOCAL_STATIC_LIBRAIRIES := CONT
$(warning $(LOCAL_PATH))
$(warning $(TARGET_ARCH_ABI))
# LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog
# LOCAL_LDLIB := -lcont
include $(BUILD_SHARED_LIBRARY)
# include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := LOVE
LOCAL_SRC_FILES := /home/rodoc/love-android/love/build/intermediates/stripped_native_libs/normalDebug/out/lib/arm64-v8a/liblove.so 
LOCAL_EXPORT_C_INCLUDES := /home/rodoc/love-android/love/src/jni/LuaJIT-2.1/src
include $(PREBUILT_SHARED_LIBRARY)
