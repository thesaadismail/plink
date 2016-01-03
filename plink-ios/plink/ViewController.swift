//
//  ViewController.swift
//  Plink
//
//  Created by Saad Ismail on 11/22/15.
//  Copyright © 2015 Saad Ismail. All rights reserved.
//

import Foundation
import UIKit
import SwiftColors

class ViewController: UIViewController, UITableViewDelegate, UITableViewDataSource {
    
    @IBOutlet weak var navigationBar: UINavigationBar!
    @IBOutlet weak var tableView: UITableView!
    
    var genericTimesData = [[MedTime]]();
    var genericTimesDataTitles = [String]();
    var genericTimesDataResults = Set<MedTime>();
    
    //MARK: - LIFECYCLE
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        setupNavigationBar();
        setupData();
        retrieveServerData()
    }
    
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    //MARK: - SETUP
    //MARK: Data
    func setupData() {
        var morningTimes = [MedTime]()
        morningTimes = setupTimeSectionArray(5,endRange: 11,timesArr: morningTimes);
        
        var afternoonTimes = [MedTime]()
        afternoonTimes = setupTimeSectionArray(12,endRange: 16,timesArr: afternoonTimes);
        
        var eveningTimes = [MedTime]()
        eveningTimes = setupTimeSectionArray(17,endRange: 20,timesArr: eveningTimes);
        
        var nightTimes = [MedTime]()
        nightTimes = setupTimeSectionArray(21, endRange: 23, timesArr: nightTimes);
        nightTimes = setupTimeSectionArray(0, endRange: 4, timesArr: nightTimes);
        
        genericTimesData.append(morningTimes);
        genericTimesData.append(afternoonTimes);
        genericTimesData.append(eveningTimes);
        genericTimesData.append(nightTimes);
        
        genericTimesDataTitles.append("Morning");
        genericTimesDataTitles.append("Afternoon");
        genericTimesDataTitles.append("Evening");
        genericTimesDataTitles.append("Night");
    }
    
    func setupTimeSectionArray(startRange: Int, endRange: Int, var timesArr: [MedTime]) -> [MedTime] {
        for index in startRange...endRange {
            timesArr.append(MedTime.init(hourInLocal: index, minuteInLocal: 0));
            timesArr.append(MedTime.init(hourInLocal: index, minuteInLocal: 30));
        }
        
        return timesArr;
    }
    
    //MARK: Navigation Bar
    func setupNavigationBar() {
        let statusBarColor = UIColor(hexString: "4169E1")
        let navItem = UINavigationItem.init(title: "PLINK")
        //let addButton = UIBarButtonItem.init(title: "Add", style: UIBarButtonItemStyle.Plain, target: self, action: "addMedication")
        //let settingsButton = UIBarButtonItem.init(title: "Settings", style: UIBarButtonItemStyle.Plain, target: self, action: "showSettingsScreen")
        //navItem.rightBarButtonItem = settingsButton
        //navItem.leftBarButtonItem = nil
        navigationBar.pushNavigationItem(navItem, animated: false)
        navigationBar.barTintColor = statusBarColor
        navigationBar.tintColor = UIColor.whiteColor()
        
        let view = UIView(frame: CGRect(x: 0.0, y: 0.0, width: UIScreen.mainScreen().bounds.size.width, height: 20.0))
        view.backgroundColor = statusBarColor
        self.view.addSubview(view)
    }
    
    //MARK: Sync
    func retrieveServerData() {
        PSManager.sharedManager.retrieveTimes({ (result) -> Void in
            print(result)
            self.genericTimesDataResults = result;
            self.tableView.reloadData()
            }, error: { (error) -> Void in
                print(error)
        })
    }
    
    //MARK: - TABLE VIEW DELEGATE/DATA SOURCE
    func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return genericTimesDataTitles.count
    }
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return genericTimesData[section].count
    }
    
    func tableView(tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        return genericTimesDataTitles[section]
    }
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell:MedTimeTableViewCell = self.tableView.dequeueReusableCellWithIdentifier("MedTimeTableViewCellID") as! MedTimeTableViewCell
        
        let medTimeForRow = (genericTimesData[indexPath.section] as [MedTime])[indexPath.row]
        cell.medTimeLabel.text = medTimeForRow.description
        cell.medTimeObj = medTimeForRow
        
        if(self.genericTimesDataResults.contains(cell.medTimeObj)) {
            cell.backgroundColor = UIColor.cyanColor();
        }
        else {
            cell.backgroundColor = UIColor.whiteColor();
        }
        
        return cell
    }
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
        let medTimeForRow = (genericTimesData[indexPath.section] as [MedTime])[indexPath.row]
        let wasSelected: Bool = self.genericTimesDataResults.contains(medTimeForRow)
        
        if(wasSelected) {
            self.genericTimesDataResults.remove(medTimeForRow)
        }
        else {
            self.genericTimesDataResults.insert(medTimeForRow)
        }
        
        self.tableView.reloadData()
        PSManager.sharedManager.setTimes(self.genericTimesDataResults)
    }
}

