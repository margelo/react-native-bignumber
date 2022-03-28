import {
  NativeBigNumber,
  InternalNumber,
  InternalModContext,
  InternalRedNumber,
} from './NativeBigNumber/NativeBigNumber';

const createFromString = NativeBigNumber.createFromString;
const createFromNumber = NativeBigNumber.createFromNumber;
const createFromArray = NativeBigNumber.createFromArray;
const createFromArrayBuffer = NativeBigNumber.createFromArrayBuffer;
const mod2bn = NativeBigNumber.mod2bn;
const bn2Mod = NativeBigNumber.bn2Mod;
const createModCtx = NativeBigNumber.createModCtx;
const createModCtxFromNumber = NativeBigNumber.createModCtxFromNumber;
const getPrimeContext = NativeBigNumber.getPrimeContext;
const forceCreateRed = NativeBigNumber.forceCreateRed;
const getPrime = NativeBigNumber.getPrime;

//bignumber
const {
  toArray,
  toString,
  add,
  iadd,
  iaddn,
  addn,

  sub,
  isub,
  isubn,
  subn,

  mul,
  imul,
  imuln,
  muln,

  div,
  idiv,
  idivn,
  divn,

  mod,
  imod,
  umod,
  imodn,
  modn,

  pow,
  ipow,

  setn,
  testn,
  bincn,
  inotn,
  notn,
  imaskn,
  maskn,
  ishln,
  shln,
  ishrn,
  shrn,

  divmod,
  divRound,

  sqr,
  isqr,
  abs,
  iabs,
  neg,
  ineg,

  isZero,
  isOne,
  clone,
  bitLength,
  byteLength,
  isNeg,
  isOdd,
  isEven,

  cmpn,
  cmp,
  ucmp,
  lt,
  lte,
  gt,
  gte,
  eq,

  invm,
  gcd,
  igcd,
  ior,
  or,
  iand,
  and,
  ixor,
  xor,
} = NativeBigNumber;

// redbignum
const {
  redAdd,
  redIAdd,
  redSub,
  redISub,
  redShl,
  redMul,
  redIMul,
  redSqr,
  redISqr,
  redSqrt,
  redInvm,
  redNeg,
  redPow,
  cloneRed,
  isRedZero,
  redCmp,
} = NativeBigNumber;

export class RedBigNumber {
  private internalRedBigNum: InternalRedNumber;
  private mctx: InternalModContext;


  constructor(bign: InternalNumber | InternalRedNumber, modCtx: InternalModContext) {
    if (bign?.isInternalBigNum) {
      this.internalRedBigNum = bn2Mod(bign, modCtx);
    } else {
      this.internalRedBigNum = bign as InternalRedNumber;
    }
   
    this.mctx = modCtx;
  }

  clone() {
    return new RedBigNumber(cloneRed.call(this.internalRedBigNum), this.mctx);
  }

  isZero() {
    return isRedZero.call(this.internalRedBigNum);
  }

  fromRed() {
    return new BN(mod2bn(this.internalRedBigNum));
  }

  redAdd(other: RedBigNumber) {
    return new RedBigNumber(redAdd.call(this.internalRedBigNum,other.internalRedBigNum), this.mctx);
  }

  redIAdd(other: RedBigNumber) {
    redIAdd.call(this.internalRedBigNum, other.internalRedBigNum);
    return this;
  }

  redSub(other: RedBigNumber) {
    return new RedBigNumber(redSub.call(this.internalRedBigNum, other.internalRedBigNum), this.mctx);
  }

  redISub(other: RedBigNumber) {
    redISub.call(this.internalRedBigNum, other.internalRedBigNum);
    return this;
  }

  redMul(other: RedBigNumber) {
    return new RedBigNumber(redMul.call(this.internalRedBigNum, other.internalRedBigNum), this.mctx);
  }

  redIMul(other: RedBigNumber) {
    redIMul.call(this.internalRedBigNum, other.internalRedBigNum);
    return this;
  }

  redShl(places: number) {
    return new RedBigNumber(redShl.call(this.internalRedBigNum, places), this.mctx);
  }

  redSqrt() {
    return new RedBigNumber(redSqrt.call(this.internalRedBigNum), this.mctx);
  }

  redSqr() {
    return new RedBigNumber(redSqr.call(this.internalRedBigNum), this.mctx);
  }

  redISqr() {
    redISqr.call(this.internalRedBigNum);
    return this;
  }

  redNeg() {
    return new RedBigNumber(redNeg.call(this.internalRedBigNum), this.mctx);
  }

