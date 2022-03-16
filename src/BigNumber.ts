import {
  NativeBigNumber,
  InternalNumber,
} from './NativeBigNumber/NativeBigNumber';

const createFromString = NativeBigNumber.createFromString;
const createFromNumber = NativeBigNumber.creareFromNumber;

export class BN {
  private internalBigNum: InternalNumber;

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

  toString(base: 2 | 10 | 16, len?: number) {
    return this.internalBigNum.toString(base, len);
  }

  iadd(other: BN) {
    this.internalBigNum.iadd(other.internalBigNum);
  }

  add(other: BN) {
    return new BN(this.internalBigNum.add(other.internalBigNum));
  }
}

