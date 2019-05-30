//
// Created by mvcoder on 2019/5/29.
//

#include "com_changba_songstudio_video_player_ProjectorPlayer.h"

extern "C" {


JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_onSurfaceCreated(JNIEnv *env, jobject obj,
        jobject surface) {

}


JNIEXPORT void  JNICALL Java_com_changba_songstudio_video_player_ProjectorPlayer_onSurfaceDestroyed(JNIEnv *, jobject,
jobject) {

}


jboolean
Java_com_changba_songstudio_video_player_ProjectorPlayer_prepare(JNIEnv *, jobject obj, jstring url, jint width,
                                                                 jint height, jobject surface) {

    return 0;
}

JNIEXPORT void  JNICALL Java_com_changba_songstudio_video_player_ProjectorPlayer_pause(JNIEnv *, jobject) {

}

JNIEXPORT void  JNICALL Java_com_changba_songstudio_video_player_ProjectorPlayer_play(JNIEnv *, jobject) {

}

JNIEXPORT void  JNICALL Java_com_changba_songstudio_video_player_ProjectorPlayer_stop(JNIEnv *, jobject) {

}

}
