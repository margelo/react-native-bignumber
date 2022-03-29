//
// Created by Szymon on 24/02/2022.
//

#ifndef BigNumberEXAMPLE_THREADAWAREHOSTOBJECT_H
#define BigNumberEXAMPLE_THREADAWAREHOSTOBJECT_H



#include <jsi/jsi.h>
#include <ReactCommon/CallInvoker.h>
#include "MGDispatchQueue.h"


namespace margelo {

namespace jsi = facebook::jsi;
namespace react = facebook::react;

class JSI_EXPORT MGThreadAwareHostObject : public jsi::HostObject {
public:
explicit MGThreadAwareHostObject(std::shared_ptr<react::CallInvoker> jsCallInvoker, std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue) :
  weakJsCallInvoker(jsCallInvoker), dispatchQueue(workerQueue)
{
}

virtual ~MGThreadAwareHostObject() {
}

void runOnWorkerThread(std::function<void(void)> && job);
void runOnJSThread(std::function<void(void)> && job);

protected:
std::weak_ptr<react::CallInvoker> weakJsCallInvoker;
std::shared_ptr<DispatchQueue::dispatch_queue> dispatchQueue;
};

} // namespace margelo


#endif //BigNumberEXAMPLE_THREADAWAREHOSTOBJECT_H
