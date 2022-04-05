"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.BigNumber = void 0;

var _NativeBigNumber = require("./NativeBigNumber/NativeBigNumber");

async function runAsync() {
  return _NativeBigNumber.NativeBigNumber.runAsync();
}

const BigNumber = {
  runAsync
};
exports.BigNumber = BigNumber;
//# sourceMappingURL=BigNumber.js.map