//
// Created by Szymon on 08/03/2022.
//

#include "MGBigNumber.h"
#include "MGBigNumHelper.h"
#include <cctype>
#define APPNAME "MyApp"

namespace margelo {

MGBigNumber::MGBigNumber(BN_CTX * ctx) {
  this->ctx = ctx;
  this->bign = BN_new();
  BN_zero(this->bign);
}

MGBigNumber::MGBigNumber(const unsigned char *s, int len, bool le, BN_CTX * ctx) {
  this->ctx = ctx;
  this->bign = BN_new();
  if (le) {
    BN_lebin2bn(s, len, this->bign);
  } else {
    BN_bin2bn(s, len, this->bign);
  }
}

MGBigNumber::MGBigNumber(std::string numberAsString, int base, BN_CTX * ctx) {
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

MGBigNumber::MGBigNumber(int value, BN_CTX * ctx) {

  this->ctx = ctx;
  this->bign = BN_new();
  BN_set_word(this->bign, abs(value));
  if (value < 0) {
    BN_set_negative(this->bign, 1);
  }
}

MGBigNumber::MGBigNumber(const MGBigNumber & other) {
  this->ctx = other.ctx;
  BN_copy(this->bign, other.bign);

  //installMethods();
}

jsi::Value MGBigNumber::get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId) {
  std::string name = propNameId.utf8(runtime);
  if (name == "isInternalBigNum") {
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
    jsi::String res = jsi::String::createFromUtf8(runtime, "[object MGBigNumber]");
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

std::vector <jsi::PropNameID> MGBigNumber::getPropertyNames(
  jsi::Runtime &runtime) {
  std::vector <jsi::PropNameID> propertyNames;
  
  propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "isInternalBigNum"));
  propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "toString"));
  propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "toStringTag"));
  propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "value"));
  return propertyNames;
}

MGBigNumber::~MGBigNumber() {
  BN_free(this->bign);
}

} // namespace margelo
