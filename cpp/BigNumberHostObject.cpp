// Copyright 2022 Margelo
#include "BigNumberHostObject.h"
#include <ReactCommon/TurboModuleUtils.h>
#include <jsi/jsi.h>
#include <vector>
#include <memory>
#include <openssl/bn.h>
#include <BigNumber/BigNumber.h>
#include <BigNumber/ModContext.h>
#include <BigNumber/RedBigNum.h>
#include "Utils/BigNumHelper.h"
#include <android/log.h>
#define APPNAME "MyApp"

namespace margelo {

namespace jsi = facebook::jsi;

BN_CTX * BigNumberHostObject::bn_ctx = BN_CTX_new();

BigNumberHostObject::BigNumberHostObject(std::shared_ptr<react::CallInvoker> jsCallInvoker,
                                         std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue) :
  SmartHostObject(jsCallInvoker, workerQueue) {
  this->fields.push_back(HOST_LAMBDA("createFromString", {
      std::string strRep = arguments[0].getString(runtime).utf8(runtime);
      strRep.erase(std::remove_if(strRep.begin(), strRep.end(), std::isspace), strRep.end());
      int base = 10;
      if (!arguments[1].isUndefined()) {
	base = arguments[1].asNumber();
      }

      if (!arguments[2].isUndefined() && arguments[2].getBool()) {
          std::reverse(strRep.begin(), strRep.end());
          for (int i = 1; i < strRep.size(); i += 2) {
              std::swap(strRep[i], strRep[i-1]);
          }
      }

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(strRep, base, BigNumberHostObject::bn_ctx);

      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("createFromArray", {
      const jsi::Array & array = arguments[0].asObject(runtime).asArray(runtime);
      int size = array.size(runtime);
      bool le = arguments[2].getBool();

      unsigned char * s = new unsigned char[size];

      for (int i = 0; i < size; ++i) {
        int val = array.getValueAtIndex(runtime, i).asNumber();
        s[i] = val;
      }

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(s, size, le, BigNumberHostObject::bn_ctx);
      delete [] s;

      return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("createFromArrayBuffer", {
        jsi::ArrayBuffer array = arguments[0].asObject(
                runtime).getArrayBuffer(runtime);
        int size = array.size(runtime);
        bool le = arguments[2].getBool();

        const unsigned char * s = array.data(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(s, size, le, BigNumberHostObject::bn_ctx);

        return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("toArray", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      int len = -1;

      bool le = arguments[0].getBool();

      if (arguments[1].isNumber()) {
	len = arguments[1].asNumber();
      }
      int numberLen = BN_num_bytes(thiz->bign);
      if (len == -1) {
          len = numberLen;
      }
      unsigned char * to = new unsigned char[len];
      if (le) {
          BN_bn2lebinpad(thiz->bign, to, len);
      } else {
          BN_bn2binpad(thiz->bign, to, len);
      }

      jsi::Array res(runtime, len);
      delete [] to;

      for (int i = 0; i < len; ++i) {
	    res.setValueAtIndex(runtime, i, jsi::Value(runtime, (int)to[i]));
      }

      return res;
    }));

  this->fields.push_back(HOST_LAMBDA("createFromNumber", {
      int val = (int) arguments[0].asNumber();


      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(val, BigNumberHostObject::bn_ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("createModCtx", {
      const jsi::Value & val = arguments[0];
      if (!val.isObject()) {
	throw jsi::JSError(runtime, "createModCtx expects bignumber");
      }
      jsi::Object obj = val.asObject(runtime);

      if (!obj.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "createModCtx expects bignumber");
      }

      std::shared_ptr<BigNumber> mod = obj.getHostObject<BigNumber>(runtime);

      std::shared_ptr<ModContext> res = std::make_shared<ModContext>(mod->bign, mod->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("createModCtxFromNumber", {
      const jsi::Value & val = arguments[0];
      if (!val.isNumber()) {
	throw jsi::JSError(runtime, "createModCtxFromNumber expects integer");
      }
      int number = val.asNumber();

      std::shared_ptr<ModContext> res = std::make_shared<ModContext>(number, BigNumberHostObject::bn_ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("bn2Mod", {
      const jsi::Value & val = arguments[0];
      if (!val.isObject()) {
	throw jsi::JSError(runtime, "bn2Mod expects first arg to be a bignumber");
      }
      jsi::Object obj = val.asObject(runtime);

      if (!obj.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "bn2Mod expects first arg to be a bignumber");
      }

      std::shared_ptr<BigNumber> number = obj.getHostObject<BigNumber>(runtime);

      const jsi::Value & val2 = arguments[1];
      if (!val2.isObject()) {
	throw jsi::JSError(runtime, "bn2Mod expects second arg to be a ModContext");
      }
      jsi::Object obj2 = val2.asObject(runtime);

      if (!obj2.isHostObject<ModContext>(runtime)) {
	throw jsi::JSError(runtime, "bn2Mod expects second arg to be a ModContext");
      }

      std::shared_ptr<ModContext> modCtx = obj2.getHostObject<ModContext>(runtime);

      std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(number->bign, false, BigNumberHostObject::bn_ctx, modCtx->mctx, modCtx->m);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("forceCreateRed", {
        jsi::Object obj = thisValue.getObject(runtime);

        if (!obj.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "forceCreateRed expects first arg to be a bignumber");
        }

        std::shared_ptr<BigNumber> number = obj.getHostObject<BigNumber>(runtime);

        const jsi::Value & val2 = arguments[0];
        if (!val2.isObject()) {
            throw jsi::JSError(runtime, "forceCreateRed expects second arg to be a ModContext");
        }
        jsi::Object obj2 = val2.asObject(runtime);

        if (!obj2.isHostObject<ModContext>(runtime)) {
            throw jsi::JSError(runtime, "forceCreateRed expects second arg to be a ModContext");
        }

        std::shared_ptr<ModContext> modCtx = obj2.getHostObject<ModContext>(runtime);

        std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(number->bign, true, BigNumberHostObject::bn_ctx, modCtx->mctx, modCtx->m);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("mod2bn", {
      const jsi::Value & val = arguments[0];
      if (!val.isObject()) {
	throw jsi::JSError(runtime, "mod2bn expects first arg to be a RedBigNumber");
      }
      jsi::Object obj = val.asObject(runtime);

      if (!obj.isHostObject<RedBigNum>(runtime)) {
	throw jsi::JSError(runtime, "mod2bn expects first arg to be a RedBigNumber");
      }

      std::shared_ptr<RedBigNum> redNumber = obj.getHostObject<RedBigNum>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(BigNumberHostObject::bn_ctx);
      BN_from_montgomery(res->bign, redNumber->bign, redNumber->mctx, BigNumberHostObject::bn_ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("getPrimeContext", {
        const jsi::Value & val = arguments[0];
        if (!val.isString()) {
            throw jsi::JSError(runtime, "getPrimeContext expects first arg to be a string");
        }
        std::string prime = val.asString(runtime).utf8(runtime);

        std::shared_ptr<BigNumber> constant;

        if (prime == "k256") {
            constant = std::make_shared<BigNumber>(ModContext::k256.c_str(), 16, BigNumberHostObject::bn_ctx);
        }
        if (prime == "p224") {
            constant = std::make_shared<BigNumber>(ModContext::p224.c_str(), 16, BigNumberHostObject::bn_ctx);
        }
        if (prime == "p192") {
            constant = std::make_shared<BigNumber>(ModContext::p192.c_str(), 16, BigNumberHostObject::bn_ctx);
        }
        if (prime == "p25519") {
            constant = std::make_shared<BigNumber>(ModContext::p25519.c_str(), 16, BigNumberHostObject::bn_ctx);
        }

        std::shared_ptr<ModContext> res = std::make_shared<ModContext>(constant->bign, constant->ctx);

        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("getPrime", {
        const jsi::Value & val = arguments[0];
        if (!val.isString()) {
            throw jsi::JSError(runtime, "getPrime expects first arg to be a string");
        }
        std::string prime = val.asString(runtime).utf8(runtime);

        std::shared_ptr<BigNumber> constant;

        if (prime == "k256") {
            constant = std::make_shared<BigNumber>(ModContext::k256.c_str(), 16, BigNumberHostObject::bn_ctx);
        }
        if (prime == "p224") {
            constant = std::make_shared<BigNumber>(ModContext::p224.c_str(), 16, BigNumberHostObject::bn_ctx);
        }
        if (prime == "p192") {
            constant = std::make_shared<BigNumber>(ModContext::p192.c_str(), 16, BigNumberHostObject::bn_ctx);
        }
        if (prime == "p25519") {
            constant = std::make_shared<BigNumber>(ModContext::p25519.c_str(), 16, BigNumberHostObject::bn_ctx);
        }

        return jsi::Object::createFromHostObject(runtime, constant);
    }));

  // BigNumber.cpp
  this->fields.push_back(HOST_LAMBDA("toString", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);

      int base = 10;
      if (!arguments[0].isUndefined() && arguments[0].isNumber()) {
	base = (int) arguments[0].asNumber();
      }
      int len = -1;
      if (!arguments[1].isUndefined()) {
	len = (int) arguments[1].asNumber();
      }

      std::string res = BigNumHelper::bn2Str(thiz->bign, base, len);
      return jsi::String::createFromAscii(runtime, res.c_str());
    }));

  // with number

  this->fields.push_back(HOST_LAMBDA("iaddn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "iaddn expects integer");
      }
      int other = otherValue.asNumber();

      if (other < 0) {
	BN_sub_word(thiz->bign, abs(other));
      } else {
	BN_add_word(thiz->bign, other);
      }

      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("addn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "iaddn expects integer");
      }
      int other = otherValue.asNumber();

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_copy(res->bign, thiz->bign);

      if (other < 0) {
	BN_sub_word(res->bign, abs(other));
      } else {
	BN_add_word(res->bign, other);
      }
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("isubn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "isubn expects integer");
      }
      int other = otherValue.asNumber();

      if (other < 0) {
	BN_add_word(thiz->bign, abs(other));
      } else {
	BN_sub_word(thiz->bign, other);
      }
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("subn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "subn expects integer");
      }
      int other = otherValue.asNumber();

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_copy(res->bign, thiz->bign);

      if (other < 0) {
	BN_add_word(res->bign, abs(other));
      } else {
	BN_sub_word(res->bign, other);
      }
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("imuln", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "imuln expects integer");
      }
      int other = otherValue.asNumber();

      BN_mul_word(thiz->bign, abs(other));           //TODO(Szymon) other can be negative
      if (other < 0) {
	BigNumHelper::BN_smart_neg(thiz->bign);
      }
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("muln", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "muln expects integer");
      }
      int other = otherValue.asNumber();

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_copy(res->bign, thiz->bign);
      BN_mul_word(res->bign, abs(other));
      if (other < 0) {
	BigNumHelper::BN_smart_neg(res->bign);
      }
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("idivn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "idivn expects integer");
      }
      int other = otherValue.asNumber();

      std::shared_ptr<BigNumber> otherBn = std::make_shared<BigNumber>(thiz->ctx);
      BN_set_word(otherBn->bign, abs(other));
      BN_div(thiz->bign, nullptr, thiz->bign, otherBn->bign, thiz->ctx);
      if (other < 0) {
	BigNumHelper::BN_smart_neg(thiz->bign);
      }
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("divn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "divn expects integer");
      }
      int other = otherValue.asNumber();

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_copy(res->bign, thiz->bign);

      std::shared_ptr<BigNumber> otherBn = std::make_shared<BigNumber>(thiz->ctx);
      BN_set_word(otherBn->bign, abs(other));
      BN_div(res->bign, nullptr, res->bign, otherBn->bign, thiz->ctx);
      if (other < 0) {
	BigNumHelper::BN_smart_neg(res->bign);
      }
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("imodn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "imodn expects integer");
      }
      int other = otherValue.asNumber();

      std::shared_ptr<BigNumber> otherBn = std::make_shared<BigNumber>(thiz->ctx);
      BN_set_word(otherBn->bign, abs(other));
      BN_div(nullptr, thiz->bign, thiz->bign, otherBn->bign, thiz->ctx);
      if (other < 0) {
	BigNumHelper::BN_smart_neg(thiz->bign);
      }

      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("modn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "divn expects integer");
      }
      int other = otherValue.asNumber();

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_copy(res->bign, thiz->bign);

      std::shared_ptr<BigNumber> otherBn = std::make_shared<BigNumber>(thiz->ctx);
      BN_set_word(otherBn->bign, abs(other));
      BN_div(nullptr, res->bign, res->bign, otherBn->bign, thiz->ctx);
      if (other < 0) {
	BigNumHelper::BN_smart_neg(res->bign);
      }
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("setn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & indexValue = arguments[0];
      const jsi::Value & newValue = arguments[1];
      if (!indexValue.isNumber() || !newValue.isNumber()) {
	throw jsi::JSError(runtime, "setn expects 2 integers");
      }
      int value = newValue.asNumber();
      unsigned int index = indexValue.asNumber();

      if (value) {
	BN_set_bit(thiz->bign, index);
      } else {
	BN_set_bit(thiz->bign, index);                 // to expand if needed
	BN_clear_bit(thiz->bign, index);
      }
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("testn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "testn expects integer");
      }
      unsigned int other = otherValue.asNumber();

      return jsi::Value(runtime, BN_is_bit_set(thiz->bign, other) != 0);
    }));

  this->fields.push_back(HOST_LAMBDA("bincn", {       // Can be optimised with low level API
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "testn expects integer");
      }
      unsigned int value = otherValue.asNumber();
      std::shared_ptr<BigNumber> temp = std::make_shared<BigNumber>(thiz->ctx);

      BN_zero(temp->bign);
      BN_set_bit(temp->bign, value);
      BN_add(thiz->bign, temp->bign, thiz->bign);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("inotn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "testn expects integer");
      }
      unsigned int len = otherValue.asNumber();

      BigNumHelper::BN_notn(thiz->bign, len);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("imaskn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "imaskn expects integer");
      }
      unsigned int other = otherValue.asNumber();
      BN_mask_bits(thiz->bign, other);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("notn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "notn expects integer");
      }
      unsigned int len = otherValue.asNumber();
      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_copy(res->bign, thiz->bign);
      BigNumHelper::BN_notn(res->bign, len);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("maskn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "maskn expects integer");
      }
      unsigned int other = otherValue.asNumber();
      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_copy(res->bign, thiz->bign);
      BN_mask_bits(res->bign, other);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("ishln", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "ishln expects integer");
      }
      unsigned int other = otherValue.asNumber();
      BN_lshift(thiz->bign, thiz->bign, other);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("shln", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "shln expects integer");
      }
      unsigned int other = otherValue.asNumber();
      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_lshift(res->bign, thiz->bign, other);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("ishrn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "ishrn expects integer");
      }
      unsigned int other = otherValue.asNumber();
      BN_rshift(thiz->bign, thiz->bign, other);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("shrn", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "shrn expects integer");
      }
      unsigned int other = otherValue.asNumber();
      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_rshift(res->bign, thiz->bign, other);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  // with BigNumber

  this->fields.push_back(HOST_LAMBDA("iadd", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "iadd expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "iadd expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
      BN_add(thiz->bign, other->bign, thiz->bign);

      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("add", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "add expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "add expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_add(res->bign, other->bign, thiz->bign);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("isub", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "isub expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "isub expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
      BN_sub(thiz->bign, thiz->bign, other->bign);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("sub", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "sub expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "sub expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_sub(res->bign, thiz->bign, other->bign);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("imul", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "imul expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "imul expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
      BN_mul(thiz->bign, other->bign, thiz->bign, thiz->ctx);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("mul", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "mul expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "mul expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_mul(res->bign, other->bign, thiz->bign, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("idiv", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "idiv expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "idiv expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
      BN_div(thiz->bign, nullptr, thiz->bign, other->bign, thiz->ctx);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("div", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "div expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "div expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_div(res->bign, nullptr, thiz->bign, other->bign, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("divmod", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "divmod expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "divmod expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      std::shared_ptr<BigNumber> rem = std::make_shared<BigNumber>(thiz->ctx);
      BN_div(res->bign, rem->bign, thiz->bign, other->bign, thiz->ctx);
      jsi::Object obj(runtime);
      obj.setProperty(runtime, "div", jsi::Object::createFromHostObject(runtime, res));
      obj.setProperty(runtime, "mod", jsi::Object::createFromHostObject(runtime, rem));
      return obj;
    }));

  this->fields.push_back(HOST_LAMBDA("divRound", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "divRound expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "divRound expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      std::shared_ptr<BigNumber> rem = std::make_shared<BigNumber>(thiz->ctx);

      BN_copy(res->bign, thiz->bign);
      if (BN_is_zero(res->bign)) {
	return jsi::Object::createFromHostObject(runtime, res);
      }

      std::shared_ptr<BigNumber> temp = std::make_shared<BigNumber>(thiz->ctx);
      BN_div(res->bign, rem->bign, thiz->bign, other->bign, thiz->ctx);
      BN_sub(temp->bign, other->bign, rem->bign);
      if (BN_cmp(rem->bign, temp->bign) < 0) {
	return jsi::Object::createFromHostObject(runtime, res);
      } else {
	BN_add_word(res->bign, 1);
	return jsi::Object::createFromHostObject(runtime, res);
      }
    }));

  this->fields.push_back(HOST_LAMBDA("isqr", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      BN_sqr(thiz->bign, thiz->bign, thiz->ctx);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("sqr", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_sqr(res->bign, thiz->bign, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("isZero", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      if (BN_is_zero(thiz->bign)) {
	return jsi::Value(runtime, true);
      } else {
	return jsi::Value(runtime, false);
      }
    }));

  this->fields.push_back(HOST_LAMBDA("clone", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_copy(res->bign, thiz->bign);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("isOne", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      if (BN_is_one(thiz->bign)) {
	return jsi::Value(runtime, true);
      } else {
	return jsi::Value(runtime, false);
      }
    }));

  this->fields.push_back(HOST_LAMBDA("bitLength", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      return jsi::Value(runtime, BN_num_bits(thiz->bign));
    }));

  this->fields.push_back(HOST_LAMBDA("byteLength", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      return jsi::Value(runtime, BN_num_bytes(thiz->bign));
    }));

  this->fields.push_back(HOST_LAMBDA("isNeg", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      if (BN_is_negative(thiz->bign)) {
	return jsi::Value(runtime, true);
      } else {
	return jsi::Value(runtime, false);
      }
    }));

  this->fields.push_back(HOST_LAMBDA("isOdd", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      if (BN_is_odd(thiz->bign)) {
	return jsi::Value(runtime, true);
      } else {
	return jsi::Value(runtime, false);
      }
    }));

  this->fields.push_back(HOST_LAMBDA("isEven", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      if (!BN_is_odd(thiz->bign)) {
	return jsi::Value(runtime, true);
      } else {
	return jsi::Value(runtime, false);
      }
    }));

  this->fields.push_back(HOST_LAMBDA("iabs", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      BN_set_negative(thiz->bign, 0);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("abs", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_copy(res->bign, thiz->bign);
      BN_set_negative(res->bign, 0);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("ineg", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      BigNumHelper::BN_smart_neg(thiz->bign);

      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("neg", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_copy(res->bign, thiz->bign);
      BigNumHelper::BN_smart_neg(res->bign);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("ipow", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "ipow expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "ipow expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
      BN_exp(thiz->bign, thiz->bign, other->bign, thiz->ctx);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("pow", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "pow expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "pow expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_exp(res->bign, thiz->bign, other->bign, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("cmp", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      return jsi::Value(runtime, BN_cmp(thiz->bign, other->bign));
    }));

    this->fields.push_back(HOST_LAMBDA("cmpn", {
        std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "cmpn expects number");
        }
        int other = otherValue.asNumber();

        BIGNUM * temp = BN_new();
        BN_set_word(temp, abs(other));
        if (other < 0) {
            BN_set_negative(temp, 1);
        }

        int res = BN_cmp(thiz->bign, temp);
        BN_free(temp);

        return jsi::Value(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("lt", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      return jsi::Value(runtime, BN_cmp(thiz->bign, other->bign) < 0);
    }));

  this->fields.push_back(HOST_LAMBDA("lte", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      return jsi::Value(runtime, BN_cmp(thiz->bign, other->bign) <= 0);
    }));

  this->fields.push_back(HOST_LAMBDA("gt", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      return jsi::Value(runtime, BN_cmp(thiz->bign, other->bign) > 0);
    }));

  this->fields.push_back(HOST_LAMBDA("gte", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      return jsi::Value(runtime, BN_cmp(thiz->bign, other->bign) >= 0);
    }));

  this->fields.push_back(HOST_LAMBDA("eq", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "cmp expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      return jsi::Value(runtime, BN_cmp(thiz->bign, other->bign) == 0);
    }));

  this->fields.push_back(HOST_LAMBDA("ucmp", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "ucmp expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "ucmp expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      return jsi::Value(runtime, BN_ucmp(thiz->bign, other->bign));
    }));

  this->fields.push_back(HOST_LAMBDA("imod", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "imod expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "imod expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
      BN_div(nullptr, thiz->bign, thiz->bign, other->bign, thiz->ctx);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("mod", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "mod expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "mod expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_div(nullptr, res->bign, thiz->bign, other->bign, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("umod", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "umod expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "umod expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_nnmod(res->bign, thiz->bign, other->bign, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("invm", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "invm expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "invm expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_mod_inverse(res->bign, thiz->bign, other->bign, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("igcd", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "igcd expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "igcd expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      BN_gcd(thiz->bign, thiz->bign, other->bign, thiz->ctx);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("gcd", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "gcd expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "gcd expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BN_gcd(res->bign, thiz->bign, other->bign, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("ior", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "ior expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "ior expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      BigNumHelper::BN_or(thiz->bign, thiz->bign, other->bign);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("or", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "or expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "or expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BigNumHelper::BN_or(res->bign, thiz->bign, other->bign);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("iand", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "iand expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "iand expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      BigNumHelper::BN_and(thiz->bign, thiz->bign, other->bign);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("and", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "and expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "and expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);

      BigNumHelper::BN_and(res->bign, thiz->bign, other->bign);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("ixor", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "ixor expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "ixor expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      BigNumHelper::BN_xor(thiz->bign, thiz->bign, other->bign);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("xor", {
      std::shared_ptr<BigNumber> thiz = thisValue.getObject(runtime).getHostObject<BigNumber>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "xor expects BigNumb");
      }
      jsi::Object otherObject = otherValue.asObject(runtime);
      if (!otherObject.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "xor expects BigNumb");
      }
      std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

      std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(thiz->ctx);
      BigNumHelper::BN_xor(res->bign, thiz->bign, other->bign);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  // RedBigNum
  this->fields.push_back(HOST_LAMBDA("redAdd", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "redAdd expects RedBigNum");
      }
      jsi::Object obj = otherValue.getObject(runtime);
      if (!obj.isHostObject<RedBigNum>(runtime)) {
	throw jsi::JSError(runtime, "redAdd expects RedBigNum");
      }

      std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);

      std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(thiz->ctx, thiz->mctx, thiz->m);
      BN_mod_add_quick(res->bign, thiz->bign, other->bign, thiz->m);           // TODO (Szymon) are we sure that values are less than m?
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("redIAdd", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "redIAdd expects RedBigNum");
      }
      jsi::Object obj = otherValue.getObject(runtime);
      if (!obj.isHostObject<RedBigNum>(runtime)) {
	throw jsi::JSError(runtime, "redIAdd expects RedBigNum");
      }

      std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);
      BN_mod_add_quick(thiz->bign, thiz->bign, other->bign, thiz->m);           // TODO (Szymon) are we sure that values are less than m?
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("redSub", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "redSub expects RedBigNum");
      }
      jsi::Object obj = otherValue.getObject(runtime);
      if (!obj.isHostObject<RedBigNum>(runtime)) {
	throw jsi::JSError(runtime, "redSub expects RedBigNum");
      }

      std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);

      std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(thiz->ctx, thiz->mctx, thiz->m);
      BN_mod_sub_quick(res->bign, thiz->bign, other->bign, thiz->m);           // TODO (Szymon) are we sure that values are less than m?
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("redISub", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "redISub expects RedBigNum");
      }
      jsi::Object obj = otherValue.getObject(runtime);
      if (!obj.isHostObject<RedBigNum>(runtime)) {
	throw jsi::JSError(runtime, "redISub expects RedBigNum");
      }

      std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);
      BN_mod_sub_quick(thiz->bign, thiz->bign, other->bign, thiz->m);           // TODO (Szymon) are we sure that values are less than m?
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("redMul", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "redMul expects RedBigNum");
      }
      jsi::Object obj = otherValue.getObject(runtime);
      if (!obj.isHostObject<RedBigNum>(runtime)) {
	throw jsi::JSError(runtime, "redMul expects RedBigNum");
      }

      std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);

      std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(thiz->ctx, thiz->mctx, thiz->m);
      BN_mod_mul_montgomery(res->bign, thiz->bign, other->bign, thiz->mctx, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("cloneRed", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(thiz->ctx, thiz->mctx, thiz->m);
      BN_copy(res->bign, thiz->bign);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("isRedZero", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      return jsi::Value(runtime, BN_is_zero(thiz->bign) ? true : false );
    }));

  this->fields.push_back(HOST_LAMBDA("redIMul", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "redIMul expects RedBigNum");
      }
      jsi::Object obj = otherValue.getObject(runtime);
      if (!obj.isHostObject<RedBigNum>(runtime)) {
	throw jsi::JSError(runtime, "redIMul expects RedBigNum");
      }

      std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);

      BN_mod_mul_montgomery(thiz->bign, thiz->bign, other->bign, thiz->mctx, thiz->ctx);
      return jsi::Value::undefined();
    }));

    this->fields.push_back(HOST_LAMBDA("redCmp", {
        std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "redIMul expects RedBigNum");
        }
        jsi::Object obj = otherValue.getObject(runtime);
        if (!obj.isHostObject<RedBigNum>(runtime)) {
            throw jsi::JSError(runtime, "redIMul expects RedBigNum");
        }

        std::shared_ptr<RedBigNum> other = obj.getHostObject<RedBigNum>(runtime);
        return jsi::Value(runtime, (int)BN_cmp(thiz->bign, other->bign));
    }));

  this->fields.push_back(HOST_LAMBDA("redPow", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isObject()) {
	throw jsi::JSError(runtime, "redPow expects RedBigNum");
      }
      jsi::Object obj = otherValue.getObject(runtime);
      if (!obj.isHostObject<BigNumber>(runtime)) {
	throw jsi::JSError(runtime, "redPow expects RedBigNum");
      }

      std::shared_ptr<BigNumber> other = obj.getHostObject<BigNumber>(runtime);

      std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(thiz->ctx, thiz->mctx, thiz->m);

      BN_one(res->bign);
      BN_to_montgomery(res->bign, res->bign, thiz->mctx, thiz->ctx);
      BIGNUM * y = BN_dup(other->bign);

      BIGNUM * temp = BN_dup(thiz->bign);

      while (!BN_is_zero(y)) {
	if (BN_is_odd(y)) {
	  BN_mod_mul_montgomery(res->bign, res->bign, temp, thiz->mctx, thiz->ctx);
	  if (BN_is_one(y)) {
	    break;
	  }
	}
	BN_mod_mul_montgomery(temp, temp, temp, thiz->mctx, thiz->ctx);
	BN_rshift1(y, y);
      }

      BN_free(y);
      BN_free(temp);

      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("redInvm", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(thiz->ctx, thiz->mctx, thiz->m);

      BN_mod_inverse(res->bign, thiz->bign, thiz->m, thiz->ctx);
      BN_to_montgomery(res->bign, res->bign, thiz->mctx, thiz->ctx);
      BN_to_montgomery(res->bign, res->bign, thiz->mctx, thiz->ctx);

      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("redNeg", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(thiz->ctx, thiz->mctx, thiz->m);
      BN_mod_sub(res->bign, thiz->m, thiz->bign, thiz->m, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("redSqr", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(thiz->ctx, thiz->mctx, thiz->m);
      BN_mod_mul_montgomery(res->bign, thiz->bign, thiz->bign, thiz->mctx, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("redISqr", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      BN_mod_mul_montgomery(thiz->bign, thiz->bign, thiz->bign, thiz->mctx, thiz->ctx);
      return jsi::Value::undefined();
    }));

  this->fields.push_back(HOST_LAMBDA("redSqrt", {       // unfortunatelly may be slow
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(thiz->ctx, thiz->mctx, thiz->m);

      BIGNUM * temp = BN_new();
      BN_from_montgomery(temp, thiz->bign, thiz->mctx, thiz->ctx);

      BN_mod_sqrt(temp, temp, thiz->m, thiz->ctx);
      BN_to_montgomery(res->bign, temp, thiz->mctx, thiz->ctx);

      BN_free(temp);
      return jsi::Object::createFromHostObject(runtime, res);
    }));

  this->fields.push_back(HOST_LAMBDA("redShl", {
      std::shared_ptr<RedBigNum> thiz = thisValue.getObject(runtime).getHostObject<RedBigNum>(runtime);
      const jsi::Value & otherValue = arguments[0];
      if (!otherValue.isNumber()) {
	throw jsi::JSError(runtime, "redShl expects number");
      }
      unsigned int number = (unsigned int) otherValue.asNumber();
      std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(thiz->ctx, thiz->mctx, thiz->m);
      BN_mod_lshift(res->bign, thiz->bign, number, thiz->m, thiz->ctx);
      return jsi::Object::createFromHostObject(runtime, res);
    }));
}

}  // namespace margelo
