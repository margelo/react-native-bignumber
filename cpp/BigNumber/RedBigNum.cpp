//
// Created by Szymon on 17/03/2022.
//

#include "RedBigNum.h"

namespace margelo {

RedBigNum::RedBigNum(BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m, std::shared_ptr<react::CallInvoker> jsCallInvoker,
std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue):
        SmartHostObject(jsCallInvoker, workerQueue) {

    this->bign = BN_new();
    BN_zero(this->bign);
    this->ctx = ctx;
    this->mctx = mctx;
    this->m = m;

    installMethods();
}

RedBigNum::RedBigNum(BIGNUM * bign, BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m, std::shared_ptr<react::CallInvoker> jsCallInvoker,
        std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue):
    SmartHostObject(jsCallInvoker, workerQueue) {

    this->bign = BN_dup(bign);
    this->ctx = ctx;
    this->mctx = mctx;
    this->m = m;

    installMethods();
}

RedBigNum::RedBigNum(const RedBigNum & other):
        SmartHostObject(other.weakJsCallInvoker.lock(), other.dispatchQueue) {

    this->bign = BN_dup(other.bign);
    this->ctx = other.ctx;
    this->mctx = other.mctx;
    this->m = other.m;

    installMethods();
}

RedBigNum::~RedBigNum() { // mctx is kept alive on js Side
    BN_free(this->bign);
}

void RedBigNum::installMethods() { // TODO

    this->fields.push_back(HOST_LAMBDA("redadd", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "iaddn expects integer");
        }
        unsigned int other = otherValue.asNumber();

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        BN_add_word(res->bign, other);
        return jsi::Object::createFromHostObject(runtime, res);
    }));
}

}