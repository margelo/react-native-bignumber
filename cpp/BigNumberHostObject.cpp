// Copyright 2022 Margelo
#include "BigNumberHostObject.h"
#include <ReactCommon/TurboModuleUtils.h>
#include <jsi/jsi.h>
#include <vector>
#include <memory>

namespace margelo {

namespace jsi = facebook::jsi;

BigNumberHostObject::BigNumberHostObject(std::shared_ptr<react::CallInvoker> jsCallInvoker,
                                           std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue) :
  SmartHostObject(jsCallInvoker, workerQueue) {
  this->fields.push_back(HOST_LAMBDA("runAsync", {
      return react::createPromiseAsJSIValue(runtime, [this](jsi::Runtime &runtime,
                                                            std::shared_ptr<react::Promise> promise) {
	this->runOnWorkerThread([this, promise]() {
	  this->runOnJSThread([=]() {
	    promise->resolve(5);
	  });
	});
      });
    }));
}

}  // namespace margelo
