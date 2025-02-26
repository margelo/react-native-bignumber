#include <fbjni/fbjni.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <jni.h>
#include <jsi/jsi.h>
#include "MGBigNumberHostObject.h"

using namespace facebook;

class BigNumberCppAdapter : public jni::HybridClass<BigNumberCppAdapter> {
public:
static auto constexpr kJavaDescriptor = "Lcom/margelo/bignumber/BigNumberModule;";

static jni::local_ref<jni::HybridClass<BigNumberCppAdapter>::jhybriddata> initHybrid(
  jni::alias_ref<jhybridobject> jThis) {
  return makeCxxInstance();
}

explicit BigNumberCppAdapter() {
}

void install(jsi::Runtime& runtime, std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker) {
  try {
    if (!jsCallInvoker) {
      __android_log_print(ANDROID_LOG_ERROR, "BigNumber", "jsCallInvoker is null in install");
      return;
    }

    auto workerQueue = std::make_shared<margelo::DispatchQueue::dispatch_queue>("Margelo MGBigNumber Thread");
    if (!workerQueue) {
      __android_log_print(ANDROID_LOG_ERROR, "BigNumber", "Failed to create worker queue");
      return;
    }

    auto hostObject = std::make_shared<margelo::MGBigNumberHostObject>(jsCallInvoker, workerQueue);

    if (!hostObject) {
        __android_log_print(ANDROID_LOG_ERROR, "BigNumber", "Host object creation failed");
        return;
    }

    auto object = jsi::Object::createFromHostObject(runtime, hostObject);

//    try {
//        auto propNames = object.getPropertyNames(runtime);
//        __android_log_print(ANDROID_LOG_INFO, "BigNumber", "Host object has %d properties", propNames.size(runtime));
//    } catch (const jsi::JSError& e) {
//        __android_log_print(ANDROID_LOG_ERROR, "BigNumber", "Error inspecting host object: %s", e.what());
//        return;
//    }

    try {
        runtime.global().setProperty(runtime, "__BigNumberProxy", std::move(object));
        __android_log_print(ANDROID_LOG_INFO, "BigNumber", "Property set successfully");
    } catch (const jsi::JSError& e) {
        jni::throwNewJavaException("java/lang/IllegalStateException", "JSI Error setting property: %s", e.what());
    } catch (const std::exception& e) {
        jni::throwNewJavaException("java/lang/IllegalStateException", "Exception setting property: %s", e.what());
    } catch (...) {
        jni::throwNewJavaException("java/lang/IllegalStateException", "Unknown exception setting property");
    }
  } catch (const std::exception& e) {
    __android_log_print(ANDROID_LOG_ERROR, "BigNumber", "Exception in install: %s", e.what());
  } catch (...) {
    __android_log_print(ANDROID_LOG_ERROR, "BigNumber", "Unknown exception in install");
  }
}

void nativeInstall(jlong jsiPtr, jni::alias_ref<facebook::react::CallInvokerHolder::javaobject>
                   jsCallInvokerHolder) {
  __android_log_print(ANDROID_LOG_INFO, "BigNumber", "nativeInstall called");

  if (!jsCallInvokerHolder) {
    __android_log_print(ANDROID_LOG_ERROR, "BigNumber", "jsCallInvokerHolder is null");
    return;
  }

  auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
  if (!jsCallInvoker) {
    __android_log_print(ANDROID_LOG_ERROR, "BigNumber", "jsCallInvoker is null");
    return;
  }

  auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
  if (runtime) {
    install(*runtime, jsCallInvoker);
  } else {
    // if runtime was nullptr, MGBigNumber will not be installed. This should only happen while Remote Debugging (Chrome), but will be weird either way.
    __android_log_print(ANDROID_LOG_ERROR, "BigNumber", "Runtime pointer is null");
  }
}

static void registerNatives() {
  registerHybrid({
      makeNativeMethod("initHybrid", BigNumberCppAdapter::initHybrid),
      makeNativeMethod("nativeInstall", BigNumberCppAdapter::nativeInstall)
    });
}

private:
friend HybridBase;
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
  __android_log_print(ANDROID_LOG_INFO, "BigNumber", "JNI_OnLoad called");
  return facebook::jni::initialize(vm, [] {
    __android_log_print(ANDROID_LOG_INFO, "BigNumber", "Registering natives");
    BigNumberCppAdapter::registerNatives();
  });
}
