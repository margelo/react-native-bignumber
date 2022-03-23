//
// Created by Szymon on 08/03/2022.
//

#include "BigNumber.h"
#include <android/log.h>
#include "Utils/BigNumHelper.h"
#include <cctype>
#define APPNAME "MyApp"

namespace margelo {

BigNumber::BigNumber(BN_CTX * ctx) {
  this->ctx = ctx;
  this->bign = BN_new();
  BN_zero(this->bign);
}

BigNumber::BigNumber(const unsigned char *s, int len, bool le, BN_CTX * ctx) {
  this->ctx = ctx;
  this->bign = BN_new();
  if (le) {
    BN_lebin2bn(s, len, this->bign);
  } else {
    BN_bin2bn(s, len, this->bign);
  }
}

BigNumber::BigNumber(std::string numberAsString, int base, BN_CTX * ctx) {
  this->ctx = ctx;
  this->bign = BN_new();

  if (base == 10) {
    BN_dec2bn(&(this->bign), numberAsString.c_str());
  } else if (base == 2) {
    bool negative = false;
    if (numberAsString.front() == '-') {
      negative = true;
      numberAsString.erase(0, 1);
    }
    int needToAdd = (4 - (numberAsString.size() % 4)) % 4;
    while (needToAdd--) {
      numberAsString = "0" + numberAsString;
    }
    int n = numberAsString.size() / 4 + 1;
    char *inHex = new char[n];
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

    delete[] inHex;
  } else if (base == 16) {
    BN_hex2bn(&(this->bign), numberAsString.c_str());
  }
}

BigNumber::BigNumber(int value, BN_CTX * ctx) {

  this->ctx = ctx;
  this->bign = BN_new();
  BN_set_word(this->bign, abs(value));
  if (value < 0) {
    BN_set_negative(this->bign, 1);
  }
}

BigNumber::BigNumber(const BigNumber & other) {
  this->ctx = other.ctx;
  BN_copy(this->bign, other.bign);

  //installMethods();
}

jsi::Value BigNumber::get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId) {
  std::string name = propNameId.utf8(runtime);
  if (name == "isInternalBigNum") {
    return jsi::Value(runtime, true);
  }
  return jsi::Value::undefined();
}

BigNumber::~BigNumber() {
  BN_free(this->bign);
}

} // namespace margelo