var dbConnectionStr = 'mongodb://127.0.0.1:27017/accounting';
var MongoClient = require('mongodb').MongoClient;
var collectionName = 'MedicationTimesCollection';
var medicationTimesAttribute = 'times';

function insertMedicationsTimes(timesStr) {
	MongoClient.connect(dbConnectionStr, 
		function(err, connection) {
			var collection = connection.collection(collectionName);
			collection.remove();

			collection.insert({'medicationTimes': timesStr}, function(err, count) {
					connection.close();
			});

		});
}

function retrieveMedicationTimes(callback){
	MongoClient.connect(dbConnectionStr, 
		function(err, connection) {
			var collection = connection.collection(collectionName);

			collection.find().toArray(function(err, documents) { 
					callback(documents[0].medicationTimes);
					connection.close();
			});

		});
}

exports.insertMedicationsTimes = insertMedicationsTimes;
exports.retrieveMedicationTimes = retrieveMedicationTimes;