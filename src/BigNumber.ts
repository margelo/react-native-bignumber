import { NativeBigNumber } from './NativeBigNumber/NativeBigNumber';

async function runAsync(): Promise<number> {
  return NativeBigNumber.runAsync();
}

export const BigNumber = {
  runAsync,
};
