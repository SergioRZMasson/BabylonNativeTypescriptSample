const path = require("path");

module.exports = {
  entry: "./src/index.ts",
// mode: "production", // Enable this when shipping for better size.
  output: {
    path: path.resolve(__dirname, "build"),
    filename: "bundle.js",
  },
  module: {
    rules: [
      {
        include: /@babylonjs/,
        test: /\.js$/,
        use: {
          loader: "babel-loader",
          options: {
            presets: [['@babel/preset-env', { targets: "ie 11" }]]
          },
        },
      },
      {
        test: /\.ts$/,
        use: {
          loader: "ts-loader"
        },
      },
    ],
  },
};