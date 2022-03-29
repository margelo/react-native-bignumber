#include <fbjni/fbjni.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <jni.h>
#include <jsi/jsi.h>
#include "BigNumberHostObject.h"

using namespace facebook;

class BigNumberCppAdapter : public jni::HybridClass<BigNumberCppAdapter> {
public:
static auto constexpr kJavaDescriptor = "Lcom/reactnativebignumber/BigNumberModule;";

static jni::local_ref<jni::HybridClass<BigNumberCppAdapter>::jhybriddata> initHybrid(
  jni::alias_ref<jhybridobject> jThis) {
  return makeCxxInstance();
}

explicit BigNumberCppAdapter() {
}

void install(jsi::Runtime& runtime, std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker) {
  auto workerQueue = std::make_shared<margelo::DispatchQueue::dispatch_queue>("Margelo BigNumber Thread");
  auto hostObject = std::make_shared<margelo::BigNumberHostObject>(jsCallInvoker, workerQueue);
  auto object = jsi::Object::createFromHostObject(runtime, hostObject);
  runtime.global().setProperty(runtime, "__BigNumberProxy", std::move(object));
}

void nativeInstall(jlong jsiPtr, jni::alias_ref<facebook::react::CallInvokerHolder::javaobject>
                   jsCallInvokerHolder) {
  auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
  auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
  if (runtime) {
    install(*runtime, jsCallInvoker);
  }
  // if runtime was nullptr, BigNumber will not be installed. This should only happen while Remote Debugging (Chrome), but will be weird either way.
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
  return facebook::jni::initialize(vm, [] {
    BigNumberCppAdapter::registerNatives();
  });
}
