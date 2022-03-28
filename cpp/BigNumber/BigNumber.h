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

class BigNumber : public jsi::HostObject {
public:
explicit BigNumber(BN_CTX * ctx);

explicit BigNumber(std::string, int base, BN_CTX * ctx);

explicit BigNumber(const unsigned char *s, int len, bool le, BN_CTX * ctx);

explicit BigNumber(int value, BN_CTX * ctx);

explicit BigNumber(const BigNumber&);

virtual ~BigNumber();

virtual jsi::Value get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId);

BIGNUM * bign;
BN_CTX * ctx;
};

} // namespace margelo


#endif //BIGNUMBEREXAMPLE_BIGNUMBER_H
