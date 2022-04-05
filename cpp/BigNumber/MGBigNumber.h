//
// Created by Szymon on 08/03/2022.
//

#ifndef BIGNUMBEREXAMPLE_MGBIGNUMBER_H
#define BIGNUMBEREXAMPLE_MGBIGNUMBER_H

#include <jsi/jsi.h>
#include <ReactCommon/CallInvoker.h>
#include <memory>
#include "MGDispatchQueue.h"
#include "MGSmartHostObject.h"
#include <openssl/ossl_typ.h>
#include <openssl/bn.h>

namespace margelo
{
    namespace jsi = facebook::jsi;

    class MGBigNumber : public jsi::HostObject
    {
    public:
        explicit MGBigNumber(BN_CTX *ctx);

        explicit MGBigNumber(std::string, int base, BN_CTX *ctx);

        explicit MGBigNumber(const unsigned char *s, int len, int base, bool le, BN_CTX *ctx);

        explicit MGBigNumber(int value, BN_CTX *ctx);

        explicit MGBigNumber(const MGBigNumber &);

        virtual ~MGBigNumber();

        virtual jsi::Value get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId);
        virtual std::vector<jsi::PropNameID> getPropertyNames(jsi::Runtime &runtime);

        BIGNUM *bign;
        BN_CTX *ctx;
    };

} // namespace margelo

#endif // BIGNUMBEREXAMPLE_MGBIGNUMBER_H
