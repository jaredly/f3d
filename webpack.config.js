const path = require('path');

module.exports = {
  entry: './lib/js/src/main.js',
  resolve: {
    alias: {
      bs: path.join(__dirname, 'lib', 'js', 'src'),
      js: path.join(__dirname, 'js'),
    }
  },
  output: {
    path: path.join(__dirname, "public"),
    filename: 'bundle.js',
  },
  module: {
    loaders: [
      {test: /\.json$/, loader: 'json-loader'}
    ]
  }
};
