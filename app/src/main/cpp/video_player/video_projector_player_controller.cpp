//
// Created by mvcoder on 2019/5/29.
//

#include "./video_projector_player_controller.h"
#include "./video_player_controller.h"
#include "video_projector_player_controller.h"

#define LOG_TAG "VideoProjector"

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

bool ProjectorCallbackImpl::equal(ProjectorCallbackImpl *other) {
    if(!other || !other->getParent()) return false;
    return other->getParent() == this->projector;
}

VideoProjector::VideoProjector() {
    userCancelled = false;
    videoOutput = NULL;
    screenWidth = 0;
    screenHeight = 0;
    projectorCallback = new ProjectorCallbackImpl();
    projectorCallback->setParent(this);
}

VideoProjector::~VideoProjector() {
    LOGI("~VideoPlayerController");
    //回收资源
    if(videoPath) delete videoPath;

    if(projectorCallback) delete projectorCallback;

    videoOutput = NULL;
}

void VideoProjector::init() {

}


void VideoProjector::onRenderTexture(FrameTexture *frameTexture) {
    //记得要判断状态 - 暂停、停止
    if(isPlaying && videoOutput){
        videoOutput->renderVideo(frameTexture);
    }
}


void VideoProjector::onAudioDataAvailable(byte *data, int bufferSize) {

}


void VideoProjector::onSurfaceCreate(ANativeWindow *window, int width, int height) {
    LOGI("enter VideoPlayerController::onSurfaceCreated...");

    if (userCancelled){
        return;
    }

    if (window != NULL){
        this->window = window;
    }

    if (width > 0 && height > 0){
        this->screenHeight = height;
        this->screenWidth = width;
    }
    if (!videoOutput) {
        initVideoOutput(window);
    }else{
        videoOutput->onSurfaceCreated(window);
    }
    LOGI("Leave VideoPlayerController::onSurfaceCreated...");
    //initAudioOutput();
}

void VideoProjector::onSurfaceDestroy() {
    LOGI("enter VideoPlayerController::onSurfaceDestroyed...");
    this->userCancelled = true;
    this->isPlaying = false;
    if (videoOutput) {
        videoOutput->onSurfaceDestroyed();
        videoOutput->stopOutput();
        delete videoOutput;
        videoOutput = NULL;
    }
    if(projectorCallback){
        unRegisterProjectorListener();
        delete projectorCallback;
        projectorCallback = NULL;
    }
    if(window){
        window = NULL;
    }
    if(videoPath){
        delete videoPath;
        videoPath = NULL;
    }
}

void VideoProjector::initVideoOutput(ANativeWindow *window) {
    LOGI("VideoPlayerController::initVideoOutput begin width:%d, height:%d", screenWidth, screenHeight);
    if (window == NULL || userCancelled){
        return;
    }
    videoOutput = new ProjectorVideoOutput();
    videoOutput->initOutput(window, screenWidth, screenHeight);
}

void VideoProjector::initAudioOutput() {

}

void VideoProjector::prepare(char *url) {
    int len = strlen(url);
    //如果上一个画面在播放，清除上一个注册的videoUrl
    if(videoPath != NULL){
        unRegisterProjectorListener();
        delete videoPath;
        videoPath = NULL;
    }
    videoPath = new char[len+1];
    memcpy(videoPath, url, sizeof(char) * (len + 1));
    registerProjectorListener();
}

void VideoProjector::pause() {
    this->isPlaying = false;
}

void VideoProjector::stop() {
    this->isPlaying = false;
    //停止读取图像
    unRegisterProjectorListener();
    //最好绘制黑屏

}

void VideoProjector::play() {
    if(!registerCallback){
        registerProjectorListener();
    }
    this->isPlaying = true;
}

void VideoProjector::registerProjectorListener() {
    string url = videoPath;
    VideoPlayerController* controller = VideoPlayerController::getPlayerControlWithUrl(url);
    if(controller){
        LOGI("find a video controller, can projection!! register callback now");
        controller->registerProjectorCallback(projectorCallback);
        registerCallback = true;
    }else{
        this->registerCallback = false;
        LOGI("this url %s doesn't have playing source...", url.c_str());
    }
}

void VideoProjector::unRegisterProjectorListener() {
    string url = videoPath;
    VideoPlayerController* controller = VideoPlayerController::getPlayerControlWithUrl(url);
    if(controller != NULL){
        LOGI("find a video controller, unregister callback now");
        controller->unRegisterCallback(projectorCallback);
    }else{
        LOGI("this url %s doesn't have playing source...may be remove from screen...", url.c_str());
    }
}




