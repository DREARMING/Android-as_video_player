//
// Created by mvcoder on 2019/5/29.
//
#ifndef ANDROID_AS_VIDEO_PLAYER1_VIDEO_PROJECTOR_PLAYER_CONTROLLER_H
#define ANDROID_AS_VIDEO_PLAYER1_VIDEO_PROJECTOR_PLAYER_CONTROLLER_H

#include <android/native_window.h>
#include <EGL/egl.h>
#include "./common/circle_texture_queue.h"
#include "./common/CommonTools.h"

class VideoProjector;

class ProjectorCallback {

public:
    virtual void onRenderTexture(FrameTexture* frameTexture) = 0;
    virtual void onAudioDataAvailable(byte* data, size_t bufferSize) = 0;
};

class ProjectorCallbackImpl : ProjectorCallback{
public:
    void setParent(VideoProjector* projector);
    void onRenderTexture(FrameTexture* frameTexture);
    void onAudioDataAvailable(byte* data, size_t bufferSize);
private:
    VideoProjector* projector;
};

class VideoProjector{

public:
    VideoProjector();
     virtual ~VideoProjector();
     void init(char* url);
     void onSurfaceCreate(ANativeWindow* window, int width, int height);
     void onSurfaceDestroy();
     void pause();
     void stop();
     void play();
     void onRenderTexture(FrameTexture* frameTexture);
     void onAudioDataAvailable(byte* data, int bufferSize);

private:
    int state;
    int screenWidth;
    int screenHeight;
    char* videoPath;
    bool userCancel;
    ANativeWindow* window;
    ProjectorCallbackImpl* projectorCallback;
    void initVideoOutput(ANativeWindow* window);
    void initAudioOutput();
};

#endif //ANDROID_AS_VIDEO_PLAYER1_VIDEO_PROJECTOR_PLAYER_CONTROLLER_H
