import { BigNumber } from 'react-native-bignumber';

// TODO use jasmine and write proper unit tests

export const benchmarkAgainstOldCrypto = async () => {
  console.log('Starting benchmark...');

  // TODO: Benchmar here!

  BigNumber.runAsync().then((num: number) => {
    console.log('num', num);
  });

  console.log(`Benchmarks finished.`);
};
