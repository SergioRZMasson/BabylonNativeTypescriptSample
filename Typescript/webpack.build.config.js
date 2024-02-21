const path = require("path");
const TerserPlugin = require("terser-webpack-plugin");

module.exports = {
  entry: "./src/index.ts",
  mode: "production",
  watch: false,
  optimization: {
    minimize: true,
    minimizer: [
      new TerserPlugin({
        terserOptions: {
          format: {
            comments: false,
          },
        },
        extractComments: false,
      }),
    ],
  },
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
            plugins: [
              require.resolve("@babel/plugin-transform-optional-chaining"),
              require.resolve("@babel/plugin-transform-nullish-coalescing-operator"),
            ],
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