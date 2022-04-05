//
// Created by Szymon on 17/03/2022.
//

#include "MGRedBigNum.h"

namespace margelo {

MGRedBigNum::MGRedBigNum(BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m): MGBigNumber(ctx) {
  BN_zero(this->bign);
  this->ctx = ctx;
  this->mctx = mctx;
  this->m = m;
}

MGRedBigNum::MGRedBigNum(BIGNUM * bign, bool force, BN_CTX * ctx, BN_MONT_CTX * mctx, BIGNUM * m): MGBigNumber(ctx) {
  this->bign = BN_copy(this->bign, bign);
    // Sometimes we already have montgomery form so we force that format without
    // convertion.
  if (!force) {
      BN_to_montgomery(this->bign, this->bign, mctx, ctx);
  }
  this->ctx = ctx;
  this->mctx = mctx;
  this->m = m;
}

MGRedBigNum::MGRedBigNum(const MGRedBigNum & other): MGBigNumber(ctx) {
  this->bign = BN_copy(this->bign,other.bign);
  this->ctx = other.ctx;
  this->mctx = other.mctx;
  this->m = other.m;
}

MGRedBigNum::~MGRedBigNum() { // mctx is kept alive on js Side

}

std::vector <jsi::PropNameID> MGRedBigNum::getPropertyNames(
  jsi::Runtime &runtime) {
  std::vector <jsi::PropNameID> propertyNames;
  
  propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "isInternalRedBigNum"));
  propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "toString"));
  propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "toStringTag"));
  propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "value"));
  return propertyNames;
}

jsi::Value MGRedBigNum::get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId) {
  std::string name = propNameId.utf8(runtime);
  if (name == "isInternalRedBigNum") {
    return jsi::Value(runtime, true);
  }
  if (name == "toString") {
    char * str = BN_bn2dec(this->bign);
    std::string strRep(str);
    delete [] str;
    jsi::String res = jsi::String::createFromUtf8(runtime, strRep);
    return jsi::Value(runtime, res);
  }
  if (name == "toStringTag") {
    jsi::String res = jsi::String::createFromUtf8(runtime, "[object MGRedBigNum]");
    return jsi::Value(runtime, res);
  }
    if (name == "value") {
        return jsi::Function::createFromHostFunction(runtime, propNameId, 0, [&](jsi::Runtime &runtime, const jsi::Value &thisValue,
            const jsi::Value *arguments, size_t count)->jsi::Value{
            return thisValue.getObject(runtime).getProperty(runtime, "toString");
        });
    }
  return jsi::Value::undefined();
}

}
