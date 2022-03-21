//
// Created by Szymon on 17/03/2022.
//

#include "ModContext.h"

namespace margelo {
    ModContext::ModContext(BIGNUM * bign, BN_CTX * ctx) {
        this->mctx = BN_MONT_CTX_new();
        this->m = bign;
        BN_MONT_CTX_set(this->mctx, bign, ctx);
    }
    ModContext::~ModContext() {
        BN_MONT_CTX_free(this->mctx);
        BN_free(this->m);
    }
} // namespace margelo