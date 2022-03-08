// Copyright 2022 Margelo
#ifndef CPP_BigNumberHOSTOBJECT_H_
#define CPP_BigNumberHOSTOBJECT_H_

#include <jsi/jsi.h>
#include <ReactCommon/CallInvoker.h>
#include <memory>
#include <openssl/ossl_typ.h>
#include "Utils/DispatchQueue.h"
#include "JSI Utils/SmartHostObject.h"

namespace margelo {

namespace jsi = facebook::jsi;
namespace react = facebook::react;

class JSI_EXPORT BigNumberHostObject : public SmartHostObject {
public:
explicit BigNumberHostObject(std::shared_ptr<react::CallInvoker> jsCallInvoker,
                              std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue);

virtual ~BigNumberHostObject() {

}

    static BN_CTX *bn_ctx;
};

}  // namespace margelo

#endif  // CPP_BigNumberHOSTOBJECT_H_
