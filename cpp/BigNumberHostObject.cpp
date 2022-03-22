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
		int base = 10;
		if (!arguments[1].isUndefined()) {
			base = arguments[1].asNumber();
		}

		std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(strRep, base, BigNumberHostObject::bn_ctx,
		        this->weakJsCallInvoker.lock(), this->dispatchQueue);
		return jsi::Object::createFromHostObject(runtime, res);
	}));

	this->fields.push_back(HOST_LAMBDA("createFromNumber", {
        int val = (int) arguments[0].asNumber();


		std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(val, BigNumberHostObject::bn_ctx,
																	 this->weakJsCallInvoker.lock(), this->dispatchQueue);
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

		std::shared_ptr<RedBigNum> res = std::make_shared<RedBigNum>(number->bign, BigNumberHostObject::bn_ctx, modCtx->mctx, modCtx->m, this->weakJsCallInvoker.lock(), this->dispatchQueue);
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

		std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(BigNumberHostObject::bn_ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
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
			constant = std::make_shared<BigNumber>(ModContext::k256.c_str(), 16, BigNumberHostObject::bn_ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
		}
		if (prime == "p224") {
			constant = std::make_shared<BigNumber>(ModContext::p224.c_str(), 16, BigNumberHostObject::bn_ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
		}
		if (prime == "p192") {
			constant = std::make_shared<BigNumber>(ModContext::p192.c_str(), 16, BigNumberHostObject::bn_ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
		}
		if (prime == "p25519") {
			constant = std::make_shared<BigNumber>(ModContext::p25519.c_str(), 16, BigNumberHostObject::bn_ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
		}

		int bitLen = BN_num_bits(constant->bign);

		BIGNUM * temp = BN_new();
		BN_zero(temp);
		BN_set_bit(temp, bitLen);

		BN_sub(temp, temp, constant->bign);
		std::shared_ptr<ModContext> res = std::make_shared<ModContext>(temp, constant->ctx);

		BN_free(temp);
		return jsi::Object::createFromHostObject(runtime, res);
	}));
}

}  // namespace margelo
