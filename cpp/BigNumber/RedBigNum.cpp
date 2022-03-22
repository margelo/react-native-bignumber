//
// Created by Szymon on 17/03/2022.
//

#include "RedBigNum.h"
#include <android/log.h>
#define APPNAME "MyApp"

namespace margelo {

RedBigNum::RedBigNum(BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m) {
  this->bign = BN_new();
  BN_zero(this->bign);
  this->ctx = ctx;
  this->mctx = mctx;
  this->m = m;
}

RedBigNum::RedBigNum(BIGNUM * bign, BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m) {
  this->bign = BN_dup(bign);
  BN_to_montgomery(this->bign, this->bign, mctx, ctx);
  this->ctx = ctx;
  this->mctx = mctx;
  this->m = m;
}

RedBigNum::RedBigNum(const RedBigNum & other) {
  this->bign = BN_dup(other.bign);
  this->ctx = other.ctx;
  this->mctx = other.mctx;
  this->m = other.m;
}

RedBigNum::~RedBigNum() { // mctx is kept alive on js Side
  BN_free(this->bign);
}

jsi::Value RedBigNum::get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId) {
  std::string name = propNameId.utf8(runtime);
  if (name == "isInternalRedBigNum") {
    return jsi::Value(runtime, true);
  }
  return jsi::Value::undefined();
}

}