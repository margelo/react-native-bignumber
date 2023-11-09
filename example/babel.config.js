const path = require('path');
const pak = require('../package.json');

module.exports = {
  presets: ['module:metro-react-native-babel-preset'],
  plugins: [
    [
      'module-resolver',
      {
        extensions: ['.js', '.ts', '.json', '.jsx', '.tsx'],
        alias: {
          [pak.name]: path.join(__dirname, '..', pak.source),
          bn_elliptic: path.join(__dirname, 'src', 'bn-elliptic', 'lib'),
          stream: 'stream-browserify',
          "buffer":"@craftzdog/react-native-buffer"
        },
      },
    ],
  ],
};
