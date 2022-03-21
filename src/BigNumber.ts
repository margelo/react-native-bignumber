import {
  NativeBigNumber,
  InternalNumber,
  InternalModContext,
  InternalRedNumber,
} from './NativeBigNumber/NativeBigNumber';

const createFromString = NativeBigNumber.createFromString;
const createFromNumber = NativeBigNumber.createFromNumber;
const mod2bn = NativeBigNumber.mod2bn;
const bn2Mod = NativeBigNumber.bn2Mod;
const createModCtx = NativeBigNumber.createModCtx;

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

  fromRed() {
    return new BN(mod2bn(this.internalRedBigNum));
  }

  redAdd(other: RedBigNumber) {
    return new RedBigNumber(this.internalRedBigNum.redAdd(other.internalRedBigNum), this.mctx);
  }

  redIAdd(other: RedBigNumber) {
    this.internalRedBigNum.redIAdd(other.internalRedBigNum);
    return this;
  }

  redSub(other: RedBigNumber) {
    return new RedBigNumber(this.internalRedBigNum.redSub(other.internalRedBigNum), this.mctx);
  }

  redISub(other: RedBigNumber) {
    this.internalRedBigNum.redISub(other.internalRedBigNum);
    return this;
  }

  redMul(other: RedBigNumber) {
    return new RedBigNumber(this.internalRedBigNum.redMul(other.internalRedBigNum), this.mctx);
  }

  redIMul(other: RedBigNumber) {
    this.internalRedBigNum.redIMul(other.internalRedBigNum);
    return this;
  }

  redShl(places: number) {
    return new RedBigNumber(this.internalRedBigNum.redShl(places), this.mctx);
  }

  redSqrt() {
    return new RedBigNumber(this.internalRedBigNum.redSqrt(), this.mctx);
  }

  redSqr() {
    return new RedBigNumber(this.internalRedBigNum.redSqr(), this.mctx);
  }

  redISqr() {
    this.internalRedBigNum.redISqr();
    return this;
  }

  redNeg() {
    return new RedBigNumber(this.internalRedBigNum.redNeg(), this.mctx);
  }

  redInvm() {
    return new RedBigNumber(this.internalRedBigNum.redInvm(), this.mctx);
  }

  redPow(other: BN) {
    return new RedBigNumber(this.internalRedBigNum.redPow(other.internalBigNum), this.mctx);
  }
}

export class BN {
  internalBigNum: InternalNumber;

  constructor(asString: string, base: 2 | 10 | 16);
  constructor(value: number);
  constructor(in: InternalNumber);
  constructor(...args: any[]) {
    if (typeof args[0] === 'string') {
      if (args[1] === 'hex') {
        args[1] = 16;
      }
      this.internalBigNum = createFromString(args[0], args[1]);
      return this;
    }
    if (typeof args[0] === 'number') {
      this.internalBigNum = createFromNumber(args[0]);
      return this;
    }
    if ((typeof args[0] === 'object') && args[0].isInternalBigNum) {
      this.internalBigNum = args[0];
      return this;
    }
    throw 'BN constructor got wrong params :(';
  }

  static red(num: 'string' | InternalNumber): InternalModContext {
    if (typeof num === 'string') { // TODO (Szymon)
      throw "not implemented yet :("
    }
    return createModCtx(num);
  }

  static mont(num: InternalNumber): InternalModContext {
    return createModCtx(num);
  }

  static isBN(obj: any): boolean {
    return obj?.internalBigNum?.isInternalBigNum === true;
  }

  toRed(mctx: InternalModContext) {
    return new RedBigNumber(this.internalBigNum, mctx);
  }

  toString(base: 2 | 10 | 16, len?: number) {
    return this.internalBigNum.toString(base, len);
  }

  toNumber() {
    return parseInt(this.internalBigNum.toString(10, 53));
  }

  // add
  iadd(other: BN) {
    this.internalBigNum.iadd(other.internalBigNum);
    return this;
  }

  add(other: BN) {
    return new BN(this.internalBigNum.add(other.internalBigNum));
  }

  iaddn(other: number) {
    this.internalBigNum.iaddn(other);
    return this;
  }

  addn(other: number) {
    return new BN(this.internalBigNum.addn(other));
  }

  // sub
  isub(other: BN) {
    this.internalBigNum.isub(other.internalBigNum);
    return this;
  }

  sub(other: BN) {
    return new BN(this.internalBigNum.sub(other.internalBigNum));
  }

  isubn(other: number) {
    this.internalBigNum.isubn(other);
    return this;
  }

  subn(other: number) {
    return new BN(this.internalBigNum.subn(other));
  }

  // mul
  imul(other: BN) {
    this.internalBigNum.imul(other.internalBigNum);
    return this;
  }

  mul(other: BN) {
    return new BN(this.internalBigNum.mul(other.internalBigNum));
  }

  mulf(other: BN) {
    return new BN(this.internalBigNum.mul(other.internalBigNum));
  }

  imuln(other: number) {
    this.internalBigNum.imuln(other);
    return this;
  }

  muln(other: number) {
    return new BN(this.internalBigNum.muln(other));
  }

  // div
  idiv(other: BN) {
    this.internalBigNum.idiv(other.internalBigNum);
    return this;
  }

