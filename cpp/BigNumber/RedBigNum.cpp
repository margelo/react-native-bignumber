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

    this->fields.push_back(HOST_LAMBDA("redAdd", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "redAdd expects RedBigNum");
        }
        jsi::Object obj = otherValue.getObject(runtime);
        if (!obj.isHostObject<RedBigNum>(runtime)) {
            throw jsi::JSError(runtime, "redAdd expects RedBigNum");
        }

        std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);

        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_add_quick(res->bign, this->bign, other->bign, this->m); // TODO (Szymon) are we sure that values are less than m?
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redIAdd", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "redIAdd expects RedBigNum");
        }
        jsi::Object obj = otherValue.getObject(runtime);
        if (!obj.isHostObject<RedBigNum>(runtime)) {
            throw jsi::JSError(runtime, "redIAdd expects RedBigNum");
        }

        std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);
        BN_mod_add_quick(this->bign, this->bign, other->bign, this->m); // TODO (Szymon) are we sure that values are less than m?
    }));

    this->fields.push_back(HOST_LAMBDA("redSub", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "redSub expects RedBigNum");
        }
        jsi::Object obj = otherValue.getObject(runtime);
        if (!obj.isHostObject<RedBigNum>(runtime)) {
            throw jsi::JSError(runtime, "redSub expects RedBigNum");
        }

        std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);

        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_sub_quick(res->bign, this->bign, other->bign, this->m); // TODO (Szymon) are we sure that values are less than m?
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redISub", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "redISub expects RedBigNum");
        }
        jsi::Object obj = otherValue.getObject(runtime);
        if (!obj.isHostObject<RedBigNum>(runtime)) {
            throw jsi::JSError(runtime, "redISub expects RedBigNum");
        }

        std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);
        BN_mod_sub_quick(this->bign, this->bign, other->bign, this->m); // TODO (Szymon) are we sure that values are less than m?
    }));

    this->fields.push_back(HOST_LAMBDA("redMul", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "redMul expects RedBigNum");
        }
        jsi::Object obj = otherValue.getObject(runtime);
        if (!obj.isHostObject<RedBigNum>(runtime)) {
            throw jsi::JSError(runtime, "redMul expects RedBigNum");
        }

        std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);

        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_mul_montgomery(res->bign, this->bign, other->bign, this->mctx, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redIMul", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "redIMul expects RedBigNum");
        }
        jsi::Object obj = otherValue.getObject(runtime);
        if (!obj.isHostObject<RedBigNum>(runtime)) {
            throw jsi::JSError(runtime, "redIMul expects RedBigNum");
        }

        std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);
        BN_mod_mul_montgomery(this->bign, this->bign, other->bign, this->mctx, this->ctx);
    }));

    this->fields.push_back(HOST_LAMBDA("redPow", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "redPow expects RedBigNum");
        }
        jsi::Object obj = otherValue.getObject(runtime);
        if (!obj.isHostObject<RedBigNum>(runtime)) {
            throw jsi::JSError(runtime, "redPow expects RedBigNum");
        }

        std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);

        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_exp_mont(res->bign, this->bign, other->bign, this->m, this->ctx, this->mctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redInvm", {
        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_inverse(res->bign, this->bign, this->m, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redNeg", {
        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_sub(res->bign, this->m, this->bign, this->m, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redSqr", {
        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_sqr(res->bign, this->bign, this->m, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redISqr", {
        BN_mod_sqr(this->bign, this->bign, this->m, this->ctx);
    }));

    this->fields.push_back(HOST_LAMBDA("redSqrt", {
        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_sqrt(res->bign, this->bign, this->m, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redShl", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "redShl expects number");
        }
        unsigned int number = (unsigned int) otherValue.asNumber();
        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_lshift(res->bign, this->bign, number, this->m, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(JSI_VALUE("isInternalRedBigNum", {
        return jsi::Value(runtime, true);
    }));
}

}