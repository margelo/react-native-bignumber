//
// Created by Szymon on 17/03/2022.
//

#ifndef BIGNUMBEREXAMPLE_MODCONTEXT_H
#define BIGNUMBEREXAMPLE_MODCONTEXT_H

#include <openssl/ossl_typ.h>
#include <openssl/bn.h>
#include <jsi/jsi.h>

namespace margelo {
namespace jsi = facebook::jsi;
struct ModContext : jsi::HostObject {
    ModContext(int number, BN_CTX * ctx);
    ModContext(BIGNUM * bign, BN_CTX * ctx);
    virtual ~ModContext();

    jsi::Value get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId);

    static std::string k256;
    static std::string p224;
    static std::string p192;
    static std::string p25519;

    BN_MONT_CTX * mctx;
    BIGNUM * m;
};
} // namespace margelo


#endif //BIGNUMBEREXAMPLE_MODCONTEXT_H