  redInvm() {
    return new RedBigNumber(redInvm.call(this.internalRedBigNum), this.mctx);
  }

  redPow(other: BN) {
    return new RedBigNumber(redPow.call(this.internalRedBigNum, other.internalBigNum), this.mctx);
  }

  toString() {
    return this.fromRed().toString(10);
  }

  toNumber() {
    return parseInt(this.fromRed().toString(10, 53));
  }

  toArray() {
    return this.fromRed().toArray();
  }

  cmp(other: RedBigNumber) {
    return redCmp.call(this.internalRedBigNum, other.internalRedBigNum);
  }

  cmpn(other: number) {
    if (other == 0) {
      if (this.isZero()) {
        return 0;
      }
      return 1;
    }
    throw new Error('cmpn only supports 0 as an argument');
  }

  get red() {
    return true;
  }
}

export class BN {
  internalBigNum: InternalNumber;

  constructor();
  constructor(asString: string, base: 2 | 10 | 16, endian?: 'le' | 'be');
  constructor(asString: Array<number>, base?: 2 | 10 | 16, endian?: 'le' | 'be');
  constructor(asString: ArrayBuffer, base?: 2 | 10 | 16, endian?: 'le' | 'be');
  constructor(asString: Buffer, base?: 2 | 10 | 16, endian?: 'le' | 'be');
  constructor(value: number);
  constructor(in: InternalNumber);
  constructor(...args: any[]) {
    let base = 16;
    if (typeof args[1] == 'number') {
      base = args[1];
    }
    let endian = false;
    if (args[2] === 'le') {
      endian = true;
    }
    if (args[1] === 'le') {
      endian = true;
    }

    if (base === 36) {
      throw new Error("The library doesn't support base-36!");
    }

    if (args[0] == null) {
      this.internalBigNum = createFromNumber(0);
      return this;
    }
    if (typeof args[0] === 'string') {
      if (args[1] === 'hex') {
        args[1] = 16;
      }
      const start = performance.now();
      this.internalBigNum = createFromString(args[0], args[1]);
      return this;
    }
    if (typeof args[0] === 'number') {
      if (Math.abs(args[0]) > 1e9) {
        this.internalBigNum = createFromString(args[0].toString(), args[1]);
      } else {
        this.internalBigNum = createFromNumber(args[0]);
      }
      return this;
    }
    if ((typeof args[0] === 'object') && args[0].isInternalBigNum) {
      this.internalBigNum = args[0];
      return this;
    }

    if (args[0] instanceof BN) {
      this.internalBigNum = clone.call(args[0].internalBigNum);
      return this;
    }

    if (args[0] instanceof ArrayBuffer) {
      this.internalBigNum = createFromArrayBuffer(args[0], base, endian);
      return this;
    }

    if ((typeof args[0] === 'object') && args[0].buffer) { // Buffer
      var arrayBuffer = args[0].buffer.slice(
        args[0].byteOffset,
        args[0].byteOffset + args[0].byteLength
      );
      this.internalBigNum = createFromArrayBuffer(arrayBuffer, base, endian);
      return this;
    }

    if (Array.isArray(args[0])) {
      this.internalBigNum = createFromArray(args[0], base, endian);
      return this;
    }
    throw 'BN constructor got wrong params :(';
  }

  static _k256: InternalModContext | null = null;
  static _p225: InternalModContext | null = null;
  static _p192: InternalModContext | null = null;
  static _p25519: InternalModContext | null = null;

  static get k256(): InternalModContext  { // It it faster than simple prime map?
    if (!BN._k256) {
      BN._k256 = getPrimeContext('k256');
    }
    return BN._k256;
  }

  static get p225(): InternalModContext {
    if (!BN._p225) {
      BN._p225 = getPrimeContext('p225');
    }
    return BN._p225;
  }

  static get p192(): InternalModContext {
    if (!BN._p192) {
      BN._p192 = getPrimeContext('p192');
    }
    return BN._p192;
  }

  static get p25519(): InternalModContext {
    if (!BN._p25519) {
      BN._p25519 = getPrimeContext('p25519');
    }
    return BN._p25519;
  }

  static _prime(prime: string) {
    switch(prime) {
      case 'k256':
      case 'p224':
      case 'p192':
      case 'p25519':
        return {p: new BN(getPrime(prime))};
      default:
        throw new Error("Unknown prime number!");
    }
  }

