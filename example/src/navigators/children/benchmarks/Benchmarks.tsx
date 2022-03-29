import React, { useEffect } from 'react';
import { View, Text, StyleSheet } from 'react-native';
import type { RootStackParamList } from '../../RootProps';
import type { NativeStackScreenProps } from '@react-navigation/native-stack';
import FastBN from 'react-native-bignumber';
import elliptic from 'elliptic';
import bn_elliptic from 'bn_elliptic';
import BN from 'bn.js';

function getTime(f: () => void): number {
  const before = global.performance.now();
  f();
  const after = global.performance.now();
  return after - before;
}

function cmp(f: () => void, g: () => void) {
  return getTime(g) - getTime(f);
}

function startBenchmarking() {
  const p = new FastBN(
    'ffffffff00000001000000000000000000000000ffffffffffffffffffffffff',
    16
  );
  const a = new FastBN(
    'e1d969b8192fbac73ea5b7921896d6a2263d4d4077bb8e5055361d1f7f8163f3',
    16
  );

  const pp = new BN(
    'ffffffff00000001000000000000000000000000ffffffffffffffffffffffff',
    16
  );
  const aa = new BN(
    'e1d969b8192fbac73ea5b7921896d6a2263d4d4077bb8e5055361d1f7f8163f3',
    16
  );

  {
    console.log('test multiply');
    let time = cmp(
      () => {
        a.mul(p);
      },
      () => {
        aa.mul(pp);
      }
    );
    console.log('test multiply - result: ' + time);
  }

  {
    console.log('test add');
    let time = cmp(
      () => {
        a.add(p);
      },
      () => {
        aa.add(pp);
      }
    );
    console.log('test add - result: ' + time);
  }

  {
    const tenFBN = new FastBN(10);
    const ten = new BN(10);
    console.log('test pow');
    let time = cmp(
      () => {
        a.pow(tenFBN);
      },
      () => {
        aa.pow(ten);
      }
    );
    console.log('test pow - result: ' + time);
  }

  {
    console.log('test div');
    let time = cmp(
      () => {
        p.div(a);
      },
      () => {
        pp.div(aa);
      }
    );
    console.log('test div - result: ' + time);
  }

  {
    console.log('test create two BN');
    let time = cmp(
      () => {
        const p = new FastBN(
          'ffffffff00000001000000000000000000000000ffffffffffffffffffffffff',
          16
        );
        const a = new FastBN(
          'e1d969b8192fbac73ea5b7921896d6a2263d4d4077bb8e5055361d1f7f8163f3',
          16
        );
      },
      () => {
        const p = new BN(
          'ffffffff00000001000000000000000000000000ffffffffffffffffffffffff',
          16
        );
        const a = new BN(
          'e1d969b8192fbac73ea5b7921896d6a2263d4d4077bb8e5055361d1f7f8163f3',
          16
        );
      }
    );
    console.log('test create two BN - result: ' + time);
  }

  {
    console.log('test red sqrt BN');
    let time = cmp(
      () => {
        const fn = FastBN.red;
        var p = new FastBN(263);
        var m = fn(p);
        var q = new FastBN(11).toRed(m);

        var qr = q.redSqrt();

        qr = q.redSqrt();

        p = new FastBN('fffffffffffffffffffffffffffffffeffffffffffffffff', 16);
        m = fn(p);

        q = new FastBN(13).toRed(m);
        qr = q.redSqrt(true, p);

        qr = q.redSqrt(false, p);

        // Tonelli-shanks
        p = new FastBN(13);
        m = fn(p);
        q = new FastBN(10).toRed(m);
      },
      () => {
        const fn = BN.red;
        var p = new BN(263);
        var m = fn(p);
        var q = new BN(11).toRed(m);

        var qr = q.redSqrt();

        qr = q.redSqrt();

        p = new BN('fffffffffffffffffffffffffffffffeffffffffffffffff', 16);
        m = fn(p);

        q = new BN(13).toRed(m);
        qr = q.redSqrt(true, p);

        qr = q.redSqrt(false, p);

        // Tonelli-shanks
        p = new BN(13);
        m = fn(p);
        q = new BN(10).toRed(m);
      }
    );
    console.log('test red sqrt BN - result: ' + time);
  }

  {
    console.log('test curve 1');
    let time = cmp(
      () => {
        var curve = new elliptic.curve.short({
          p: '1d',
          a: '4',
          b: '14',
        });

        var p = curve.point('18', '16');
        p.validate();
        p.dbl().validate();
        p.dbl().add(p).validate();
        p.dbl().add(p.dbl()).validate();
        p.dbl().add(p.dbl()).eq(p.add(p).add(p).add(p));
      },
      () => {
        var curve = new bn_elliptic.curve.short({
          p: '1d',
          a: '4',
          b: '14',
        });

        var p = curve.point('18', '16');
        p.validate();
        p.dbl().validate();
        p.dbl().add(p).validate();
        p.dbl().add(p.dbl()).validate();
        p.dbl().add(p.dbl()).eq(p.add(p).add(p).add(p));
      }
    );
    console.log('test curve 1 - result: ' + time);
  }

  {
    console.log('test curve 2');
    let time = cmp(
      () => {
        var curve = new elliptic.curve.edwards({
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

        var bytes = new Uint8Array([
          5, 69, 248, 173, 171, 254, 19, 253, 143, 140, 146, 174, 26, 128, 3,
          52, 106, 55, 112, 245, 62, 127, 42, 93, 0, 81, 47, 177, 30, 25, 39,
          70,
        ]);
        var y = new FastBN(bytes, 16, 'le');
        var point = curve.pointFromY(y, true);
        var target =
          '2cd591ae3789fd62dc420a152002f79973a387eacecadc6a9a00c1a89488c15d';
        point.getX().toString(16);
      },
      () => {
        var curve = new bn_elliptic.curve.edwards({
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

        var bytes = new Uint8Array([
          5, 69, 248, 173, 171, 254, 19, 253, 143, 140, 146, 174, 26, 128, 3,
          52, 106, 55, 112, 245, 62, 127, 42, 93, 0, 81, 47, 177, 30, 25, 39,
          70,
        ]);
        var y = new BN(bytes, 16, 'le');
        var point = curve.pointFromY(y, true);
        var target =
          '2cd591ae3789fd62dc420a152002f79973a387eacecadc6a9a00c1a89488c15d';
        point.getX().toString(16);
      }
    );
    console.log('test curve 2 - result: ' + time);
  }

  {
    const fastRed = FastBN.red('k256');
    const fast = new FastBN(
      '79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798',
      16
    ).toRed(fastRed);
    const slowRed = BN.red('k256');
    const slow = new BN(
      '79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798',
      16
    ).toRed(slowRed);
    console.log('redInvm');
    let time = cmp(
      () => {
        fast.redInvm();
      },
      () => {
        slow.redInvm();
      }
    );
    console.log('redInvm - result: ' + time);
  }
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
