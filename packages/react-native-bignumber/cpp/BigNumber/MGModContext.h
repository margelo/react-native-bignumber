//
// Created by Szymon on 17/03/2022.
//

#ifndef BIGNUMBEREXAMPLE_MGMODCONTEXT_H
#define BIGNUMBEREXAMPLE_MGMODCONTEXT_H

#include <openssl/ossl_typ.h>
#include <openssl/bn.h>
#include <jsi/jsi.h>

namespace margelo
{
    namespace jsi = facebook::jsi;
    struct MGModContext : jsi::HostObject
    {
        MGModContext(int number, BN_CTX *ctx);
        MGModContext(BIGNUM *bign, BN_CTX *ctx);
        virtual ~MGModContext();

        jsi::Value get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId);
        virtual std::vector<jsi::PropNameID> getPropertyNames(jsi::Runtime &runtime);

        static std::string k256;
        static std::string p224;
        static std::string p192;
        static std::string p25519;

        BN_MONT_CTX *mctx;
        BIGNUM *m;
    };
} // namespace margelo

#endif // BIGNUMBEREXAMPLE_MGMODCONTEXT_H
