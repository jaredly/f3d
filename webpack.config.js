const path = require('path');
const webpack = require('webpack');

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
  plugins: [
    new webpack.DefinePlugin({
      __DEV__: process.env.NODE_ENV !== "production",
    }),
  ],
  module: {
    loaders: [
      {test: /\.json$/, loader: 'json-loader'},
      {test: /\.css$/, loaders: ['style-loader', 'css-loader']}
    ]
  }
};
