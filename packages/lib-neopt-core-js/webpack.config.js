//const webpack = require("webpack");
const path = require("path");

module.exports = {
  mode: "development",
  context: path.resolve(__dirname, "."),
  entry: "./src/index.autogen.js",
  output: {
    path: path.resolve(__dirname, "dist"),
    filename: "lib-neopt-core.js",
    library: 'Neo3CppLib',
    libraryTarget: 'umd'
  },
  node: {
	fs: "empty"
  },
  module: {
    rules: [
      {
        test: /neopt_raw_lib\.js$/,
        loader: "exports-loader"
      },
      {
        test: /neopt_raw_lib\.wasm$/,
        type: "javascript/auto",
        loader: "file-loader",
        options: {
          publicPath: "dist/"
        }
      }
    ]
  },
};