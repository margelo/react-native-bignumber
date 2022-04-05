"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});

var _BigNumber = require("./BigNumber");

Object.keys(_BigNumber).forEach(function (key) {
  if (key === "default" || key === "__esModule") return;
  if (key in exports && exports[key] === _BigNumber[key]) return;
  Object.defineProperty(exports, key, {
    enumerable: true,
    get: function () {
      return _BigNumber[key];
    }
  });
});
//# sourceMappingURL=index.js.map