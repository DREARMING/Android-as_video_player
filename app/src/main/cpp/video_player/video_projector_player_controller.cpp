//
// Created by mvcoder on 2019/5/29.
//

#include "./video_projector_player_controller.h"
#include "video_projector_player_controller.h"


void ProjectorCallbackImpl::setParent(VideoProjector *projector) {
    this->projector = projector;
}

void ProjectorCallbackImpl::onRenderTexture(FrameTexture *frameTexture) {
    if(projector){
        projector->onRenderTexture(frameTexture);
    }
}

void ProjectorCallbackImpl::onAudioDataAvailable(byte *data, size_t bufferSize) {
    if(projector){
        projector->onAudioDataAvailable(data, bufferSize);
    }
}

VideoProjector::VideoProjector() {
    this->userCancel = false;
}

void VideoProjector::init(char *url) {
    int len = strlen(url);
    videoPath = new char[len+1];
    memcpy(videoPath, url, sizeof(char) * (len + 1));


    projectorCallback = new ProjectorCallbackImpl();
    projectorCallback->setParent(this);
}


void VideoProjector::onRenderTexture(FrameTexture *frameTexture) {

}


void VideoProjector::onAudioDataAvailable(byte *data, int bufferSize) {

}


void VideoProjector::onSurfaceCreate(ANativeWindow *window, int width, int height) {
    this->window = window;
    this->screenHeight = height;
    this->screenWidth = width;

    initVideoOutput(window);
    initAudioOutput();
}

void VideoProjector::onSurfaceDestroy() {
    this->userCancel = true;
}

VideoProjector::~VideoProjector() {
    //回收资源
    if(videoPath) delete videoPath;

    if(projectorCallback) delete projectorCallback;

}

void VideoProjector::initVideoOutput(ANativeWindow *window) {
    if (window == NULL || userCancel){
        return;
    }
}

void VideoProjector::initAudioOutput() {

}




