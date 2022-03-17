//
// Created by Szymon on 08/03/2022.
//

#include "BigNumber.h"
#include <android/log.h>
#include "Utils/BigNumHelper.h"
#define APPNAME "MyApp"

namespace margelo {

BigNumber::BigNumber(BN_CTX * ctx, std::shared_ptr<react::CallInvoker> jsCallInvoker,
    std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue):
    SmartHostObject(jsCallInvoker, workerQueue) {

    this->ctx = ctx;
    this->bign = BN_new();
    BN_zero(this->bign);

    installMethods();
}

BigNumber::BigNumber(std::string numberAsString, int base, BN_CTX * ctx, std::shared_ptr<react::CallInvoker> jsCallInvoker,
            std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue):
    SmartHostObject(jsCallInvoker, workerQueue) {

    this->ctx = ctx;
    this->bign = BN_new();

    if (base == 10) {
        BN_dec2bn(&(this->bign), numberAsString.c_str());
    }
    if (base == 2) {
        bool negative = false;
        if (numberAsString.front() == '-') {
            negative = true;
            numberAsString.erase(0, 1);
        }
        int needToAdd = (4 - (numberAsString.size() % 4)) % 4;
        while (needToAdd--) {
            numberAsString = "0" + numberAsString;
        }
        int n = numberAsString.size()/4 + 1;
        char * inHex = new char[n];
        inHex[n - 1] = '\0';
        for (int i = 0; i < n - 1; ++i) {
            int value = 0;
            int beginning = i * 4;
            for (int j = 0; j < 4; ++j) {
                value *= 2;
                if (numberAsString[beginning + j] == '1') {
                    value++;
                }
            }
            if (value <= 9) {
                inHex[i] = '0' + value;
            } else {
                inHex[i] = 'a' + value - 10;
            }
        }

        BN_hex2bn(&(this->bign), inHex);

        if (negative) {
            BN_set_negative(this->bign, 1);
        }

        delete [] inHex;
    }
    if (base == 16) {
        BN_hex2bn(&(this->bign), numberAsString.c_str());
    }

    installMethods();
}

BigNumber::BigNumber(int value, BN_CTX * ctx, std::shared_ptr<react::CallInvoker> jsCallInvoker,
                   std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue):
        SmartHostObject(jsCallInvoker, workerQueue) {

    this->ctx = ctx;
    this->bign = BN_new();
    BN_set_word(this->bign, value);

    installMethods();
}

BigNumber::BigNumber(const BigNumber & other): SmartHostObject(other.weakJsCallInvoker.lock(), other.dispatchQueue) {
    this->ctx = other.ctx;
    BN_copy(this->bign, other.bign);

    installMethods();
}

void BigNumber::installMethods() {

    this->fields.push_back(HOST_LAMBDA("toString", {
        int base = (int) arguments[0].asNumber();
        int len = -1;
        if (!arguments[1].isUndefined()) {
            len = (int) arguments[1].asNumber();
        }

        char *strRep = nullptr;
        if (base == 2) { // TODO (Szymon)
            throw jsi::JSError(runtime, "BN to bin hasn't been implemented yet! :(");
        }
        if (base == 10) {
            strRep = BN_bn2dec(this->bign);
            if (len == -1) {
                return jsi::String::createFromAscii(runtime, strRep);
            }
            int sizeOfRep = strlen(strRep);
            std::string res(len, '0');
            for (int i = 0; i < std::min(len, sizeOfRep); ++i) {
                char dig = strRep[sizeOfRep - 1 - i];
                res[len - 1 - i] = dig;
            }
            return jsi::String::createFromAscii(runtime, res.c_str());
        }
        if (base == 16) {
            strRep = BN_bn2hex(this->bign);
            if (len == -1) {
                return jsi::String::createFromAscii(runtime, strRep);
            }
            int sizeOfRep = strlen(strRep);
            std::string res(len, '0');
            for (int i = 0; i < std::min(len, sizeOfRep); ++i) {
                char dig = strRep[sizeOfRep - 1 - i];
                res[len - 1 - i] = dig;
            }
            return jsi::String::createFromAscii(runtime, res.c_str());
        }
    }));

    // with number

    this->fields.push_back(HOST_LAMBDA("iaddn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "iaddn expects integer");
        }
        unsigned int other = otherValue.asNumber();

        BN_add_word(this->bign, other);
    }));

    this->fields.push_back(HOST_LAMBDA("addn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "iaddn expects integer");
        }
        unsigned int other = otherValue.asNumber();

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        BN_add_word(res->bign, other);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("isubn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "isubn expects integer");
        }
        unsigned int other = otherValue.asNumber();

        BN_sub_word(this->bign, other);
    }));

    this->fields.push_back(HOST_LAMBDA("subn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "subn expects integer");
        }
        unsigned int other = otherValue.asNumber();

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        BN_sub_word(res->bign, other);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("imuln", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "imuln expects integer");
        }
        unsigned int other = otherValue.asNumber();

        BN_mul_word(this->bign, other);
    }));

    this->fields.push_back(HOST_LAMBDA("muln", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "muln expects integer");
        }
        unsigned int other = otherValue.asNumber();

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        BN_mul_word(res->bign, other);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("idivn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "idivn expects integer");
        }
        unsigned int other = otherValue.asNumber();

        BN_div_word(this->bign, other);
    }));

    this->fields.push_back(HOST_LAMBDA("divn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "divn expects integer");
        }
        unsigned int other = otherValue.asNumber();

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        BN_div_word(res->bign, other);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("imodn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "imodn expects integer");
        }
        unsigned int other = otherValue.asNumber();

        BN_mod_word(this->bign, other);
    }));

    this->fields.push_back(HOST_LAMBDA("modn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "modn expects integer");
        }
        unsigned int other = otherValue.asNumber();

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        BN_mod_word(res->bign, other);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("setn", {
        const jsi::Value & indexValue = arguments[0];
        const jsi::Value & newValue = arguments[1];
        if (!indexValue.isNumber() || !newValue.isNumber()) {
            throw jsi::JSError(runtime, "setn expects 2 integers");
        }
        int value = newValue.asNumber();
        unsigned int index = indexValue.asNumber();

        if (value) {
            BN_set_bit(this->bign, index);
        } else {
            BN_clear_bit(this->bign, index);
        }
    }));

    this->fields.push_back(HOST_LAMBDA("testn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "testn expects integer");
        }
        unsigned int other = otherValue.asNumber();

        return jsi::Value(runtime, BN_is_bit_set(this->bign, other) != 0);
    }));

    this->fields.push_back(HOST_LAMBDA("bincn", { // Can be optimised with low level API
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "testn expects integer");
        }
        unsigned int value = otherValue.asNumber();
        std::shared_ptr<BigNumber> temp = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);

        BN_zero(temp->bign);
        BN_set_bit(temp->bign, value);
        BN_add(this->bign, temp->bign, this->bign);
    }));

    this->fields.push_back(HOST_LAMBDA("inotn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "testn expects integer");
        }
        unsigned int len = otherValue.asNumber();

        BigNumHelper::BN_notn(this->bign, len);
    }));

    this->fields.push_back(HOST_LAMBDA("imaskn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "imaskn expects integer");
        }
        unsigned int other = otherValue.asNumber();
        BN_mask_bits(this->bign, other);
    }));

    this->fields.push_back(HOST_LAMBDA("notn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "notn expects integer");
        }
        unsigned int len = otherValue.asNumber();
        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        BigNumHelper::BN_notn(res->bign, len);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("maskn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "maskn expects integer");
        }
        unsigned int other = otherValue.asNumber();
        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        BN_mask_bits(res->bign, other);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("ishln", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "ishln expects integer");
        }
        unsigned int other = otherValue.asNumber();
        BN_lshift(this->bign, this->bign, other);
    }));

    this->fields.push_back(HOST_LAMBDA("shln", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "shln expects integer");
        }
        unsigned int other = otherValue.asNumber();
        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_lshift(res->bign, this->bign, other);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("ishrn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "ishrn expects integer");
        }
        unsigned int other = otherValue.asNumber();
        BN_rshift(this->bign, this->bign, other);
    }));

    this->fields.push_back(HOST_LAMBDA("shrn", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isNumber()) {
            throw jsi::JSError(runtime, "shrn expects integer");
        }
        unsigned int other = otherValue.asNumber();
        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_rshift(res->bign, this->bign, other);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    // with BigNumber

    this->fields.push_back(HOST_LAMBDA("iadd", {
            const jsi::Value & otherValue = arguments[0];
            if (!otherValue.isObject()) {
                throw jsi::JSError(runtime, "iadd expects BigNumb");
            }
            jsi::Object otherObject = otherValue.asObject(runtime);
            if (!otherObject.isHostObject<BigNumber>(runtime)) {
                throw jsi::JSError(runtime, "iadd expects BigNumb");
            }
            std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
            BN_add(this->bign, other->bign, this->bign);
    }));

    this->fields.push_back(HOST_LAMBDA("add", {
            const jsi::Value & otherValue = arguments[0];
            if (!otherValue.isObject()) {
                throw jsi::JSError(runtime, "add expects BigNumb");
            }
            jsi::Object otherObject = otherValue.asObject(runtime);
            if (!otherObject.isHostObject<BigNumber>(runtime)) {
                throw jsi::JSError(runtime, "add expects BigNumb");
            }
            std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

            std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
            BN_add(res->bign, other->bign, this->bign);
            return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("isub", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "isub expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "isub expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
        BN_sub(this->bign, this->bign, other->bign);
    }));

    this->fields.push_back(HOST_LAMBDA("sub", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "sub expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "sub expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_sub(res->bign, this->bign, other->bign);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("imul", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "imul expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "imul expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
        BN_mul(this->bign, other->bign, this->bign, this->ctx);
    }));

    this->fields.push_back(HOST_LAMBDA("mul", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "mul expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "mul expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mul(res->bign, other->bign, this->bign, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("idiv", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "idiv expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "idiv expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
        BN_div(this->bign, nullptr, this->bign, other->bign, this->ctx);
    }));

    this->fields.push_back(HOST_LAMBDA("div", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "div expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "div expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_div(res->bign, nullptr, this->bign, other->bign, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("divmod", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "divmod expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "divmod expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        std::shared_ptr<BigNumber> rem = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_div(res->bign, rem->bign, this->bign, other->bign, this->ctx);
        jsi::Object obj(runtime);
        obj.setProperty(runtime, "div", jsi::Object::createFromHostObject(runtime, res));
        obj.setProperty(runtime, "mod", jsi::Object::createFromHostObject(runtime, rem));
        return obj;
    }));

    this->fields.push_back(HOST_LAMBDA("divRound", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "divRound expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "divRound expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        std::shared_ptr<BigNumber> rem = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);

        if (BN_is_zero(res->bign)) {
            return jsi::Object::createFromHostObject(runtime, res);
        }

        std::shared_ptr<BigNumber> temp = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_div(res->bign, rem->bign, this->bign, other->bign, this->ctx);
        BN_sub(temp->bign, other->bign, rem->bign);
        if (BN_cmp(rem->bign, temp->bign) <= 0) {
            return jsi::Object::createFromHostObject(runtime, res);
        } else {
            BN_add_word(res->bign, 1);
            return jsi::Object::createFromHostObject(runtime, res);
        }
    }));

    this->fields.push_back(HOST_LAMBDA("isqr", {
        BN_sqr(this->bign, this->bign, this->ctx);
    }));

    this->fields.push_back(HOST_LAMBDA("sqr", {
        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_sqr(res->bign, this->bign, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("isZero", {
        if (BN_zero(this->bign)) {
            return jsi::Value(runtime, true);
        } else {
            return jsi::Value(runtime, false);
        }
    }));

    this->fields.push_back(HOST_LAMBDA("clone", {
        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("isOne", {
        if (BN_one(this->bign)) {
            return jsi::Value(runtime, true);
        } else {
            return jsi::Value(runtime, false);
        }
    }));

    this->fields.push_back(HOST_LAMBDA("bitLength", {
        return jsi::Value(runtime, BN_num_bits(this->bign));
    }));

    this->fields.push_back(HOST_LAMBDA("byteLength", {
        return jsi::Value(runtime, BN_num_bytes(this->bign));
    }));

    this->fields.push_back(HOST_LAMBDA("isNeg", {
        if (BN_is_negative(this->bign)) {
            return jsi::Value(runtime, true);
        } else {
            return jsi::Value(runtime, false);
        }
    }));

    this->fields.push_back(HOST_LAMBDA("isOdd", {
        if (BN_is_odd(this->bign)) {
            return jsi::Value(runtime, true);
        } else {
            return jsi::Value(runtime, false);
        }
    }));

    this->fields.push_back(HOST_LAMBDA("isEven", {
        if (!BN_is_odd(this->bign)) {
            return jsi::Value(runtime, true);
        } else {
            return jsi::Value(runtime, false);
        }
    }));

    this->fields.push_back(HOST_LAMBDA("iabs", {
        BN_set_negative(this->bign, 0);
    }));

    this->fields.push_back(HOST_LAMBDA("abs", {
        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        BN_set_negative(res->bign, 0);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("ineg", {
        BN_set_negative(this->bign, 1);
    }));

    this->fields.push_back(HOST_LAMBDA("neg", {
        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_copy(res->bign, this->bign);
        BN_set_negative(res->bign, 1);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("ipow", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "ipow expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "ipow expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
        BN_exp(this->bign, this->bign, other->bign, this->ctx);
    }));

    this->fields.push_back(HOST_LAMBDA("pow", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "pow expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "pow expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_exp(res->bign, this->bign, other->bign, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("cmp", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        return jsi::Value(runtime, BN_cmp(this->bign, other->bign));
    }));

    this->fields.push_back(HOST_LAMBDA("lt", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        return jsi::Value(runtime, BN_cmp(this->bign, other->bign) < 0);
    }));

    this->fields.push_back(HOST_LAMBDA("lte", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        return jsi::Value(runtime, BN_cmp(this->bign, other->bign) <= 0);
    }));

    this->fields.push_back(HOST_LAMBDA("gt", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        return jsi::Value(runtime, BN_cmp(this->bign, other->bign) > 0);
    }));

    this->fields.push_back(HOST_LAMBDA("gte", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        return jsi::Value(runtime, BN_cmp(this->bign, other->bign) >= 0);
    }));

    this->fields.push_back(HOST_LAMBDA("eq", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "cmp expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        return jsi::Value(runtime, BN_cmp(this->bign, other->bign) == 0);
    }));

    this->fields.push_back(HOST_LAMBDA("ucmp", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "ucmp expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "ucmp expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        return jsi::Value(runtime, BN_ucmp(this->bign, other->bign));
    }));

    this->fields.push_back(HOST_LAMBDA("imod", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "imod expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "imod expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
        BN_div(nullptr, this->bign, this->bign, other->bign, this->ctx);
    }));

    this->fields.push_back(HOST_LAMBDA("mod", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "mod expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "mod expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_div(nullptr, res->bign, this->bign, other->bign, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("umod", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "umod expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "umod expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_nnmod(res->bign, this->bign, other->bign, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("invm", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "invm expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "invm expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_mod_inverse(res->bign, this->bign, other->bign, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("igcd", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "igcd expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "igcd expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        BN_gcd(this->bign, this->bign, other->bign, this->ctx);
    }));

    this->fields.push_back(HOST_LAMBDA("gcd", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "gcd expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "gcd expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BN_gcd(res->bign, this->bign, other->bign, this->ctx);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("ior", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "ior expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "ior expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        BigNumHelper::BN_or(this->bign, this->bign, other->bign);
    }));

    this->fields.push_back(HOST_LAMBDA("or", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "or expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "or expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BigNumHelper::BN_or(res->bign, this->bign, other->bign);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("iand", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "iand expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "iand expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        BigNumHelper::BN_and(this->bign, this->bign, other->bign);
    }));

    this->fields.push_back(HOST_LAMBDA("and", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "and expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "and expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BigNumHelper::BN_and(res->bign, this->bign, other->bign);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(HOST_LAMBDA("ixor", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "ixor expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "ixor expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        BigNumHelper::BN_xor(this->bign, this->bign, other->bign);
    }));

    this->fields.push_back(HOST_LAMBDA("xor", {
        const jsi::Value & otherValue = arguments[0];
        if (!otherValue.isObject()) {
            throw jsi::JSError(runtime, "xor expects BigNumb");
        }
        jsi::Object otherObject = otherValue.asObject(runtime);
        if (!otherObject.isHostObject<BigNumber>(runtime)) {
            throw jsi::JSError(runtime, "xor expects BigNumb");
        }
        std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

        std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
        BigNumHelper::BN_xor(res->bign, this->bign, other->bign);
        return jsi::Object::createFromHostObject(runtime, res);
    }));

    this->fields.push_back(JSI_VALUE("isInternalBigNum", {
        return jsi::Value(runtime, true);
    }));
}

BigNumber::~BigNumber() {
    BN_free(this->bign);
}

} // namespace margelo