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
    ModContext(BIGNUM * bign, BN_CTX * ctx);
    virtual ~ModContext();
    BN_MONT_CTX * mctx;
    BIGNUM * m;
};
} // namespace margelo


#endif //BIGNUMBEREXAMPLE_MODCONTEXT_H
