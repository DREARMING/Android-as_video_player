#include <jni.h>
#include <string>

extern "C" {
JNIEXPORT jstring JNICALL Java_media_ushow_as_1video_1player_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello From VideoPlayer >>>";
    return env->NewStringUTF(hello.c_str());
}

/*
JNIEXPORT void  JNICALL
Java_com_changba_songstudio_video_player_ProjectorPlayer_onSurfaceCreated(JNIEnv *env, jobject obj,
                                                                          jobject surface) {

}


JNIEXPORT void  JNICALL Java_com_changba_songstudio_video_player_ProjectorPlayer_onSurfaceDestroyed(JNIEnv *, jobject,
                                                                                 jobject) {

}


jboolean
Java_com_changba_songstudio_video_player_ProjectorPlayer_prepare(JNIEnv *, jobject, jstring, jint,
                                                                 jint, jobject) {
    return 0;
}

JNIEXPORT void  JNICALL Java_com_changba_songstudio_video_player_ProjectorPlayer_pause(JNIEnv *, jobject) {

}

JNIEXPORT void  JNICALL Java_com_changba_songstudio_video_player_ProjectorPlayer_play(JNIEnv *, jobject) {

}

JNIEXPORT void  JNICALL Java_com_changba_songstudio_video_player_ProjectorPlayer_stop(JNIEnv *, jobject) {

}*/


}