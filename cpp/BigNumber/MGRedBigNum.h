//
// Created by Szymon on 17/03/2022.
//

#ifndef BIGNUMBEREXAMPLE_MGREDBIGNUM_H
#define BIGNUMBEREXAMPLE_MGREDBIGNUM_H

#include "MGBigNumber.h"

namespace margelo {
namespace jsi = facebook::jsi;

class MGRedBigNum : public MGBigNumber {
public:
explicit MGRedBigNum(BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m);

explicit MGRedBigNum(BIGNUM * bign, bool force, BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m);

explicit MGRedBigNum(const MGRedBigNum&);

virtual ~MGRedBigNum();

jsi::Value get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId);

BN_MONT_CTX * mctx;
BIGNUM * m;
};

} // namespace margelo


#endif //BIGNUMBEREXAMPLE_MGREDBIGNUM_H
