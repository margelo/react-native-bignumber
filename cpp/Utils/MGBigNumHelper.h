//
// Created by Szymon on 16/03/2022.
//

#ifndef BIGNUMBEREXAMPLE_MGBIGNUMHELPER_H
#define BIGNUMBEREXAMPLE_MGBIGNUMHELPER_H

#include <openssl/ossl_typ.h>
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/sha.h>
#include <openssl/lhash.h>
#include <openssl/opensslconf.h>
#include <openssl/crypto.h>

namespace margelo {

class MGBigNumHelper {
public:
static void BN_xor(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);

static void BN_and(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);

static void BN_or(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);

static void BN_notn(BIGNUM *pSt, unsigned int len);

static void EGDC(BIGNUM * x, BIGNUM * y, BIGNUM *g, const BIGNUM *a, const BIGNUM *b);

static void BN_smart_neg(BIGNUM *pSt);

static std::string bn2Str(BIGNUM * num, int base, int len);
    
static int charToInt(char & a);

// static void bn_correct_top(BIGNUM *pSt);
};
} // namespace margelo

#endif //BIGNUMBEREXAMPLE_MGBIGNUMHELPER_H
