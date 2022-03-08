//
// Created by Szymon on 08/03/2022.
//

#include "BigNumber.h"

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
    if (base == 2) { // TODO(Szymon) fix negative numbers
        int needToAdd = (4 - (numberAsString.size() % 4)) % 4;
        while (needToAdd--) {
            numberAsString = "0" + numberAsString;
        }
        int n = numberAsString.size()/4 + 1;
        char * inHex = new char[n];
        inHex[n - 1] = '\0';
        for (int i = 0; i < n; ++i) {
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
        if (base == 2) {
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


    this->fields.push_back(HOST_LAMBDA("iadd", {
            const jsi::Value & otherValue = arguments[0];
            if (!otherValue.isObject()) {
                throw "iadd expects BigNumb";
            }
            jsi::Object otherObject = otherValue.asObject(runtime);
            if (!otherObject.isHostObject<BigNumber>(runtime)) {
                throw "iadd expects BigNumb";
            }
            std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);
            BN_add(this->bign, other->bign, this->bign);
    }));



    this->fields.push_back(HOST_LAMBDA("add", {
            const jsi::Value & otherValue = arguments[0];
            if (!otherValue.isObject()) {
                throw "iadd expects BigNumb";
            }
            jsi::Object otherObject = otherValue.asObject(runtime);
            if (!otherObject.isHostObject<BigNumber>(runtime)) {
                throw "iadd expects BigNumb";
            }
            std::shared_ptr<BigNumber> other = otherObject.getHostObject<BigNumber>(runtime);

            std::shared_ptr<BigNumber> res = std::make_shared<BigNumber>(this->ctx, this->weakJsCallInvoker.lock(), this->dispatchQueue);
            BN_add(this->bign, other->bign, res->bign);
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