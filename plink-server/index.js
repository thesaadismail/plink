var server = require("./server");
var router = require("./router");
var database = require("./database");
var requestHandlers = require("./requestHandlers");

var now    = new Date();
console.log("=================================================")
console.log("APPLICATION HAS STARTED: "+now)
console.log("=================================================")

var handle = {};
handle["/"] = requestHandlers.start;
handle["/start"] = requestHandlers.start;
handle["/upload"] = requestHandlers.upload;
handle["/show"] = requestHandlers.show;

handle["/getClockTimeForArduino"] = requestHandlers.getClockTimeForArduino;

handle["/retrieveTimes"] = requestHandlers.retrieveTimes;
handle["/retrieveTimesJSON"] = requestHandlers.retrieveTimesJSON;

handle["/setTimes"] = requestHandlers.setTimes;

database.setupDB();
server.start(router.route, handle);