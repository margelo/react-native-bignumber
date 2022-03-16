//
// Created by Szymon on 16/03/2022.
//

#include "BigNumHelper.h"

struct bignum_st_hack
{
    BN_ULONG *d;    /* Pointer to an array of 'BN_BITS2' bit chunks. */
    int top;        /* Index of last used d +1. */
    /* The next are internal book keeping for bn_expand. */
    int dmax;       /* Size of the d array. */
    int neg;        /* one if the number is negative */
    int flags;
};

int BigNumHelper::BN_xor(BIGNUM *rr, const BIGNUM *aa, const BIGNUM *bb) {
    int i;
    const bignum_st_hack *at, *bt, *r = (bignum_st_hack*)rr, *a = (bignum_st_hack*)aa, *b = (bignum_st_hack*)bb;

    if (a->top < b->top) {
        at = b;
        bt = a;
    } else {
        at = a;
        bt = b;
    }

    if (bn_wexpand(r, at->top) == NULL)
        return 0;

    for (i = 0; i < bt->top; i++) {
        r->d[i] = at->d[i] ^ bt->d[i];
    }
    for (; i < at->top; i++) {
        r->d[i] = at->d[i];
    }

    r->top = at->top;
    bn_correct_top(r);

    return 1;
}

int BigNumHelper::BN_and(BIGNUM *r, const BIGNUM *a, const BIGNUM *b) {
    int i;
    const BIGNUM *at, *bt;

    if (a->top < b->top) {
        at = b;
        bt = a;
    } else {
        at = a;
        bt = b;
    }

    if (bn_wexpand(r, at->top) == NULL)
        return 0;

    for (i = 0; i < bt->top; i++) {
        r->d[i] = at->d[i] & bt->d[i];
    }
    for (; i < at->top; i++) {
        r->d[i] = at->d[i];
    }

    r->top = at->top;
    bn_correct_top(r);

    return 1;
}

int BigNumHelper::BN_or(BIGNUM *r, const BIGNUM *a, const BIGNUM *b) {
    int i;
    const BIGNUM *at, *bt;

    if (a->top < b->top) {
        at = b;
        bt = a;
    } else {
        at = a;
        bt = b;
    }

    if (bn_wexpand(r, at->top) == NULL)
        return 0;

    for (i = 0; i < bt->top; i++) {
        r->d[i] = at->d[i] | bt->d[i];
    }
    for (; i < at->top; i++) {
        r->d[i] = at->d[i];
    }

    r->top = at->top;
    bn_correct_top(r);

    return 1;
}

void BigNumHelper::BN_notn(BIGNUM *a, unsigned int len) {
    if (len == 0) return;
    len = (len - 1) / 32 + 1;
    if (len > a->top) {
        if (bn_wexpand(a, len) == NULL) {
            return;
        }
        a->top = len;
    }

    unsigned
    for (int i = 0; i < len; i++) {
        a->d[i] = ~(a->d[i]);
    }

    bn_correct_top(a);
}

void BigNumHelper::bn_correct_top(BIGNUM *a) { // looks like it's not exported :(
        BN_ULONG *ftl;
        int tmp_top = a->top;

        if (tmp_top > 0) {
            for (ftl = &(a->d[tmp_top]); tmp_top > 0; tmp_top--) {
                ftl--;
                if (*ftl != 0)
                    break;
            }
            a->top = tmp_top;
        }
        if (a->top == 0)
            a->neg = 0;
        a->flags &= ~BN_FLG_FIXED_TOP;
        // bn_pollute(a); Looks to be only used in debug
}
