#include "./egl_core.h"
#include  "./egl_share_context.h"
#define LOG_TAG "EGLCore"


EGLCore::EGLCore() {
	pfneglPresentationTimeANDROID = 0;
	display = EGL_NO_DISPLAY;
	context = EGL_NO_CONTEXT;
}

EGLCore::~EGLCore() {
}

void EGLCore::release() {
	if(EGL_NO_DISPLAY != display && EGL_NO_CONTEXT != context){
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		LOGI("after eglMakeCurrent...");
		eglDestroyContext(display, context);
		LOGI("after eglDestroyContext...");
	}
	display = EGL_NO_DISPLAY;
	context = EGL_NO_CONTEXT;
}

void EGLCore::releaseSurface(EGLSurface eglSurface) {
	eglDestroySurface(display, eglSurface);
	eglSurface = EGL_NO_SURFACE;
}

EGLContext EGLCore::getContext(){
	LOGI("return EGLCore getContext...");
	return context;
}

EGLDisplay EGLCore::getDisplay(){
	return display;
}

EGLConfig EGLCore::getConfig(){
	return config;
}

EGLSurface EGLCore::createWindowSurface(ANativeWindow* _window) {
	EGLSurface surface = NULL;
	EGLint format;

	if (_window == NULL){
		LOGE("EGLCore::createWindowSurface  _window is NULL");
		return NULL;
	}

	if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
		LOGE("eglGetConfigAttrib() returned error %d", eglGetError());
		release();
		return surface;
	}
	ANativeWindow_setBuffersGeometry(_window, 0, 0, format);
	//Surface实际上就是一个FrameBuffer，也就是渲染目的地
	if (!(surface = eglCreateWindowSurface(display, config, _window, 0))) {
		LOGE("eglCreateWindowSurface() returned error %d", eglGetError());
	}
	return surface;
}

EGLSurface EGLCore::createOffscreenSurface(int width, int height) {
	EGLSurface surface;
	EGLint PbufferAttributes[] = { EGL_WIDTH, width, EGL_HEIGHT, height, EGL_NONE, EGL_NONE };
	if (!(surface = eglCreatePbufferSurface(display, config, PbufferAttributes))) {
		LOGE("eglCreatePbufferSurface() returned error %d", eglGetError());
	}
	return surface;
}

int EGLCore::setPresentationTime(EGLSurface surface, khronos_stime_nanoseconds_t nsecs) {
	pfneglPresentationTimeANDROID(display, surface, nsecs);
	return 1;
}

int EGLCore::querySurface(EGLSurface surface, int what) {
	int value = -1;
	eglQuerySurface(display, surface, what, &value);
	return value;
}

bool EGLCore::swapBuffers(EGLSurface eglSurface) {
	//绘制FrameBuffer到界面
	return eglSwapBuffers(display, eglSurface);
}

bool EGLCore::makeCurrent(EGLSurface eglSurface) {
	/**
     * 该接口将申请到的display，draw（surface）和 context进行了绑定。也就是说，
     * 在context下的OpenGLAPI指令将draw（surface）作为其渲染最终目的地。
     * 而display作为draw（surface）的前端显示。
     * 调用后，当前线程使用的EGLContex为context。
     * @param eglSurface
     * @return
     */
	return eglMakeCurrent(display, eglSurface, eglSurface, context);
}

void EGLCore::doneCurrent() {
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

bool EGLCore::init() {
	return this->init(NULL);
}

bool EGLCore::initWithSharedContext(){
	EGLContext context = EglShareContext::getSharedContext();

	if (context == EGL_NO_CONTEXT){
		return false;
	}

	return init(context);
}

bool EGLCore::init(EGLContext sharedContext) {
	EGLint numConfigs;
	EGLint width;
	EGLint height;

	//FrameBuffer的属性，包括大小，ARGB占的字节数...
	const EGLint attribs[] = { EGL_BUFFER_SIZE, 32, EGL_ALPHA_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE };

	//获取默认屏幕设备，一般厂商都会支持的
	if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
		LOGE("eglGetDisplay() returned error %d", eglGetError());
		return false;
	}
	//初始化屏幕设备，第二个是EGL major 版本，第三个是 minor版本，不关心的话，可以填0或者NULL
	if (!eglInitialize(display, 0, 0)) {
		LOGE("eglInitialize() returned error %d", eglGetError());
		return false;
	}

	//选择EGL的设置，设置实际指的是FrameBuffer 的设置，FrameBuffer 就是最后用来显示的图片
	if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
		LOGE("eglChooseConfig() returned error %d", eglGetError());
		release();
		return false;
	}

	EGLint eglContextAttributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	/**
	 * OpenGL ES的pipeline（即处理流程）从程序的角度看就是一个状态机，有当前的颜色、纹理坐标、变换矩阵、绚染模式等一大堆状态，
	 * 这些状态作用于OpenGL API程序提交的顶点坐标等图元从而形成帧缓冲内的像素（FrameBuffer）。
	 * 在OpenGL的编程接口中，Context就代表这个状态机，OpenGL API程序的主要工作就是向Context提供图元、设置状态，偶尔也从Context里获取一些信息。
	 */
	if (!(context = eglCreateContext(display, config, NULL == sharedContext ? EGL_NO_CONTEXT : sharedContext, eglContextAttributes))) {
		LOGE("eglCreateContext() returned error %d", eglGetError());
		release();
		return false;
	}

	pfneglPresentationTimeANDROID = (PFNEGLPRESENTATIONTIMEANDROIDPROC)eglGetProcAddress("eglPresentationTimeANDROID");
	if (!pfneglPresentationTimeANDROID) {
		LOGE("eglPresentationTimeANDROID is not available!");
	}

	return true;
}
