//
// Created by Szymon on 17/03/2022.
//

#include "RedBigNum.h"
#include <android/log.h>
#define APPNAME "MyApp"

namespace margelo {

RedBigNum::RedBigNum(BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m): BigNumber(ctx) {
  BN_zero(this->bign);
  this->ctx = ctx;
  this->mctx = mctx;
  this->m = m;
}

RedBigNum::RedBigNum(BIGNUM * bign, bool force, BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m): BigNumber(ctx) {
  this->bign = BN_copy(this->bign, bign);
  if (!force) {
      BN_to_montgomery(this->bign, this->bign, mctx, ctx);
  }
  this->ctx = ctx;
  this->mctx = mctx;
  this->m = m;
}

RedBigNum::RedBigNum(const RedBigNum & other): BigNumber(ctx) {
  this->bign = BN_copy(this->bign,other.bign);
  this->ctx = other.ctx;
  this->mctx = other.mctx;
  this->m = other.m;
}

RedBigNum::~RedBigNum() { // mctx is kept alive on js Side

}

jsi::Value RedBigNum::get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId) {
  std::string name = propNameId.utf8(runtime);
  if (name == "isInternalRedBigNum") {
    return jsi::Value(runtime, true);
  }
  return jsi::Value::undefined();
}

}