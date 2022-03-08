import type { TestResult } from './TestResult';
import { rootSuite, describe, it } from './MochaRNAdapter';
import 'mocha';
import type * as MochaTypes from 'mocha';
import chai from 'chai';
import { BN } from 'react-native-bignumber';

export async function testLib(addTestResult: (testResult: TestResult) => void) {
  console.log('setting up mocha');

  const {
    EVENT_RUN_BEGIN,
    EVENT_RUN_END,
    EVENT_TEST_FAIL,
    EVENT_TEST_PASS,
    EVENT_SUITE_BEGIN,
    EVENT_SUITE_END,
  } = Mocha.Runner.constants;

  var runner = new Mocha.Runner(rootSuite) as MochaTypes.Runner;

  let indents = 0;
  const indent = () => Array(indents).join('  ');
  runner
    .once(EVENT_RUN_BEGIN, () => {})
    .on(EVENT_SUITE_BEGIN, () => {
      indents++;
    })
    .on(EVENT_SUITE_END, () => {
      indents--;
    })
    .on(EVENT_TEST_PASS, (test: MochaTypes.Runnable) => {
      addTestResult({
        name: test.fullTitle(),
        key: Math.random().toString(),
        status: 'correct',
      });
      console.log(`${indent()}pass: ${test.fullTitle()}`);
    })
    .on(EVENT_TEST_FAIL, (test: MochaTypes.Runnable, err: Error) => {
      addTestResult({
        name: test.fullTitle(),
        key: Math.random().toString(),
        status: 'incorrect',
        errorMsg: err.message,
      });
      console.log(
        `${indent()}fail: ${test.fullTitle()} - error: ${err.message}`
      );
    })
    .once(EVENT_RUN_END, () => {});

  describe('basic tests', () => {
    it('basic constructor and to String', () => {
      var a = new BN('10', 10);
      chai.expect(a.toString(10)).to.be.eql('10');
    });
    it('basic add', () => {
      var a = new BN('dead', 16);
      var b = new BN('101010', 2);

      var res = a.add(b);
      chai.expect(res.toString(10)).to.be.eql('57047');
    });
  });

  runner.run();
}
