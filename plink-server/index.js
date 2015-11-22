var server = require("./server");
var router = require("./router");
var database = require("./database");
var requestHandlers = require("./requestHandlers");

var handle = {};
handle["/"] = requestHandlers.start;
handle["/start"] = requestHandlers.start;
handle["/upload"] = requestHandlers.upload;
handle["/show"] = requestHandlers.show;

handle["/getClockTimeForArduino"] = requestHandlers.getClockTimeForArduino;

handle["/retrieveTimes"] = requestHandlers.retrieveTimes;
handle["/setTimes"] = requestHandlers.setTimes;

server.start(router.route, handle);