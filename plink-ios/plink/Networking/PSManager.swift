//
//  PSManager.swift
//  Plink
//
//  Created by Saad Ismail on 11/22/15.
//  Copyright © 2015 Saad Ismail. All rights reserved.
//

import Foundation
import AFNetworking

class PSManager {
    static let sharedManager = PSManager.init();
    
    var serverURL = "http://plink-55964.onmodulus.net"
    
    func retrieveTimes(completion:(result: Set<MedTime>) -> Void, error:(errorObj: NSError) -> Void) {
        let manager = AFHTTPRequestOperationManager.init()
        //manager.responseSerializer = AFJSONResponseSerializer(readingOptions: NSJSONReadingOptions.AllowFragments)
        //manager.responseSerializer.acceptableContentTypes =  Set(["text/plain"])
        manager.requestSerializer.cachePolicy = NSURLRequestCachePolicy.ReloadIgnoringLocalCacheData;
        manager.GET(serverURL+"/retrieveTimesJSON", parameters: nil,
            success: { (operation, responseObject) -> Void in
                guard let processedData = self.processTimeData(responseObject["data"] as? String) else {
                    print("error!")
                    return
                }
                
                completion(result: processedData);
            }, failure: { (operation, errorObjFromServer) -> Void in
                print(errorObjFromServer.description);
                error(errorObj: errorObjFromServer);
        })
    }
    
    func setTimes(data:Set<MedTime>) {
        let manager = AFHTTPRequestOperationManager.init()
        manager.requestSerializer.cachePolicy = NSURLRequestCachePolicy.ReloadIgnoringLocalCacheData;
        
        var serverMedTimesStr: String = ""
        for (index, medTimeObj) in data.enumerate() {
            
            if(index != 0) {
                serverMedTimesStr+="-"
            }
            
            if(medTimeObj.minuteInUTC == 0) {
                serverMedTimesStr+="\(medTimeObj.hourInUTC)0"
            }
            else {
                serverMedTimesStr+="\(medTimeObj.hourInUTC)1"
            }
            
            
        }
        
        manager.GET(serverURL+"/setTimes?data="+serverMedTimesStr, parameters: nil,
            success: { (operation, responseObject) -> Void in
                
            }, failure: { (operation, errorObjFromServer) -> Void in
                print(errorObjFromServer.description);
        })
    }
    
    func processTimeData(data: String?) -> Set<MedTime>? {
        guard let dataStr = data else {
            print("data form server is null");
            return nil;
        }
        
        var processedArr = Set<MedTime>()
        
        let timesArr = dataStr.componentsSeparatedByString("-");
        for medicationTimeStrRaw in timesArr {
            guard let medicationTimeIntRaw: Int = Int(medicationTimeStrRaw)! else {
                print("data form server is null");
                return nil;
            }
            
            let parsedMedicationTime: Int = medicationTimeIntRaw/10;
            
            if(medicationTimeIntRaw % 10 == 0)
            {
                //at the hour
                processedArr.insert(MedTime(hourInUTC: parsedMedicationTime, minuteInUTC:0))
            }
            else
            {
                //at the half hour
                processedArr.insert(MedTime(hourInUTC: parsedMedicationTime, minuteInUTC:30))
            }
        }
        
        return processedArr
    }
}