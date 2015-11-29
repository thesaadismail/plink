//
//  NSDateExtension.swift
//  Plink
//
//  Created by Saad Ismail on 11/25/15.
//  Copyright © 2015 Saad Ismail. All rights reserved.
//

import Foundation

extension NSDate
{
    convenience
    init(dateString:String) {
        let dateStringFormatter = NSDateFormatter()
        dateStringFormatter.dateFormat = "yyyy-MM-dd"
        dateStringFormatter.locale = NSLocale(localeIdentifier: "en_US_POSIX")
        let d = dateStringFormatter.dateFromString(dateString)!
        self.init(timeInterval:0, sinceDate:d)
    }
    
    convenience
    init(timeString:String) {
        let timeStringFormatter = NSDateFormatter()
        timeStringFormatter.dateFormat = "HH:mm:ss"
        timeStringFormatter.locale = NSLocale(localeIdentifier: "en_US_POSIX")
        let d = timeStringFormatter.dateFromString(timeString)!
        self.init(timeInterval:0, sinceDate:d)
    }
}