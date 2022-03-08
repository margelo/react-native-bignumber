// Copyright 2022 Margelo
#include "BigNumberHostObject.h"
#include <ReactCommon/TurboModuleUtils.h>
#include <jsi/jsi.h>
#include <vector>
#include <memory>
#include <openssl/bn.h>
#include <BigNumber/BigNumber.h>

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
		int val = arguments[0].asNumber();

		std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(val, BigNumberHostObject::bn_ctx,
																	 this->weakJsCallInvoker.lock(), this->dispatchQueue);
		return jsi::Object::createFromHostObject(runtime, res);
	}));

}

}  // namespace margelo
