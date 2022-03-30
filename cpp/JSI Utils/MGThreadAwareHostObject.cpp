//
// Created by Szymon on 24/02/2022.
//

#include "MGThreadAwareHostObject.h"

namespace margelo {

namespace jsi = facebook::jsi;

void MGThreadAwareHostObject::runOnWorkerThread(std::function<void()> &&job) {
  this->dispatchQueue->dispatch(job);
}

void MGThreadAwareHostObject::runOnJSThread(std::function<void()> &&job) {
  auto callInvoker = this->weakJsCallInvoker.lock();
  if (callInvoker != nullptr) {
    callInvoker->invokeAsync(std::move(job));
  }
}

} // namespace margelo

