//
//  MedTime.swift
//  Plink
//
//  Created by Saad Ismail on 11/23/15.
//  Copyright © 2015 Saad Ismail. All rights reserved.
//

import Foundation

class MedTime: Hashable, Equatable {
    //everything is in 24 hour time
    var hourInUTC: Int = 0;
    var minuteInUTC: Int = 0;
    
    init(hourInUTC:Int, minuteInUTC:Int) {
        self.hourInUTC = hourInUTC
        self.minuteInUTC = minuteInUTC
    }
    
    //mongodb user name  plinkdb
    init(hourInLocal:Int, minuteInLocal:Int) {
        var utcTimeHour = hourInLocal + 5
        if(utcTimeHour >= 24)
        {
            utcTimeHour -= 24;
        }
        
        hourInUTC = utcTimeHour;
        minuteInUTC = minuteInLocal;
        
    }
    
    func hourInLocalTime() -> Int {
        //TODO fix this. defaulting to eastern for now
        var localTimeHour = hourInUTC - 5
        if(localTimeHour < 0)
        {
            localTimeHour += 24;
        }
        
        return localTimeHour;
    }
    
    func minuteInLocalTime() -> Int {
        return minuteInUTC;
    }
    
    // MARK: Hashable, Equatable
    var hashValue: Int {
        let str: String = String(hourInUTC)+""+String(minuteInUTC)
        return Int(str)!
    }
    
    var description: String {
        let hourInt24 = hourInLocalTime();
        
        var hourInt12 = hourInt24;
        if(hourInt12 > 12) {
            hourInt12-=12;
        }
        
        let minuteInt = minuteInLocalTime();
        
        
        var minuteStr: String = "\(minuteInt)"
        var hourStr: String = "\(hourInt12)"
        
        var endStr = "am"
        
        if(hourInt24 >= 12) {
            endStr = "pm"
        }
        
        if(hourInt24 == 0) {
            hourStr = "12"
        }
        if(minuteInt == 0) {
           minuteStr = "00"
        }
        
        return "\(hourStr):\(minuteStr) \(endStr)"
        
    }
    
   
}

func ==(lhs: MedTime, rhs: MedTime) -> Bool {
    return lhs.hourInUTC == rhs.hourInUTC && lhs.minuteInUTC == rhs.minuteInUTC
}
