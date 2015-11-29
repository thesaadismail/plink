var exec = require("child_process").exec;
var fs = require('fs')
var querystring = require("querystring");
// var formidable = require("formidable");
var url = require('url');
var querystring = require('querystring');

var database = require("./database");

function start(response) {
	console.log("Request handler 'start' was called.");

	response.writeHead(200, {"Content-Type":"text/html"});

	getHTMLFile("./start.html", function(data) {
		response.write(data);
		response.end();
	});
}

function upload(response, request) {
	console.log("Request handler 'upload' was called.");

	response.writeHead(200, {"Content-Type":"text/html"});

	getHTMLFile("./start.html", function(data) {
		response.write(data);
		response.end();
	});

	// console.log("Request handler 'upload' was called.");

	// var form = new formidable.IncomingForm();
	// console.log("about to parse");
	// form.parse(request, function(error, fields, files) {
	// 	console.log("parsing done");
	// 	/* Possible error on Windows systems:
	// 	tried to rename to an already existing file */
	// 	fs.rename(files.upload.path, "/tmp/test.png", function(error) {
	// 		if (error) {
	// 			fs.unlink("/tmp/test.png");
	// 			fs.rename(files.upload.path, "/tmp/test.png");
	// 		}
	// 	});
	// 	response.writeHead(200, {"Content-Type": "text/html"});
	// 	response.write("received image:<br/>");
	// 	response.write("<img src='/show' />");
	// 	response.end();
	// });
}

function retrieveTimes(response) {
	console.log("Request handler 'retrieveTimes' was called.");
	response.writeHead(200, {"Content-Type":"text/html"});

	database.retrieveMedicationTimes(function(medicationTimes) {
		response.write(medicationTimes);
		response.end();
	});
	// fs.createReadStream("/tmp/test.png").pipe(response);
}

function retrieveTimesJSON(response) {
	console.log("Request handler 'retrieveTimesJSON' was called.");
	response.writeHead(200, {"Content-Type":"application/json"});

	database.retrieveMedicationTimes(function(medicationTimes) {
		response.write("{ \"data\": \""+medicationTimes+"\" }");
		response.end();
	});
	// fs.createReadStream("/tmp/test.png").pipe(response);
}

function getClockTimeForArduino(response) {
	console.log("Request handler 'getClockTimeForArduino' was called.");
	response.writeHead(200, {"Content-Type":"text/plain"});

	database.retrieveMedicationTimes(function(medicationTimes) {
		var timeZone = 0; //for UTC Time
		var now    = new Date();
		var hour   = now.getUTCHours();
   		var minute = now.getUTCMinutes();
   		var second = now.getUTCSeconds();

   		var month = now.getUTCMonth();
   		var days = now.getUTCDate();
   		var year = now.getUTCFullYear();

		response.write(hour+"-"+minute+"-"+second+"-"+days+"-"+month+"-"+year);
		response.end();
	});
	// fs.createReadStream("/tmp/test.png").pipe(response);
}

function setTimes(response, request) {
	console.log("Request handler 'setTimes' was called.");
	var splitStr = request.url.split('?')[1]
	var queryStr = querystring.parse(splitStr);
	var times = queryStr["data"];

	database.insertMedicationsTimes(times);

	response.writeHead(200, {"Content-Type":"text/plain"});

	database.retrieveMedicationTimes(function(medicationTimes) {
		//console.log(medicationTimes);
		response.write(medicationTimes);
		response.end();
	});
}

function show(response) {
	console.log("Request handler 'show' was called.");
	response.writeHead(200, {"Content-Type":"image/png"});
	fs.createReadStream("/tmp/test.png").pipe(response);
}

function getHTMLFile(filename, processData) {
	fs.readFile("./start.html", 'utf8', function(err, data) {
		if (err) throw err;
		processData(data);
	});
}

exports.start = start;
exports.upload = upload;
exports.retrieveTimes = retrieveTimes;
exports.retrieveTimesJSON = retrieveTimesJSON;
exports.setTimes = setTimes;
exports.show = show;
exports.getClockTimeForArduino = getClockTimeForArduino;