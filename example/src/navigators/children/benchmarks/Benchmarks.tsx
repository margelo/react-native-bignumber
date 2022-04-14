import React, { useEffect } from 'react';
import { View, Text, StyleSheet } from 'react-native';
import type { RootStackParamList } from '../../RootProps';
import type { NativeStackScreenProps } from '@react-navigation/native-stack';
import FastBN from 'react-native-bignumber';
import elliptic from 'elliptic';
import bn_elliptic from 'bn_elliptic';
import BN from 'bn.js';

// console.log('FastBN', FastBN.isBN);

function startBenchmarking() {
  // console.log('test curve 2');
  console.log('🔥 Benchmarking elliptic curve function...');
  const curveSlow = new bn_elliptic.curve.edwards({
    p: '7fffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffed',
    a: '-1',
    c: '1',
    // -121665 * (121666^(-1)) (mod P)
    d: '52036cee2b6ffe73 8cc740797779e898 00700a4d4141d8ab 75eb4dca135978a3',
    n: '1000000000000000 0000000000000000 14def9dea2f79cd6 5812631a5cf5d3ed',
    gRed: false,
    g: [
      '216936d3cd6e53fec0a4e231fdd6dc5c692cc7609525a7b2c9562d608f25d51a',

      // 4/5
      '6666666666666666666666666666666666666666666666666666666666666658',
    ],
  });
  const curveFast = new elliptic.curve.edwards({
    p: '7fffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffed',
    a: '-1',
    c: '1',
    // -121665 * (121666^(-1)) (mod P)
    d: '52036cee2b6ffe73 8cc740797779e898 00700a4d4141d8ab 75eb4dca135978a3',
    n: '1000000000000000 0000000000000000 14def9dea2f79cd6 5812631a5cf5d3ed',
    gRed: false,
    g: [
      '216936d3cd6e53fec0a4e231fdd6dc5c692cc7609525a7b2c9562d608f25d51a',

      // 4/5
      '6666666666666666666666666666666666666666666666666666666666666658',
    ],
  });

  const bytes = new Uint8Array([
    5, 69, 248, 173, 171, 254, 19, 253, 143, 140, 146, 174, 26, 128, 3, 52, 106,
    55, 112, 245, 62, 127, 42, 93, 0, 81, 47, 177, 30, 25, 39, 70,
  ]);
  const yFast = new FastBN(bytes, 16, 'le');
  const ySlow = new BN(bytes, 16, 'le');
  const a = () => {
    const point = curveFast.pointFromY(yFast, true);
    point.getX().toString(16);
  };
  const b = () => {
    const point = curveSlow.pointFromY(ySlow, true);
    point.getX().toString(16);
  };

  const startCpp = global.performance.now();
  for (let i = 0; i < 10; i++) a();
  const endCpp = global.performance.now();
  const startJS = global.performance.now();
  for (let i = 0; i < 10; i++) b();
  const endJS = global.performance.now();

  console.log(
    `🐌 Elliptic curve function with BN.js took: ${(endJS - startJS).toFixed(
      0
    )}ms`
  );
  console.log(
    `🏎️  Elliptic curve function with react-native-bignumber took: ${(
      endCpp - startCpp
    ).toFixed(0)}ms`
  );
  // console.log('test curve 2 - result: ' + time);
}

type BenchmarksProps = NativeStackScreenProps<RootStackParamList, 'Benchmarks'>;

export const Benchmarks: React.FC<BenchmarksProps> = () => {
  useEffect(() => {
    startBenchmarking();
  }, []);

  return (
    <View style={styles.container}>
      <Text> Testing performance - You can see results in logs! </Text>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignContext: 'center',
  },
});
