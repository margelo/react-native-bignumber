//
// Created by Szymon on 16/03/2022.
//

#ifndef BIGNUMBEREXAMPLE_BIGNUMHELPER_H
#define BIGNUMBEREXAMPLE_BIGNUMHELPER_H

#include <openssl/ossl_typ.h>
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/sha.h>
#include <openssl/lhash.h>
#include <openssl/opensslconf.h>
#include <openssl/crypto.h>

class BigNumHelper {
public:
    static int BN_xor(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);
    static int BN_and(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);
    static int BN_or(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);

    static void BN_notn(BIGNUM *pSt, unsigned int len);

    static void bn_correct_top(BIGNUM *pSt);
};


#endif //BIGNUMBEREXAMPLE_BIGNUMHELPER_H
