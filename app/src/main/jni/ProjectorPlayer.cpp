//
// Created by mvcoder on 2019/5/29.
//

#include "com_changba_songstudio_video_player_ProjectorPlayer.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "video_projector_player_controller.h"

VideoProjector* videoProjector = NULL;

extern "C" {

JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_onSurfaceCreated(JNIEnv *env, jobject obj,
                                                                          jobject surface,
                                                                          jint width, jint height) {
    if(videoProjector == NULL){
        videoProjector = new VideoProjector();
    }
    ANativeWindow* window = ANativeWindow_fromSurface(env,surface);
    videoProjector->onSurfaceCreate(window, width, height);
}


JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_onSurfaceDestroyed(JNIEnv *env, jobject obj,
                                                                            jobject surface) {
    if(videoProjector){
        videoProjector->onSurfaceDestroy();
        delete videoProjector;
        videoProjector = NULL;
    }
}


jboolean
Java_com_changba_songstudio_video_player_ProjectorPlayer_prepare(JNIEnv *env, jobject obj,
                                                                 jstring url) {

    if(videoProjector == NULL){
        videoProjector = new VideoProjector();
    }
    char* videoUrl =  (char*)(env->GetStringUTFChars(url, NULL));
    videoProjector->prepare(videoUrl);
    env->ReleaseStringUTFChars(url, videoUrl);
    return true;
}

JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_pause(JNIEnv *env, jobject obj) {
    if(videoProjector){
        videoProjector->pause();
    }
}

JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_play(JNIEnv *env, jobject obj) {
    if(videoProjector){
        videoProjector->play();
    }
}

JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_stop(JNIEnv *env, jobject obj) {
    if(videoProjector){
        videoProjector->stop();
    }
}

}
