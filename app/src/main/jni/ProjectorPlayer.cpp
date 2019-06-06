//
// Created by mvcoder on 2019/5/29.
//

#include "com_changba_songstudio_video_player_ProjectorPlayer.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "video_projector_player_controller.h"

#define LOG_TAG "VideoProjector";

jlong getProjectorPlayerPointer(JNIEnv *jniEnv, jobject obj) {
    jclass clazz = jniEnv->GetObjectClass(obj);
    jmethodID methodId = jniEnv->GetMethodID(clazz, "getNativeObjPointer", "()J");
    return jniEnv->CallLongMethod(obj, methodId);
}

void setProjectorPlayerPointer(JNIEnv *jniEnv, jobject obj, jlong value) {
    jclass clazz = jniEnv->GetObjectClass(obj);
    jmethodID methodId = jniEnv->GetMethodID(clazz, "setNativeObjPointer", "(J)V");
    jniEnv->CallVoidMethod(obj, methodId, value);
}

VideoProjector *getVideoProjector(JNIEnv *env, jobject obj, bool create) {
    jlong value = getProjectorPlayerPointer(env, obj);
    VideoProjector *videoProjector = NULL;
    if (value == -1) {
        if(!create) return NULL;
        videoProjector = new VideoProjector();
        setProjectorPlayerPointer(env, obj, (jlong) videoProjector);
    } else {
        videoProjector = (VideoProjector *) value;
    }
    return videoProjector;
}

extern "C" {

JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_onSurfaceCreated(JNIEnv *env, jobject obj,
                                                                          jobject surface,
                                                                          jint width, jint height) {

    VideoProjector *videoProjector = getVideoProjector(env, obj, true);
    if (videoProjector != NULL) {
        ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
        videoProjector->onSurfaceCreate(window, width, height);
    }
}


JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_onSurfaceDestroyed(JNIEnv *env,
                                                                            jobject obj,
                                                                            jobject surface) {

    VideoProjector *videoProjector = getVideoProjector(env, obj, false);
    if (videoProjector != NULL) {
        videoProjector->onSurfaceDestroy();
        delete videoProjector;
        videoProjector = NULL;
        setProjectorPlayerPointer(env, obj, -1);
    }
}


jboolean
Java_com_changba_songstudio_video_player_ProjectorPlayer_prepare(JNIEnv *env, jobject obj,
                                                                 jstring url) {

    VideoProjector *videoProjector = getVideoProjector(env, obj, true);
    if (videoProjector != NULL) {
        char *videoUrl = (char *) (env->GetStringUTFChars(url, NULL));
        videoProjector->prepare(videoUrl);
        env->ReleaseStringUTFChars(url, videoUrl);
        return true;
    }
    return false;
}

JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_pause(JNIEnv *env, jobject obj) {
    VideoProjector *videoProjector = getVideoProjector(env, obj, false);
    if (videoProjector != NULL) {
        videoProjector->pause();
    }

}

JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_play(JNIEnv *env, jobject obj) {
    VideoProjector *videoProjector = getVideoProjector(env, obj, false);
    if (videoProjector != NULL) {
        videoProjector->pause();
    }
}

JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_stop(JNIEnv *env, jobject obj) {
    VideoProjector *videoProjector = getVideoProjector(env, obj, false);
    if (videoProjector != NULL) {
        videoProjector->pause();
    }
}


}
