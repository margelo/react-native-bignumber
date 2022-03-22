//
// Created by Szymon on 17/03/2022.
//

#include "RedBigNum.h"
#include <android/log.h>
#define APPNAME "MyApp"

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
    BN_to_montgomery(this->bign, this->bign, mctx, ctx);
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
        return jsi::Value::undefined();
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
        return jsi::Value::undefined();
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

    this->fields.push_back(HOST_LAMBDA("clone", {
        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("isZero", {
        return jsi::Value(runtime, BN_is_zero(this->bign) ? true : false );
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
        return jsi::Value::undefined();
    }));

    this->fields.push_back(HOST_LAMBDA("redPow", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "redPow expects RedBigNum");
        }
        jsi::Object obj = otherValue.getObject(runtime);
        if (!obj.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "redPow expects RedBigNum");
        }

        std::shared_ptr<BigNumber> other = obj.getHostObject<BigNumber>(runtime);


        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);

        BN_one(res->bign);
        BN_to_montgomery(res->bign, res->bign, this->mctx, this->ctx);
        BIGNUM * y = BN_dup(other->bign);

        BIGNUM * temp = BN_dup(this->bign);

        while (!BN_is_zero(y)) {
            if (BN_is_odd(y)) {
                BN_mod_mul_montgomery(res->bign, res->bign, temp, this->mctx, this->ctx);
                if (BN_is_one(y)) {
                    break;
                }
            }
            BN_mod_mul_montgomery(temp, temp, temp, this->mctx, this->ctx);
            BN_rshift1(y, y);
        }

        BN_free(y);
        BN_free(temp);

        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redInvm", {
        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_inverse(res->bign, this->bign, this->m, this->ctx);
        BN_to_montgomery(res->bign, res->bign, this->mctx, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redNeg", {
        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_sub(res->bign, this->m, this->bign, this->m, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redSqr", {
        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_mul_montgomery(res->bign, this->bign, this->bign, this->mctx, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("redISqr", {
        BN_mod_mul_montgomery(this->bign, this->bign, this->bign, this->mctx, this->ctx);
        return jsi::Value::undefined();
    }));

    this->fields.push_back(HOST_LAMBDA("redSqrt", { // unfortunatelly may be slow
        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(this->ctx, this->mctx, this->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);

        BIGNUM * temp = BN_new();
        BN_from_montgomery(temp, this->bign, this->mctx, this->ctx);

        BN_mod_sqrt(temp, temp, this->m, this->ctx);
        BN_to_montgomery(res->bign, temp, this->mctx, this->ctx);

        BN_free(temp);
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