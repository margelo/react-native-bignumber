//
// Created by Szymon on 16/03/2022.
//

#include <algorithm>
#include "MGBigNumHelper.h"
#include <string>
#include <algorithm>
#define APPNAME "MyApp"

namespace margelo {

/* that implementation would be 32-64 times faster
   struct bignum_st_hack
   {
    BN_ULONG *d;    // Pointer to an array of 'BN_BITS2' bit chunks.
    int top;        //
    // The next are internal book keeping for bn_expand.
    int dmax;       // Size of the d array.
    int neg;       //  one if the number is negative
    int flags;
   };

   int MGBigNumHelper::BN_xor(BIGNUM *rr, const BIGNUM *aa, const BIGNUM *bb) {
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

   int MGBigNumHelper::BN_and(BIGNUM *r, const BIGNUM *a, const BIGNUM *b) {
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

   int MGBigNumHelper::BN_or(BIGNUM *r, const BIGNUM *a, const BIGNUM *b) {
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

   void MGBigNumHelper::BN_notn(BIGNUM *a, unsigned int len) {
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

   void MGBigNumHelper::bn_correct_top(BIGNUM *a) { // looks like it's not exported :(
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
   } */

//TODO try to do it more efficiently
//TODO probably it's better to start from most significant bits to avoid unnecessary expanding of nums

void MGBigNumHelper::BN_xor(BIGNUM *r, const BIGNUM *a, const BIGNUM *b) {
  int sizeA = BN_num_bits(a);
  int sizeB = BN_num_bits(b);
  if (sizeA < sizeB) {
    std::swap(a, b);
    std::swap(sizeA, sizeB);
  }
  for (int i = 0; i < sizeB; ++i) {
    int abit = BN_is_bit_set(a, i) ? 1 : 0;
    int bbit = BN_is_bit_set(b, i) ? 1 : 0;

    int val = abit ^ bbit;

    BN_set_bit(r, i);     // To expand
    if (!val) {
      BN_clear_bit(r, i);
    }
  }

  for (int i = sizeB; i < sizeA; ++i) {
    int val = BN_is_bit_set(a, i) ? 1 : 0;

    BN_set_bit(r, i);     // To expand
    if (!val) {
      BN_clear_bit(r, i);
    }
  }
}

void MGBigNumHelper::BN_and(BIGNUM *r, const BIGNUM *a, const BIGNUM *b) {
  int sizeA = BN_num_bits(a);
  int sizeB = BN_num_bits(b);
  if (sizeA < sizeB) {
    std::swap(a, b);
    std::swap(sizeA, sizeB);
  }
  for (int i = 0; i < sizeB; ++i) {
    int abit = BN_is_bit_set(a, i) ? 1 : 0;
    int bbit = BN_is_bit_set(b, i) ? 1 : 0;

    int val = abit & bbit;

    BN_set_bit(r, i);     // To expand
    if (!val) {
      BN_clear_bit(r, i);
    }
  }
  for (int i = sizeB; i < sizeA; ++i) {   // in case r is equal to a or b
    BN_set_bit(r, i);     // To expand
    BN_clear_bit(r, i);
  }
}

void MGBigNumHelper::BN_or(BIGNUM *r, const BIGNUM *a, const BIGNUM *b) {
  int sizeA = BN_num_bits(a);
  int sizeB = BN_num_bits(b);
  if (sizeA < sizeB) {
    std::swap(a, b);
    std::swap(sizeA, sizeB);
  }
  for (int i = 0; i < sizeB; ++i) {
    int abit = BN_is_bit_set(a, i) ? 1 : 0;
    int bbit = BN_is_bit_set(b, i) ? 1 : 0;

    int val = abit | bbit;

    BN_set_bit(r, i);     // To expand
    if (!val) {
      BN_clear_bit(r, i);
    }
  }

  for (int i = sizeB; i < sizeA; ++i) {
    int val = BN_is_bit_set(a, i) ? 1 : 0;

    BN_set_bit(r, i);     // To expand
    if (!val) {
      BN_clear_bit(r, i);
    }
  }
}

void MGBigNumHelper::BN_notn(BIGNUM *a, unsigned int len) {
  int sizeA = BN_num_bits(a);

  for (int i = 0; i < std::min(sizeA, (int)len); ++i) {
    int abit = BN_is_bit_set(a, i) ? 1 : 0;
    int val = 1 - abit;

    if (val) {
      BN_set_bit(a, i);
    } else {
      BN_clear_bit(a, i);
    }
  }

  for (int i = sizeA; i < len; ++i) {
    BN_set_bit(a, i);
  }
}

void MGBigNumHelper::BN_smart_neg(BIGNUM *pSt) {
  if (BN_is_negative(pSt)) {
    BN_set_negative(pSt, 0);
  } else {
    BN_set_negative(pSt, 1);
  }
}

std::string MGBigNumHelper::bn2Str(BIGNUM * num, int base, int len) {
  char *strRep = nullptr;
  if (base == 2) {
    bool negative = BN_is_negative(num);
    BN_set_negative(num, 0);
    char * hexRep = BN_bn2hex(num);
    if (negative) {
      BN_set_negative(num, 1);
    }
    int hexRepLen = strlen(hexRep);
    const int strRepLen = (negative) ? hexRepLen * 4 + 2 : hexRepLen * 4 + 1;
    strRep = new char[strRepLen];
    int offset = (int) negative;

    strRep[strRepLen - 1] = '\0';
    if (negative) {
      strRep[0] = '-';
    }

    for (int i = 0; i < hexRepLen; ++i) {
      int hexVal = static_cast<int>(hexRep[i] >= 'A' ? hexRep[i] - 'A' + 10 : hexRep[i] - '0');
      for (int bit = 0; bit < 4; ++bit) {
	char val = ((hexVal & (1 << bit)) > 0) ? '1' : '0';
	strRep[offset + i * 4 + (4 - bit - 1)] = val;
      }
    }
    delete [] hexRep;
  }
  if (base == 10) {
    strRep = BN_bn2dec(num);
  }
  if (base == 16) {
    strRep = BN_bn2hex(num);
  }
  int sizeOfRep = strlen(strRep);
  bool reduce = false;
  if (len == -1) {
    reduce = true;
    len = sizeOfRep;
  }

  if (BN_is_zero(num) && reduce) {
    return std::string("0");
  }

  std::string res(len, '0');
  for (int i = 0; i < std::min(len, sizeOfRep); ++i) {
    char dig = strRep[sizeOfRep - 1 - i];
    if (dig >= 'A' and dig <= 'F') {
      dig = dig - 'A' + 'a';
    }
    if (dig == '-') {
      res = "-" + res;
      break;
    }
    res[len - 1 - i] = dig;
  }

  int start = 0;
  if (res[0] == '-') {
    start = 1;
  }

  if (reduce) {
    int ptr = start;
    while (ptr < len && res[ptr] == '0') ptr++;
    res.erase(start, ptr - start);
  }

  delete [] strRep;
  return res;
}

void MGBigNumHelper::EGDC(BIGNUM * x, BIGNUM * y, BIGNUM *g, const BIGNUM *a, const BIGNUM *b) {
  //TODO (Szymon)
}

} // namespace margelo