  static red(num: string | BN | number): InternalModContext {
    if (typeof num === 'string') { // TODO (Szymon)
      switch(num) {
        case 'k256':
          return BN.k256;
        case 'p224':
          return BN.p225;
        case 'p192':
          return BN.p192;
        case 'p25519':
          return BN.p25519;
        default:
          throw new Error("Unknown prime number!");
      }
    }
    if (typeof num === 'number') {
      return createModCtxFromNumber(num);
    }
    return createModCtx(num.internalBigNum);
  }

  static mont(num: string | BN | number): InternalModContext {
    return BN.red(num);
  }

  static isBN(obj: any): boolean {
    return obj?.internalBigNum?.isInternalBigNum === true;
  }

  toRed(mctx: InternalModContext) {
    return new RedBigNumber(this.internalBigNum, mctx);
  }

  toString(base: 2 | 10 | 16, len?: number) {
    return toString.call(this.internalBigNum, base, len);
  }

  toNumber() {
    return parseInt(toString.call(this.internalBigNum, 10, 53));
  }

  toArray(endian?: 'le' | 'be', len?: number) {
    return toArray.call(this.internalBigNum, endian === 'le', len || -1);
  }

  toBuffer(endian?: 'le' | 'be', len?: number) {
    return Buffer.from(this.toArray(endian || 'be', len));
  }

