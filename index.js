var scws = require("./build/Release/nscws.node");
var worker = scws.createWorker();
worker.createWorker = scws.createWorker;
module.exports = worker;
