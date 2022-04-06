/* global Buffer */
import {
  NativeBigNumber as native,
  InternalNumber,
  InternalModContext,
  InternalRedNumber,
  BigNumberPrimeType,
} from './NativeBigNumber/NativeBigNumber';

/**
 * A number instance that can store numbers with virtually infinite size.
 */
export class BN {
  internalBigNum: InternalNumber;
  _mctx?: InternalModContext;

  constructor();
  constructor(asString: string, base: 2 | 10 | 16, endian?: 'le' | 'be');
  constructor(
    asString: Array<number>,
    base?: 2 | 10 | 16,
    endian?: 'le' | 'be'
  );
  constructor(asString: ArrayBuffer, base?: 2 | 10 | 16, endian?: 'le' | 'be');
  constructor(asString: Buffer, base?: 2 | 10 | 16, endian?: 'le' | 'be');
  constructor(value: number);
  constructor(inInternalNumber: InternalNumber);
  constructor(inRedNumber: InternalRedNumber, modctx: InternalModContext);
  constructor(inInternalNumber: InternalNumber, modctx: InternalModContext);
  constructor(...args: any[]) {
    if (args[0] instanceof BN && args[0].red) {
      this.internalBigNum = native.cloneRed.call(args[0].internalRedBigNum);
      this._mctx = args[0]._mctx;
      return this;
    }
    if (args[1]?.isModContext) {
      if (!args[0]?.isInternalRedBigNum) {
        this.internalBigNum = native.bn2Mod(args[0], args[1]);
      } else {
        this.internalBigNum = args[0] as InternalRedNumber;
      }
      this._mctx = args[1];
      return;
    }

    let base = 16;
    if (typeof args[1] === 'number') {
      base = args[1];
    }
    let endian = false;
    if (args[2] === 'le') {
      endian = true;
    }
    if (args[1] === 'le') {
      endian = true;
    }

    if (args[0] == null) {
      this.internalBigNum = native.createFromNumber(0);
      return this;
    }
    if (typeof args[0] === 'string') {
      if (args[1] === 'hex') {
        args[1] = 16;
      }
      this.internalBigNum = native.createFromString(args[0], args[1], endian);
      return this;
    }
    if (typeof args[0] === 'number') {
      if (Math.abs(args[0]) > 1e9) {
        this.internalBigNum = native.createFromString(
          args[0].toString(),
          args[1],
          false
        );
      } else {
        this.internalBigNum = native.createFromNumber(args[0]);
      }
      return this;
    }
    if (typeof args[0] === 'object' && args[0].isInternalBigNum) {
      this.internalBigNum = args[0];
      return this;
    }

    if (args[0] instanceof BN) {
      this.internalBigNum = native.clone.call(args[0].internalBigNum);
      return this;
    }

    if (args[0] instanceof ArrayBuffer) {
      this.internalBigNum = native.createFromArrayBuffer(args[0], base, endian);
      return this;
    }

    if (typeof args[0] === 'object' && args[0].buffer) {
      // Buffer
      var arrayBuffer = args[0].buffer.slice(
        args[0].byteOffset,
        args[0].byteOffset + args[0].byteLength
      );
      this.internalBigNum = native.createFromArrayBuffer(
        arrayBuffer,
        base,
        endian
      );
      return this;
    }

    if (Array.isArray(args[0])) {
      this.internalBigNum = native.createFromArray(args[0], base, endian);
      return this;
    }
    throw new Error('BN constructor got wrong params :(');
  }

  get red() {
    return this._mctx != null;
  }

  static _k256: InternalModContext | null = null;
  static _p224: InternalModContext | null = null;
  static _p192: InternalModContext | null = null;
  static _p25519: InternalModContext | null = null;

  static get k256(): InternalModContext {
    // It it faster than simple prime map?
    if (!BN._k256) {
      BN._k256 = native.getPrimeContext('k256');
    }
    return BN._k256;
  }

  static get p224(): InternalModContext {
    if (!BN._p224) {
      BN._p224 = native.getPrimeContext('p224');
    }
    return BN._p224;
  }

  static get p192(): InternalModContext {
    if (!BN._p192) {
      BN._p192 = native.getPrimeContext('p192');
    }
    return BN._p192;
  }

  static get p25519(): InternalModContext {
    if (!BN._p25519) {
      BN._p25519 = native.getPrimeContext('p25519');
    }
    return BN._p25519;
  }

  static _prime(prime: BigNumberPrimeType) {
    switch (prime) {
      case 'k256':
      case 'p224':
      case 'p192':
      case 'p25519':
        return { p: new BN(native.getPrime(prime)) };
      default:
        throw new Error('Unknown prime number!');
    }
  }