  div(other: BN) {
    return new BN(this.internalBigNum.div(other.internalBigNum));
  }

  idivn(other: number) {
    this.internalBigNum.idivn(other);
    return this;
  }

  divn(other: number) {
    return new BN(this.internalBigNum.divn(other));
  }

  divmod(m: BN) {
    return new BN(this.internalBigNum.divmod(m.internalBigNum));
  }

  divRound(m: BN) {
    return new BN(this.internalBigNum.divRound(m.internalBigNum));
  }

  // mod
  imod(other: BN) {
    this.internalBigNum.imod(other.internalBigNum);
    return this;
  }

  mod(other: BN) {
    return new BN(this.internalBigNum.mod(other.internalBigNum));
  }

  umod(other: BN) {
    return new BN(this.internalBigNum.umod(other.internalBigNum));
  }

  imodn(other: number) {
    this.internalBigNum.imodn(other);
    return this;
  }

  modn(other: number) {
    return new BN(this.internalBigNum.modn(other));
  }

  modrn(other: number) {
    return new BN(this.internalBigNum.modn(other));
  }

  // pow
  ipow(other: BN) {
    this.internalBigNum.ipow(other.internalBigNum);
    return this;
  }

  pow(other: BN) {
    return new BN(this.internalBigNum.pow(other.internalBigNum));
  }

  // bit manipulation
  setn(bit: number) {
    this.internalBigNum.setn(bit);
    return this;
  }

  testn(bit: number) {
    return this.internalBigNum.testn(bit);
  }

  bincn(bit: number) {
    this.internalBigNum.bincn(bit);
    return this;
  }

  notn(width: number) {
    return new BN(this.internalBigNum.notn(width));
  }

  inotn(width: number) {
    this.internalBigNum.inotn(width);
    return this;
  }

  maskn(width: number) {
    return new BN(this.internalBigNum.maskn(width));
  }

  imaskn(width: number) {
    this.internalBigNum.imaskn(width);
    return this;
  }

  shln(width: number) {
    return new BN(this.internalBigNum.shln(width));
  }

  ishln(width: number) {
    this.internalBigNum.ishln(width);
    return this;
  }

  shrn(width: number) {
    return new BN(this.internalBigNum.shrn(width));
  }

  ishrn(width: number) {
    this.internalBigNum.ishrn(width);
    return this;
  }

  // no arg operations
  sqr() {
    return new BN(this.internalBigNum.sqr());
  }

  isqr() {
    this.internalBigNum.isqr();
    return this;
  }

  abs() {
    return new BN(this.internalBigNum.abs());
  }

  iabs() {
    this.internalBigNum.iabs();
    return this;
  }

  neg() {
    return new BN(this.internalBigNum.neg());
  }

  ineg() {
    this.internalBigNum.ineg();
    return this;
  }

  clone() {
    return new BN(this.internalBigNum.clone());
  }

  // get info
  isZero() {
    return this.internalBigNum.isZero();
  }

  isOne() {
    return this.internalBigNum.isOne();
  }

  bitLength() {
    return this.internalBigNum.bitLength();
  }

  byteLength() {
    return this.internalBigNum.byteLength();
  }

  isEven() {
    return this.internalBigNum.isEven();
  }

  isOdd() {
    return this.internalBigNum.isOdd();
  }

  isNeg() {
    return this.internalBigNum.isNeg();
  }

  get negative() {
    return this.isNeg();
  }

  // compare methods
  cmp(other: BN) {
    return this.internalBigNum.cmp(other.internalBigNum);
  }

  cmpn(other: number) {
    return this.internalBigNum.cmp(new BN(other).internalBigNum);
  }

  ucmp(other: BN) {
    return this.internalBigNum.ucmp(other.internalBigNum);
  }

  lt(other: BN) {
    return this.internalBigNum.lt(other.internalBigNum);
  }

  lte(other: BN) {
    return this.internalBigNum.lte(other.internalBigNum);
  }

  gt(other: BN) {
    return this.internalBigNum.gt(other.internalBigNum);
  }

  gte(other: BN) {
    return this.internalBigNum.gte(other.internalBigNum);
  }

  eq(other: BN) {
    return this.internalBigNum.eq(other.internalBigNum);
  }

  // Field helpers
  invm(other: BN) {
    return new BN(this.internalBigNum.invm(other.internalBigNum));
  }

  igcd(other: BN) {
    this.internalBigNum.igcd(other.internalBigNum);
    return this;
  }

  gcd(other: BN) {
    return new BN(this.internalBigNum.gcd(other.internalBigNum));
  }

  egcd(other: BN): { a: BN, b: BN, gcd: BN } {
    throw 'not implemented yet :(';
  }

  // bitwise
  or(other: BN) {
    return new BN(this.internalBigNum.or(other.internalBigNum));
  }

  ior(other: BN) {
    this.internalBigNum.ior(other.internalBigNum);
    return this;
  }

  and(other: BN) {
    return new BN(this.internalBigNum.and(other.internalBigNum));
  }

  iand(other: BN) {
    this.internalBigNum.iand(other.internalBigNum);
    return this;
  }

  xor(other: BN) {
    return new BN(this.internalBigNum.xor(other.internalBigNum));
  }

  ixor(other: BN) {
    this.internalBigNum.ixor(other.internalBigNum);
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
}

