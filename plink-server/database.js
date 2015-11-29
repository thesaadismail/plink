//var dbConnectionStr = "mongodb://127.0.0.1:27017/accounting";
var dbConnectionStr = process.env.MONGO_URL;
var MongoClient = require('mongodb').MongoClient;
var collectionName = 'MedicationTimesCollection';
var medicationTimesAttribute = 'times';

function setupDB() {

	MongoClient.connect(dbConnectionStr, 
		function(err, connection) {
			connection.createCollection(collectionName, function(err, collection){
				if (err) throw err;

				console.log("Created "+collectionName);
				//console.log(collection);
			});
		});
}

function insertMedicationsTimes(timesStr) {
	MongoClient.connect(dbConnectionStr, 
		function(err, connection) {
			var collection = connection.collection(collectionName);
			collection.remove({} , function(err, result) {
				console.log("Removing All Entries")
				collection.insert({'medicationTimes': timesStr}, function(err, count) {
					connection.close();
				});
			});

			

		});
}

function retrieveMedicationTimes(callback){
	MongoClient.connect(dbConnectionStr, 
		function(err, connection) {
			var collection = connection.collection(collectionName);

			collection.find().toArray(function(err, documents) { 
				if(documents.length == 0) {
					callback("NO DATA AVAILABLE")
				}
				else {
					callback(documents[0].medicationTimes);
				}
				connection.close();
			});

		});
}

exports.setupDB = setupDB;

exports.insertMedicationsTimes = insertMedicationsTimes;
exports.retrieveMedicationTimes = retrieveMedicationTimes;