  static red(num: BigNumberPrimeType | BN | number): InternalModContext {
    if (typeof num === 'string') {
      // TODO (Szymon)
      switch (num) {
        case 'k256':
          return BN.k256;
        case 'p224':
          return BN.p224;
        case 'p192':
          return BN.p192;
        case 'p25519':
          return BN.p25519;
        default:
          throw new Error('Unknown prime number!');
      }
    }
    if (typeof num === 'number') {
      return native.createModCtxFromNumber(num);
    }
    return native.createModCtx(num.internalBigNum);
  }

  static mont(num: BigNumberPrimeType | BN | number): InternalModContext {
    return BN.red(num);
  }

  static isBN(obj: any): boolean {
    return obj?.internalBigNum?.isInternalBigNum === true;
  }

  toRed(mctx: InternalModContext) {
    return new BN(this.internalBigNum, mctx);
  }

  toString(base: 2 | 10 | 16, len?: number): string {
    if (this.red) {
      return this.fromRed().toString(base, len);
    }
    return native.toString.call(this.internalBigNum, base, len);
  }

  toJSON(): string {
    let str = this.toString(16);
    let par = 0;
    if (str[0] === '-') {
      par = 1;
    }
    if (str.length % 2 !== par) {
      str = str.slice(0, par) + '0' + str.slice(par);
    }
    return str;
  }

  toNumber() {
    return native.toNumber.call(this.internalBigNum);
  }

  toArray(endian?: 'le' | 'be', len?: number) {
    return native.toArray.call(this.internalBigNum, endian === 'le', len || -1);
  }

  // TODO(MARC ts magician )
  toArrayLike<T extends (len: number) => { buffer: ArrayBuffer }>(
    arrayLike: T,
    endian: 'le' | 'be',
    len: number
  ) {
    if (typeof arrayLike !== 'function') {
      console.log('toArrayLike exptects constructor');
      throw new Error('toArrayLike exptects constructor');
    }

    if (arrayLike?.name === 'Array') {
      return this.toArray(endian, len);
    }

    const res = new (arrayLike as any)(len);

    native.toArrayLike.call(
      this.internalBigNum,
      res.buffer,
      endian === 'le',
      len || -1
    );
    return res;
  }

  toBuffer(endian?: 'le' | 'be', len?: number) {
    return Buffer.from(this.toArray(endian || 'be', len));
  }

