//
// Created by Szymon on 17/03/2022.
//

#ifndef BIGNUMBEREXAMPLE_REDBIGNUM_H
#define BIGNUMBEREXAMPLE_REDBIGNUM_H

#include <BigNumber/BigNumber.h>

namespace margelo {
namespace jsi = facebook::jsi;

class RedBigNum : public BigNumber {
public:
explicit RedBigNum(BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m);

explicit RedBigNum(BIGNUM * bign, bool force, BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m);

explicit RedBigNum(const RedBigNum&);

virtual ~RedBigNum();

jsi::Value get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId);

BN_MONT_CTX * mctx;
BIGNUM * m;
};

} // namespace margelo


#endif //BIGNUMBEREXAMPLE_REDBIGNUM_H
