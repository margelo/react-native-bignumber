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
  }

  redSub(other: RedBigNumber) {
    return new RedBigNumber(this.internalRedBigNum.redSub(other.internalRedBigNum), this.mctx);
  }

  redISub(other: RedBigNumber) {
    this.internalRedBigNum.redISub(other.internalRedBigNum);
  }

  redMul(other: RedBigNumber) {
    return new RedBigNumber(this.internalRedBigNum.redMul(other.internalRedBigNum), this.mctx);
  }

  redIMul(other: RedBigNumber) {
    this.internalRedBigNum.redIMul(other.internalRedBigNum);
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

  // add
  iadd(other: BN) {
    this.internalBigNum.iadd(other.internalBigNum);
  }

  add(other: BN) {
    return new BN(this.internalBigNum.add(other.internalBigNum));
  }

  iaddn(other: number) {
    this.internalBigNum.iaddn(other);
  }

  addn(other: number) {
    return new BN(this.internalBigNum.addn(other));
  }

  // sub
  isub(other: BN) {
    this.internalBigNum.isub(other.internalBigNum);
  }

  sub(other: BN) {
    return new BN(this.internalBigNum.sub(other.internalBigNum));
  }

  isubn(other: number) {
    this.internalBigNum.isubn(other);
  }

  subn(other: number) {
    return new BN(this.internalBigNum.subn(other));
  }

  // mul
  imul(other: BN) {
    this.internalBigNum.imul(other.internalBigNum);
  }

  mul(other: BN) {
    return new BN(this.internalBigNum.mul(other.internalBigNum));
  }

  imuln(other: number) {
    this.internalBigNum.imuln(other);
  }

  muln(other: number) {
    return new BN(this.internalBigNum.muln(other));
  }

  // div
  idiv(other: BN) {
    this.internalBigNum.idiv(other.internalBigNum);
  }

  div(other: BN) {
    return new BN(this.internalBigNum.div(other.internalBigNum));
  }

  idivn(other: number) {
    this.internalBigNum.idivn(other);
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
  }

  mod(other: BN) {
    return new BN(this.internalBigNum.mod(other.internalBigNum));
  }

  umod(other: BN) {
    return new BN(this.internalBigNum.umod(other.internalBigNum));
  }

  imodn(other: number) {
    this.internalBigNum.imodn(other);
  }

  modn(other: number) {
    return new BN(this.internalBigNum.modn(other));
  }

  // pow
  ipow(other: BN) {
    this.internalBigNum.ipow(other.internalBigNum);
  }

  pow(other: BN) {
    return new BN(this.internalBigNum.pow(other.internalBigNum));
  }

  // bit manipulation
  setn(bit: number) {
    this.internalBigNum.setn(bit);
  }

  testn(bit: number) {
    return this.internalBigNum.testn(bit);
  }

  bincn(bit: number) {
    this.internalBigNum.bincn(bit);
  }

  notn(width: number) {
    return new BN(this.internalBigNum.notn(width));
  }

  inotn(width: number) {
    this.internalBigNum.inotn(width);
  }

  maskn(width: number) {
    return new BN(this.internalBigNum.maskn(width));
  }

  imaskn(width: number) {
    this.internalBigNum.imaskn(width);
  }

  shln(width: number) {
    return new BN(this.internalBigNum.shln(width));
  }

  ishln(width: number) {
    this.internalBigNum.ishln(width);
  }

  shrn(width: number) {
    return new BN(this.internalBigNum.shrn(width));
  }

  ishrn(width: number) {
    this.internalBigNum.ishrn(width);
  }

  // no arg operations
  sqr() {
    return new BN(this.internalBigNum.sqr());
  }

  isqr() {
    this.internalBigNum.isqr();
  }

  abs() {
    return new BN(this.internalBigNum.abs());
  }

  iabs() {
    this.internalBigNum.iabs();
  }

  neg() {
    return new BN(this.internalBigNum.neg());
  }

  ineg() {
    this.internalBigNum.ineg();
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

  // compare methods
  cmp(other: BN) {
    return this.internalBigNum.cmp(other.internalBigNum);
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
    return new BN(this.internalBigNum.igcd(other.internalBigNum));
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
  }

  and(other: BN) {
    return new BN(this.internalBigNum.and(other.internalBigNum));
  }

  iand(other: BN) {
    this.internalBigNum.iand(other.internalBigNum);
  }

  xor(other: BN) {
    return new BN(this.internalBigNum.xor(other.internalBigNum));
  }

  ixor(other: BN) {
    this.internalBigNum.ixor(other.internalBigNum);
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
}