  // add
  iadd(other: BN) {
    native.iadd.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  add(other: BN) {
    return new BN(native.add.call(this.internalBigNum, other.internalBigNum));
  }

  iaddn(other: number) {
    native.iaddn.call(this.internalBigNum, other);
    return this;
  }

  addn(other: number) {
    return new BN(native.addn.call(this.internalBigNum, other));
  }

  // sub
  isub(other: BN) {
    native.isub.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  sub(other: BN) {
    return new BN(native.sub.call(this.internalBigNum, other.internalBigNum));
  }

  isubn(other: number) {
    native.isubn.call(this.internalBigNum, other);
    return this;
  }

  subn(other: number) {
    return new BN(native.subn.call(this.internalBigNum, other));
  }

  // mul
  imul(other: BN) {
    native.imul.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  mul(other: BN) {
    return new BN(native.mul.call(this.internalBigNum, other.internalBigNum));
  }

  mulf(other: BN) {
    return new BN(native.mul.call(this.internalBigNum, other.internalBigNum));
  }

  imuln(other: number) {
    native.imuln.call(this.internalBigNum, other);
    return this;
  }

  muln(other: number) {
    return new BN(native.muln.call(this.internalBigNum, other));
  }

  // div
  idiv(other: BN) {
    native.idiv.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  div(other: BN) {
    return new BN(native.div.call(this.internalBigNum, other.internalBigNum));
  }

  idivn(other: number) {
    native.idivn.call(this.internalBigNum, other);
    return this;
  }

  divn(other: number) {
    return new BN(native.divn.call(this.internalBigNum, other));
  }

  divmod(m: BN) {
    return new BN(native.divmod.call(this.internalBigNum, m.internalBigNum));
  }

  divRound(m: BN) {
    return new BN(native.divRound.call(this.internalBigNum, m.internalBigNum));
  }

  // mod
  imod(other: BN) {
    native.imod.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  mod(other: BN) {
    return new BN(native.mod.call(this.internalBigNum, other.internalBigNum));
  }

  umod(other: BN) {
    return new BN(native.umod.call(this.internalBigNum, other.internalBigNum));
  }

  imodn(other: number) {
    native.imodn.call(this.internalBigNum, other);
    return this;
  }

  modn(other: number) {
    return new BN(native.modn.call(this.internalBigNum, other)).toNumber();
  }

  modrn(other: number) {
    return new BN(native.modn.call(this.internalBigNum, other));
  }

  // pow
  ipow(other: BN) {
    native.ipow.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  pow(other: BN) {
    return new BN(native.pow.call(this.internalBigNum, other.internalBigNum));
  }

  // bit manipulation
  setn(bit: number, value: boolean) {
    native.setn.call(this.internalBigNum, bit, value ? 1 : 0);
    return this;
  }

  testn(bit: number) {
    return native.testn.call(this.internalBigNum, bit);
  }

  bincn(bit: number) {
    native.bincn.call(this.internalBigNum, bit);
    return this;
  }

  notn(width: number) {
    return new BN(native.notn.call(this.internalBigNum, width));
  }

  inotn(width: number) {
    native.inotn.call(this.internalBigNum, width);
    return this;
  }

  maskn(width: number) {
    return new BN(native.maskn.call(this.internalBigNum, width));
  }

  imaskn(width: number) {
    native.imaskn.call(this.internalBigNum, width);
    return this;
  }

  shln(width: number) {
    return new BN(native.shln.call(this.internalBigNum, width));
  }

  ushln(width: number) {
    return new BN(native.shln.call(this.internalBigNum, width));
  }

  ishln(width: number) {
    native.ishln.call(this.internalBigNum, width);
    return this;
  }

  iushln(width: number) {
    native.ishln.call(this.internalBigNum, width);
    return this;
  }

  shrn(width: number) {
    return new BN(native.shrn.call(this.internalBigNum, width));
  }

  ushrn(width: number) {
    return new BN(native.shrn.call(this.internalBigNum, width));
  }

  ishrn(width: number) {
    native.ishrn.call(this.internalBigNum, width);
    return this;
  }

  iushrn(width: number) {
    native.ishrn.call(this.internalBigNum, width);
    return this;
  }

  // no arg operations
  sqr() {
    return new BN(native.sqr.call(this.internalBigNum));
  }

  isqr() {
    native.isqr.call(this.internalBigNum);
    return this;
  }

  abs() {
    return new BN(native.abs.call(this.internalBigNum));
  }

  iabs() {
    native.iabs.call(this.internalBigNum);
    return this;
  }

  neg() {
    if (this.red) {
      return new BN(native.redNeg.call(this.internalRedBigNum), this.mctx);
    }
    return new BN(native.neg.call(this.internalBigNum));
  }

  ineg() {
    native.ineg.call(this.internalBigNum);
    return this;
  }

  clone() {
    if (this.red) {
      return new BN(native.cloneRed.call(this.internalRedBigNum), this.mctx);
    }
    return new BN(native.clone.call(this.internalBigNum));
  }

  // get info
  isZero() {
    return native.isZero.call(this.internalBigNum);
  }

  isOne() {
    return native.isOne.call(this.internalBigNum);
  }

  bitLength() {
    return native.bitLength.call(this.internalBigNum);
  }

  byteLength() {
    return native.byteLength.call(this.internalBigNum);
  }

  isEven() {
    return native.isEven.call(this.internalBigNum);
  }

  isOdd() {
    return native.isOdd.call(this.internalBigNum);
  }

  isNeg() {
    return native.isNeg.call(this.internalBigNum);
  }

  get negative() {
    return this.isNeg();
  }

  // compare methods
  cmp(other: BN) {
    return native.cmp.call(this.internalBigNum, other.internalBigNum);
  }

  cmpn(other: number) {
    return native.cmpn.call(this.internalBigNum, other);
  }

  ucmp(other: BN) {
    return native.ucmp.call(this.internalBigNum, other.internalBigNum);
  }

  ltn(other: number) {
    return native.cmpn.call(this.internalBigNum, other) < 0;
  }

  lten(other: number) {
    return native.cmpn.call(this.internalBigNum, other) <= 0;
  }

  gtn(other: number) {
    return native.cmpn.call(this.internalBigNum, other) > 0;
  }

  gten(other: number) {
    return native.cmpn.call(this.internalBigNum, other) >= 0;
  }

  lt(other: BN) {
    return native.lt.call(this.internalBigNum, other.internalBigNum);
  }

  lte(other: BN) {
    return native.lte.call(this.internalBigNum, other.internalBigNum);
  }

  gt(other: BN) {
    return native.gt.call(this.internalBigNum, other.internalBigNum);
  }

  gte(other: BN) {
    return native.gte.call(this.internalBigNum, other.internalBigNum);
  }

  eq(other: BN) {
    return native.eq.call(this.internalBigNum, other.internalBigNum);
  }

  eqn(other: number) {
    return native.cmpn.call(this.internalBigNum, other) === 0;
  }

  // Field helpers
  invm(other: BN) {
    return new BN(native.invm.call(this.internalBigNum, other.internalBigNum));
  }

  igcd(other: BN) {
    native.igcd.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  gcd(other: BN) {
    return new BN(native.gcd.call(this.internalBigNum, other.internalBigNum));
  }

  egcd(other: BN): { a: BN; b: BN; gcd: BN } {
    throw new Error(`egcd(${other}) is not yet implemented!`);
  }

  // bitwise
  or(other: BN) {
    return new BN(native.or.call(this.internalBigNum, other.internalBigNum));
  }

  ior(other: BN) {
    native.ior.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  and(other: BN) {
    return new BN(native.and.call(this.internalBigNum, other.internalBigNum));
  }

  andn(other: number) {
    return this.and(new BN(other));
  }

  andln(other: number) {
    // probaby we can use getWord
    return this.andn(other).toNumber();
  }

  iand(other: BN) {
    native.iand.call(this.internalBigNum, other.internalBigNum);
    return this;
  }

  xor(other: BN) {
    return new BN(native.xor.call(this.internalBigNum, other.internalBigNum));
  }

  ixor(other: BN) {
    native.ixor.call(this.internalBigNum, other.internalBigNum);
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

  // RedBigNum
  forceRed(mctx: InternalModContext) {
    this.internalBigNum = native.forceCreateRed.call(this.internalBigNum, mctx);
    this._mctx = mctx;
  }

  get internalRedBigNum(): InternalRedNumber {
    if (this.red) {
      return this.internalBigNum as InternalRedNumber;
    }
    throw new Error('This BN is not RedNumber');
  }

  get mctx(): InternalModContext {
    if (this.red) {
      return this._mctx as InternalModContext;
    }
    throw new Error('This BN is not RedNumber');
  }

  fromRed() {
    return new BN(native.mod2bn(this.internalRedBigNum));
  }

  redAdd(other: BN) {
    return new BN(
      native.redAdd.call(this.internalRedBigNum, other.internalRedBigNum),
      this.mctx
    );
  }

  redIAdd(other: BN) {
    native.redIAdd.call(this.internalRedBigNum, other.internalRedBigNum);
    return this;
  }

  redSub(other: BN) {
    return new BN(
      native.redSub.call(this.internalRedBigNum, other.internalRedBigNum),
      this.mctx
    );
  }

  redISub(other: BN) {
    native.redISub.call(this.internalRedBigNum, other.internalRedBigNum);
    return this;
  }

  redMul(other: BN) {
    return new BN(
      native.redMul.call(this.internalRedBigNum, other.internalRedBigNum),
      this.mctx
    );
  }

  redIMul(other: BN) {
    native.redIMul.call(this.internalRedBigNum, other.internalRedBigNum);
    return this;
  }

  redShl(places: number) {
    return new BN(
      native.redShl.call(this.internalRedBigNum, places),
      this.mctx
    );
  }

  redSqrt() {
    return new BN(native.redSqrt.call(this.internalRedBigNum), this.mctx);
  }

  redSqr() {
    return new BN(native.redSqr.call(this.internalRedBigNum), this.mctx);
  }

  redISqr() {
    native.redISqr.call(this.internalRedBigNum);
    return this;
  }

  redNeg() {
    return new BN(native.redNeg.call(this.internalRedBigNum), this.mctx);
  }

  redInvm() {
    return new BN(native.redInvm.call(this.internalRedBigNum), this.mctx);
  }

  redPow(other: BN) {
    return new BN(
      native.redPow.call(this.internalRedBigNum, other.internalBigNum),
      this.mctx
    );
  }

  toTwos(width: number) {
    // todo Can be optimised by adding one native call
    if (this.isNeg() !== false) {
      return this.abs().inotn(width).iaddn(1);
    }
    return this.clone();
  }

  fromTwos(width: number) {
    // todo Can be optimised by adding one native call
    if (this.testn(width - 1)) {
      return this.notn(width).iaddn(1).ineg();
    }
    return this.clone();
  }

  mulTo(other: BN, res: BN) {
    // todo memory allocation can be avoided here
    res.internalBigNum = native.mul.call(
      this.internalBigNum,
      other.internalBigNum
    );
    return res;
  }

  value() {
    return this.toString(10);
  }

  static get BN() {
    return BN;
  }

  static get default() {
    return BN;
  }

  static get defaults() {
    return { BN };
  }
}

declare global {
  function __createBN(internalNumber: InternalNumber): BN;
}
global.__createBN = (internalNumber: InternalNumber) => {
  return new BN(internalNumber);
};

export default BN;
