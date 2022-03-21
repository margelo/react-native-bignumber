//
// Created by Szymon on 08/03/2022.
//

#ifndef BIGNUMBEREXAMPLE_BIGNUMBER_H
#define BIGNUMBEREXAMPLE_BIGNUMBER_H

#include <jsi/jsi.h>
#include <ReactCommon/CallInvoker.h>
#include <memory>
#include "Utils/DispatchQueue.h"
#include "JSI Utils/SmartHostObject.h"
#include <openssl/ossl_typ.h>
#include <openssl/bn.h>

namespace margelo {
namespace jsi = facebook::jsi;

class BigNumber : public SmartHostObject {
public:
    explicit BigNumber(BN_CTX * ctx, std::shared_ptr<react::CallInvoker> jsCallInvoker,
std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue);

    explicit BigNumber(std::string, int base, BN_CTX * ctx, std::shared_ptr<react::CallInvoker> jsCallInvoker,
                       std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue);

    explicit BigNumber(int value, BN_CTX * ctx, std::shared_ptr<react::CallInvoker> jsCallInvoker,
                       std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue);

    explicit BigNumber(const BigNumber&);

    virtual ~BigNumber();

    BIGNUM * bign;
    BN_CTX * ctx;
private:

    void installMethods();
};

} // namespace margelo


#endif //BIGNUMBEREXAMPLE_BIGNUMBER_H
