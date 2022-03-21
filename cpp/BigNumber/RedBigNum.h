//
// Created by Szymon on 17/03/2022.
//

#ifndef BIGNUMBEREXAMPLE_REDBIGNUM_H
#define BIGNUMBEREXAMPLE_REDBIGNUM_H

#import <BigNumber/BigNumber.h>

namespace margelo {
namespace jsi = facebook::jsi;

class RedBigNum : public SmartHostObject {
public:
    explicit RedBigNum(BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m, std::shared_ptr<react::CallInvoker> jsCallInvoker,
            std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue);

    explicit RedBigNum(BIGNUM * bign, BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m, std::shared_ptr<react::CallInvoker> jsCallInvoker,
            std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue);

    explicit RedBigNum(const RedBigNum&);

    virtual ~RedBigNum();

    BN_CTX * ctx;
    BN_MONT_CTX * mctx;
    BIGNUM * bign;
    BIGNUM * m;

    void installMethods();
};

} // namespace margelo


#endif //BIGNUMBEREXAMPLE_REDBIGNUM_H
