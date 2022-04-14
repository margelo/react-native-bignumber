//
// Created by Szymon on 08/03/2022.
//

#include "MGBigNumber.h"
#include "MGBigNumHelper.h"
#include <cctype>

namespace margelo
{

    MGBigNumber::MGBigNumber(BN_CTX *ctx)
    {
        this->ctx = ctx;
        this->bign = BN_new();
        BN_zero(this->bign);
    }

    MGBigNumber::MGBigNumber(const unsigned char *s, int len, int base, bool le, BN_CTX *ctx)
    {
        this->ctx = ctx;
        this->bign = BN_new();

        if (base == 36)
        {
            if (le)
            {
                BN_zero(this->bign);
                for (int i = len - 1; i >= 0; --i)
                {
                    int value = s[i];
                    BN_mul_word(this->bign, 36);
                    BN_add_word(this->bign, value);
                }
            }
            else
            {
                BN_zero(this->bign);
                for (int i = 0; i < len; ++i)
                {
                    int value = s[i];
                    BN_mul_word(this->bign, 36);
                    BN_add_word(this->bign, value);
                }
            }
        }

        if (base == 16)
        {
            if (le)
            {
                BN_lebin2bn(s, len, this->bign);
            }
            else
            {
                BN_bin2bn(s, len, this->bign);
            }
        }
    }

    MGBigNumber::MGBigNumber(std::string numberAsString, int base, BN_CTX *ctx)
    {
        this->ctx = ctx;
        this->bign = BN_new();

        if (base == 10)
        {
            BN_dec2bn(&(this->bign), numberAsString.c_str());
        }
        else if (base == 2)
        {
            // openSSL doesnt have method that can parse '010110'(binary) format
            // so I first change binary to hex then use BN_hex2bn to parse it
            // changing binary to hex is simple just simply concatenate the adjacent 4 bits and change to hex  |0010|1100 = 2C
            bool negative = false;
            if (numberAsString.front() == '-')
            {
                negative = true;
                numberAsString.erase(0, 1);
            }
            // Adding leading 0 so that it equals 0 mod 4
            int needToAdd = (4 - (numberAsString.size() % 4)) % 4;
            while (needToAdd--)
            {
                numberAsString = "0" + numberAsString;
            }
            int n = numberAsString.size() / 4 + 1;
            char *inHex = new char[n];
            inHex[n - 1] = '\0';
            for (int i = 0; i < n - 1; ++i)
            {
                int value = 0;
                int beginning = i * 4;
                for (int j = 0; j < 4; ++j)
                {
                    value *= 2;
                    if (numberAsString[beginning + j] == '1')
                    {
                        value++;
                    }
                }
                if (value <= 9)
                {
                    inHex[i] = '0' + value;
                }
                else
                {
                    inHex[i] = 'a' + value - 10;
                }
            }

            BN_hex2bn(&(this->bign), inHex);

            // set negative if input string had '-' at the beginning
            if (negative)
            {
                BN_set_negative(this->bign, 1);
            }

            delete[] inHex;
        }
        else if (base == 16)
        {
            BN_hex2bn(&(this->bign), numberAsString.c_str());
        }
        else if (base == 36)
        {
            bool negative = false;
            if (numberAsString.front() == '-')
            {
                negative = true;
                numberAsString.erase(0, 1);
            }
            BN_zero(this->bign);
            for (char c : numberAsString)
            {
                int value = c - '0';
                if (c >= 'A' && c <= 'Z')
                {
                    value = c - 'A' + 10;
                }
                if (c >= 'a' && c <= 'z')
                {
                    value = c - 'a' + 10;
                }
                BN_mul_word(this->bign, 36);
                BN_add_word(this->bign, value);
            }
            BN_set_negative(this->bign, negative);
        }
    }

    MGBigNumber::MGBigNumber(int value, BN_CTX *ctx)
    {

        this->ctx = ctx;
        this->bign = BN_new();
        BN_set_word(this->bign, abs(value));
        if (value < 0)
        {
            BN_set_negative(this->bign, 1);
        }
    }

    MGBigNumber::MGBigNumber(const MGBigNumber &other)
    {
        this->ctx = other.ctx;
        BN_copy(this->bign, other.bign);
    }

    jsi::Value MGBigNumber::get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId)
    {
        std::string name = propNameId.utf8(runtime);
        if (name == "isInternalBigNum")
        {
            return jsi::Value(runtime, true);
        }
        if (name == "toString")
        {
            char *str = BN_bn2dec(this->bign);
            std::string strRep(str);
            delete[] str;
            jsi::String res = jsi::String::createFromUtf8(runtime, strRep);
            return jsi::Value(runtime, res);
        }
        if (name == "toStringTag")
        {
            jsi::String res = jsi::String::createFromUtf8(runtime, "[object MGBigNumber]");
            return jsi::Value(runtime, res);
        }
        if (name == "value")
        {
            return jsi::Function::createFromHostFunction(runtime, propNameId, 0, [&](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *arguments, size_t count) -> jsi::Value
                                                         { return thisValue.getObject(runtime).getProperty(runtime, "toString"); });
        }
        return jsi::Value::undefined();
    }

    std::vector<jsi::PropNameID> MGBigNumber::getPropertyNames(
        jsi::Runtime &runtime)
    {
        std::vector<jsi::PropNameID> propertyNames;

        propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "isInternalBigNum"));
        propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "toString"));
        propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "toStringTag"));
        propertyNames.push_back(jsi::PropNameID::forAscii(runtime, "value"));
        return propertyNames;
    }

    void MGBigNumber::set(jsi::Runtime &runtime, const jsi::PropNameID &propNameId, const jsi::Value & value) {
        // do nothing;
    }


    MGBigNumber::~MGBigNumber()
    {
        BN_free(this->bign);
    }

} // namespace margelo
