#include "opensl_es_context.h"

#define LOG_TAG "OpenSLESContext"

OpenSLESContext* OpenSLESContext::instance = new OpenSLESContext();

void OpenSLESContext::init() {
	LOGI("createEngine");
	//创建 opensl 引擎
	SLresult result = createEngine();
	LOGI("createEngine result is s%", ResultToString(result));
	if (SL_RESULT_SUCCESS == result) {
		LOGI("Realize the engine object");
		// Realize the engine object - 实例化引擎
		result = RealizeObject(engineObject);
		if (SL_RESULT_SUCCESS == result) {
			LOGI("Get the engine interface");
			// Get the engine interface
			result = GetEngineInterface();
		}
	}
}

OpenSLESContext::OpenSLESContext() {
	isInited = false;
}
OpenSLESContext::~OpenSLESContext() {
}

OpenSLESContext* OpenSLESContext::GetInstance() {
	if (!instance->isInited) {
		instance->init();
		instance->isInited = true;
	}
	return instance;
}