  // add
  iadd(other: BN) {
    iadd.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  add(other: BN) {
    return new BN(add.call(this.internalBigNum, other.internalBigNum));
  }

  iaddn(other: number) {
    iaddn.call(this.internalBigNum, other);
    return this;
  }

  addn(other: number) {
    return new BN(addn.call(this.internalBigNum, other));
  }

  // sub
  isub(other: BN) {
    isub.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  sub(other: BN) {
    return new BN(sub.call(this.internalBigNum, other.internalBigNum));
  }

  isubn(other: number) {
    isubn.call(this.internalBigNum, other);
    return this;
  }

  subn(other: number) {
    return new BN(subn.call(this.internalBigNum, other));
  }

  // mul
  imul(other: BN) {
    imul.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  mul(other: BN) {
    return new BN(mul.call(this.internalBigNum, other.internalBigNum));
  }

  mulf(other: BN) {
    return new BN(mul.call(this.internalBigNum, other.internalBigNum));
  }

  imuln(other: number) {
    imuln.call(this.internalBigNum, other);
    return this;
  }

  muln(other: number) {
    return new BN(muln.call(this.internalBigNum, other));
  }

  // div
  idiv(other: BN) {
    idiv.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  div(other: BN) {
    return new BN(div.call(this.internalBigNum, other.internalBigNum));
  }

  idivn(other: number) {
    idivn.call(this.internalBigNum, other);
    return this;
  }

  divn(other: number) {
    return new BN(divn.call(this.internalBigNum, other));
  }

  divmod(m: BN) {
    return new BN(divmod.call(this.internalBigNum, m.internalBigNum));
  }

  divRound(m: BN) {
    return new BN(divRound.call(this.internalBigNum, m.internalBigNum));
  }

  // mod
  imod(other: BN) {
    imod.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  mod(other: BN) {
    return new BN(mod.call(this.internalBigNum, other.internalBigNum));
  }

  umod(other: BN) {
    return new BN(umod.call(this.internalBigNum, other.internalBigNum));
  }

  imodn(other: number) {
    imodn.call(this.internalBigNum, other);
    return this;
  }

  modn(other: number) {
    return new BN(modn.call(this.internalBigNum, other));
  }

  modrn(other: number) {
    return new BN(modn.call(this.internalBigNum, other));
  }

  // pow
  ipow(other: BN) {
    ipow.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  pow(other: BN) {
    return new BN(pow.call(this.internalBigNum, other.internalBigNum));
  }

  // bit manipulation
  setn(bit: number, value: boolean) {
    setn.call(this.internalBigNum, bit, value ? 1 : 0);
    return this;
  }

  testn(bit: number) {
    return testn.call(this.internalBigNum, bit);
  }

  bincn(bit: number) {
    bincn.call(this.internalBigNum, bit);
    return this;
  }

  notn(width: number) {
    return new BN(notn.call(this.internalBigNum, width));
  }

  inotn(width: number) {
    inotn.call(this.internalBigNum, width);
    return this;
  }

  maskn(width: number) {
    return new BN(maskn.call(this.internalBigNum, width));
  }

  imaskn(width: number) {
    imaskn.call(this.internalBigNum, width);
    return this;
  }

  shln(width: number) {
    return new BN(shln.call(this.internalBigNum, width));
  }

  ushln(width: number) {
    return new BN(shln.call(this.internalBigNum, width));
  }

  ishln(width: number) {
    ishln.call(this.internalBigNum, width);
    return this;
  }

  iushln(width: number) {
    ishln.call(this.internalBigNum, width);
    return this;
  }

  shrn(width: number) {
    return new BN(shrn.call(this.internalBigNum, width));
  }

  ushrn(width: number) {
    return new BN(shrn.call(this.internalBigNum, width));
  }

  ishrn(width: number) {
    ishrn.call(this.internalBigNum, width);
    return this;
  }

  iushrn(width: number) {
    ishrn.call(this.internalBigNum, width);
    return this;
  }

  // no arg operations
  sqr() {
    return new BN(sqr.call(this.internalBigNum));
  }

  isqr() {
    isqr.call(this.internalBigNum);
    return this;
  }

  abs() {
    return new BN(abs.call(this.internalBigNum));
  }

  iabs() {
    iabs.call(this.internalBigNum);
    return this;
  }

  neg() {
    return new BN(neg.call(this.internalBigNum));
  }

  ineg() {
    ineg.call(this.internalBigNum);
    return this;
  }

  clone() {
    return new BN(clone.call(this.internalBigNum));
  }

  // get info
  isZero() {
    return isZero.call(this.internalBigNum);
  }

  isOne() {
    return isOne.call(this.internalBigNum);
  }

  bitLength() {
    return bitLength.call(this.internalBigNum);
  }

  byteLength() {
    return byteLength.call(this.internalBigNum);
  }

  isEven() {
    return isEven.call(this.internalBigNum);
  }

  isOdd() {
    return isOdd.call(this.internalBigNum);
  }

  isNeg() {
    return isNeg.call(this.internalBigNum);
  }

  get negative() {
    return this.isNeg();
  }

  // compare methods
  cmp(other: BN) {
    return cmp.call(this.internalBigNum, other.internalBigNum);
  }

  cmpn(other: number) {
    return cmpn.call(this.internalBigNum, other);
  }

  ucmp(other: BN) {
    return ucmp.call(this.internalBigNum, other.internalBigNum);
  }

  ltn(other: number) {
    return cmpn.call(this.internalBigNum, other) < 0;
  }

  lten(other: number) {
    return cmpn.call(this.internalBigNum, other) <= 0;
  }

  gtn(other: number) {
    return cmpn.call(this.internalBigNum, other) > 0;
  }

  gten(other: number) {
    return cmpn.call(this.internalBigNum, other) >= 0;
  }

  lt(other: BN) {
    return lt.call(this.internalBigNum, other.internalBigNum);
  }

  lte(other: BN) {
    return lte.call(this.internalBigNum, other.internalBigNum);
  }

  gt(other: BN) {
    return gt.call(this.internalBigNum, other.internalBigNum);
  }

  gte(other: BN) {
    return gte.call(this.internalBigNum, other.internalBigNum);
  }

  eq(other: BN) {
    return eq.call(this.internalBigNum, other.internalBigNum);
  }

  eqn(other: number) {
    return cmpn.call(this.internalBigNum, other) === 0;
  }

  // Field helpers
  invm(other: BN) {
    return new BN(invm.call(this.internalBigNum, other.internalBigNum));
  }

  igcd(other: BN) {
    igcd.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  gcd(other: BN) {
    return new BN(gcd.call(this.internalBigNum, other.internalBigNum));
  }

  egcd(other: BN): { a: BN, b: BN, gcd: BN } {
    throw 'not implemented yet :(';
  }

  // bitwise
  or(other: BN) {
    return new BN(or.call(this.internalBigNum, other.internalBigNum));
  }

  ior(other: BN) {
    ior.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  and(other: BN) {
    return new BN(and.call(this.internalBigNum, other.internalBigNum));
  }

  andn(other: number) {
    return this.and(new BN(other));
  }

  andln(other: number) {
    return this.andn(other);
  }

  iand(other: BN) {
    iand.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  xor(other: BN) {
    return new BN(xor.call(this.internalBigNum, other.internalBigNum));
  }

  ixor(other: BN) {
    ixor.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  // min max
  min(other: BN) {
    if (this.lte(other)) {
      return this;
    }
    return other;
  }

  max(other: BN) {
    if (this.gte(other)) {
      return this;
    }
    return other;
  }

  static max(a: BN, b: BN) {
    return a.max(b);
  }

  static min(a: BN, b: BN) {
    return a.min(b);
  }

  forceRed(mctx: InternalModContext) {
    return new RedBigNumber(forceCreateRed.call(this.internalBigNum, mctx), mctx);
  }
}

export default BN;

