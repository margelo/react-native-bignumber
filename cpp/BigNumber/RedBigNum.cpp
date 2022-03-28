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

RedBigNum::RedBigNum(BIGNUM * bign, bool force, BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m) {
  this->bign = BN_dup(bign);
  if (!force) {
      BN_to_montgomery(this->bign, this->bign, mctx, ctx);
  }
  this->ctx = ctx;
  this->mctx = mctx;
  this->m = m;
  __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "aaa The value m is %s", BN_bn2dec(this->m));
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