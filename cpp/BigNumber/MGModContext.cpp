//
// Created by Szymon on 17/03/2022.
//

#include "MGModContext.h"
#include "MGBigNumber.h"
#include "MGBigNumberHostObject.h"

namespace margelo {
    std::string MGModContext::k256 = std::string("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
    std::string MGModContext::p224 = std::string("ffffffffffffffffffffffffffffffff000000000000000000000001");
    std::string MGModContext::p192 = std::string("fffffffffffffffffffffffffffffffeffffffffffffffff");
    std::string MGModContext::p25519 = std::string("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed");

    MGModContext::MGModContext(int number, BN_CTX * ctx) {
        this->mctx = BN_MONT_CTX_new();
        this->m = BN_new();
        BN_set_word(this->m, number);
        BN_MONT_CTX_set(this->mctx, this->m, ctx);
    }
    MGModContext::MGModContext(BIGNUM * bign, BN_CTX * ctx) {
        this->mctx = BN_MONT_CTX_new();
        this->m = BN_new();
        BN_copy(this->m, bign);
        BN_MONT_CTX_set(this->mctx, this->m, ctx);
    }

    jsi::Value MGModContext::get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId) {
        std::string name = propNameId.utf8(runtime);
        if (name == "isModContext") {
            return jsi::Value(runtime, true);
        }
        if (name == "m") {
            std::shared_ptr<MGBigNumber> res = std::make_shared<MGBigNumber>(MGBigNumberHostObject::bn_ctx);
            BN_copy(res->bign, this->m);
            jsi::Object obj = jsi::Object::createFromHostObject(runtime, res);
            return runtime.global().getPropertyAsFunction(runtime, "__createBN").call(runtime, obj);
        }
        return jsi::Value::undefined();
    }

    MGModContext::~MGModContext() {
        BN_MONT_CTX_free(this->mctx);
        BN_free(this->m);
    }
} // namespace margelo