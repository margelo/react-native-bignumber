//
// Created by Szymon on 17/03/2022.
//

#include "ModContext.h"
#include <BigNumber/BigNumber.h>
#include <BigNumberHostObject.h>

namespace margelo {
    std::string ModContext::k256 = std::string("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
    std::string ModContext::p224 = std::string("ffffffffffffffffffffffffffffffff000000000000000000000001");
    std::string ModContext::p192 = std::string("fffffffffffffffffffffffffffffffeffffffffffffffff");
    std::string ModContext::p25519 = std::string("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed");

    ModContext::ModContext(int number, BN_CTX * ctx) {
        this->mctx = BN_MONT_CTX_new();
        this->m = BN_new();
        BN_set_word(this->m, number);
        BN_MONT_CTX_set(this->mctx, this->m, ctx);
    }
    ModContext::ModContext(BIGNUM * bign, BN_CTX * ctx) {
        this->mctx = BN_MONT_CTX_new();
        this->m = BN_new();
        BN_copy(this->m, bign);
        BN_MONT_CTX_set(this->mctx, this->m, ctx);
    }

    jsi::Value ModContext::get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId) {
        std::string name = propNameId.utf8(runtime);
        if (name == "isModContext") {
            return jsi::Value(runtime, true);
        }
        if (name == "m") {
            std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(BigNumberHostObject::bn_ctx);
            BN_copy(res->bign, this->m);
            jsi::Object obj = jsi::Object::createFromHostObject(runtime, res);
            return runtime.global().getPropertyAsFunction(runtime, "__createBN").call(runtime, obj);
        }
        return jsi::Value::undefined();
    }

    ModContext::~ModContext() {
        BN_MONT_CTX_free(this->mctx);
        BN_free(this->m);
    }
} // namespace margelo