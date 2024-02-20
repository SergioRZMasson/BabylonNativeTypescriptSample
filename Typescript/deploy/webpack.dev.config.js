const { DefinePlugin } = require("webpack");
const ChildProcess = require("child_process");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const path = require("path");
const fs = require("fs");

const package = require('../package.json');
const version = `${package.version}.${ChildProcess.execSync('git rev-list HEAD --count')}`.trim();
const appDirectory = fs.realpathSync(process.cwd());

module.exports = {
  entry: "./src/index.js",
  mode: "development",
  devServer: {
    open: true,
    watchFiles: ["./src/**/*", "../app/lib/**/*", "../docs/**/*"],
    static: path.resolve(appDirectory, "../docs"), //tells webpack to serve from the public folder
  },
  output: {
    filename: "bundle.js",
  },
  plugins: [
    new DefinePlugin({
      DEV_BUILD: true,
      VERSION: JSON.stringify(version),
    }),
    new HtmlWebpackPlugin({ title: "voxeltactics" }),
  ]
};