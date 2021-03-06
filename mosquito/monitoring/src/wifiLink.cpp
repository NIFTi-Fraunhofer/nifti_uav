/*
 * wifiLink.cpp
 *
 *  Created on: Jan 21, 2011
 *      Author: Thorsten Linder
 *      note: requires installed "iwconfig" package
 */

#include <ros/ros.h>
#include <monitoring_msgs/WiFi.h>
#include <diagnostic_msgs/DiagnosticStatus.h>
#include <diagnostic_msgs/DiagnosticArray.h>
#include <diagnostic_msgs/KeyValue.h>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

monitoring_msgs::WiFi wifi;

diagnostic_msgs::DiagnosticArray diagArray;
std::stringstream floatStringHelper;

void setDefaultValues() {
    wifi.interfaceName = "";
    wifi.networkProtocol = "";
    wifi.essid = "";
    wifi.mode = "";
    wifi.frequency = "";
    wifi.apMac = "";
    wifi.bitRate = 0;
    wifi.txdBm = 0;
    wifi.txPower = 0;
    wifi.linkQuality = 0;
    wifi.signalLevel = 0;
    wifi.signalPower = 0;

    diagArray.status.clear();

    return;
}

diagnostic_msgs::DiagnosticStatus getLinkQualityStatus() {
    diagnostic_msgs::DiagnosticStatus linkQualityStatus;

    linkQualityStatus.name = "linkQuality";
    linkQualityStatus.message = "The network link quality is ";

    if (wifi.linkQuality > 0.75) {
	linkQualityStatus.level = 0;
	linkQualityStatus.message += "OK";
    }
    else if (wifi.linkQuality >= 0.5) {
	linkQualityStatus.level = 1;
	linkQualityStatus.message += "moderate";
    }
    else if (wifi.linkQuality < 0.5) {
	linkQualityStatus.level = 2;
	linkQualityStatus.message += "weak";
    }
    linkQualityStatus.hardware_id = wifi.interfaceName;
    linkQualityStatus.values.resize(3);

    floatStringHelper.clear();
    floatStringHelper << wifi.linkQuality;

    linkQualityStatus.values[0].key = "linkQuality";
    floatStringHelper >> linkQualityStatus.values[0].value;
    linkQualityStatus.values[1].key = "interface";
    linkQualityStatus.values[1].value = wifi.interfaceName;
    linkQualityStatus.values[2].key = "accesPoint";
    linkQualityStatus.values[2].value = wifi.apMac;
    return linkQualityStatus;
}

diagnostic_msgs::DiagnosticStatus getSignalLevelStatus() {
    diagnostic_msgs::DiagnosticStatus signalLevelStatus;

    signalLevelStatus.name = "receiverLevel";
    signalLevelStatus.message = "The network receiver Level is ";
    signalLevelStatus.hardware_id = wifi.interfaceName;

    if(0==wifi.signalLevel){
	signalLevelStatus.level = 2;
	signalLevelStatus.message += "broken";
    }else if (wifi.signalLevel > -60) {
	signalLevelStatus.level = 0;
	signalLevelStatus.message += "OK";
    }
    else if (wifi.signalLevel >= -70) {
	signalLevelStatus.level = 1;
	signalLevelStatus.message += "moderate";
    }
    else {
	signalLevelStatus.level = 2;
	signalLevelStatus.message += "weak";
    }

    signalLevelStatus.hardware_id = wifi.interfaceName;
    signalLevelStatus.values.resize(3);
    floatStringHelper.clear();
    floatStringHelper << wifi.signalLevel;
    signalLevelStatus.values[0].key = "signalLevel";
    floatStringHelper >> signalLevelStatus.values[0].value;

    floatStringHelper.clear();
    floatStringHelper << wifi.signalPower;
    signalLevelStatus.values[1].key = "signalPower";
    floatStringHelper >> signalLevelStatus.values[1].value;
    signalLevelStatus.values[2].key = "interface";
    signalLevelStatus.values[2].value = wifi.interfaceName;

    return signalLevelStatus;
}

diagnostic_msgs::DiagnosticStatus getTxLevelStatus() {
    diagnostic_msgs::DiagnosticStatus txLevelStatus;
    txLevelStatus.name = "transmitterLevel";
    txLevelStatus.message = "The network transmitter Level is ";

    if (wifi.txdBm >= 15) {
	txLevelStatus.level = 0;
	txLevelStatus.message += "OK";
    }
    else {
	txLevelStatus.level = 2;
	txLevelStatus.message += "weak";
    }
    txLevelStatus.hardware_id = wifi.interfaceName;
    txLevelStatus.values.resize(3);
    floatStringHelper.clear();
    floatStringHelper << wifi.txdBm;
    txLevelStatus.values[0].key = "txdBm";
    floatStringHelper >> txLevelStatus.values[0].value;
    floatStringHelper.clear();
    floatStringHelper << wifi.txPower;
    txLevelStatus.values[1].key = "txPower";
    floatStringHelper >> txLevelStatus.values[1].value;
    txLevelStatus.values[2].key = "interface";
    txLevelStatus.values[2].value = wifi.interfaceName;
    return txLevelStatus;
}

diagnostic_msgs::DiagnosticStatus getConnectionStatus() {
    diagnostic_msgs::DiagnosticStatus connectionStatus;
    connectionStatus.name = "connectionStatus";
    connectionStatus.message = "The network connection is ";

    if (("" != wifi.interfaceName) && ("Not-Associated" != wifi.apMac)) {
	if ("Managed" == wifi.mode) {
	    connectionStatus.level = 0;
	    connectionStatus.message += "OK";
	}
	else {
	    connectionStatus.level = 1;
	    connectionStatus.message += "Un-Managed";
	}

    }
    else {
	connectionStatus.level = 2;
	connectionStatus.message += "broken";
    }
    connectionStatus.hardware_id = wifi.interfaceName;
    connectionStatus.values.resize(4);
    connectionStatus.values[0].key = "accesPoint";
    connectionStatus.values[0].value = wifi.apMac;
    connectionStatus.values[1].key = "essid";
    connectionStatus.values[1].value = wifi.essid;
    connectionStatus.values[2].key = "networkProtocol";
    connectionStatus.values[2].value = wifi.networkProtocol;
    connectionStatus.values[3].key = "frequency";
    connectionStatus.values[3].value = wifi.frequency;
    connectionStatus.values[3].key = "mode";
    connectionStatus.values[3].value = wifi.mode;
    return connectionStatus;
}

diagnostic_msgs::DiagnosticStatus getBitRateStatus() {
    diagnostic_msgs::DiagnosticStatus bitRateStatus;

    bitRateStatus.name = "bitRate";
    bitRateStatus.message = "The bit rate status is ";
    if (48 <= wifi.bitRate) {
	bitRateStatus.level = 0;
	bitRateStatus.message += "OK";
    }
    else if (11 <= wifi.bitRate) {
	bitRateStatus.level = 1;
	bitRateStatus.message += "moderate";
    }
    else {
	bitRateStatus.level = 2;
	bitRateStatus.message += "weak";
    }
    bitRateStatus.hardware_id = wifi.interfaceName;

    bitRateStatus.values.resize(1);
    floatStringHelper.clear();
    floatStringHelper << wifi.bitRate;
    bitRateStatus.values[0].key = "bitRate";
    floatStringHelper >> bitRateStatus.values[0].value;

    return bitRateStatus;
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "wifiLink");
    ros::NodeHandle nodeHandle;
    ros::Publisher wifi_pub = nodeHandle.advertise<monitoring_msgs::WiFi> ("/monitoring/wifiLink", 10);
    ros::Publisher diagnostic_pub = nodeHandle.advertise<diagnostic_msgs::DiagnosticArray> ("diagnostics", 100);

    string hz = "1.0";
    if (nodeHandle.getParam("/monitoring_wifiLink/wifiLink/hz", hz)) {
	ROS_INFO("Use WiFi info update frequency \"%s\" " , hz.c_str());
    }
    else {
	ROS_WARN("Using default WiFi info update frequency \"%s\"",hz.c_str());
    }
    ros::Rate loop_time(atof(hz.c_str()));

    ros::Duration(0.25).sleep();
    string interfaceName = "wlan0";
    if (nodeHandle.getParam("/monitoring_wifiLink/wifiLink/interface", interfaceName)) {
	ROS_INFO("Use WiFi interface \"%s\" " , interfaceName.c_str());
    }
    else {

	ROS_WARN("Using default WiFi interface \"%s\"",interfaceName.c_str());//..,.. Ask Mario ;-)
    }
    ros::Duration(0.25).sleep();

    std::string lastReadWord;
    string subpart;

    while (nodeHandle.ok()) {

	lastReadWord.clear();
	subpart.clear();
	setDefaultValues();
	subpart = "iwconfig " + interfaceName + " > wifiLink.txt";
	if (system(subpart.c_str())) {
	    ROS_ERROR("Could not recieve wifi info by calling iwconfig!");
		loop_time.sleep();
	}
	else {
	    subpart.clear();
	    ifstream wifiFile;
	    wifiFile.open("wifiLink.txt");
	    if (!wifiFile) {
		ROS_ERROR("Unable to open file wifiLink.txt");
	    }
	    else {
		if (wifiFile >> lastReadWord) {
		    wifi.interfaceName = lastReadWord;
		}

		while (wifiFile >> lastReadWord) {

		    if (0 == lastReadWord.compare("IEEE")) {
			subpart = lastReadWord;
			wifiFile >> lastReadWord;
			subpart += " ";
			subpart += lastReadWord;
			wifi.networkProtocol = subpart;
			subpart.clear();
		    }

		    if (0 == lastReadWord.substr(0, 6).compare("ESSID:")) {
			int first = lastReadWord.find("\"", 0);
			int last = lastReadWord.find("\"", first + 1);
			wifi.essid = lastReadWord.substr(first + 1, last - first - 1);
		    }

		    if (0 == lastReadWord.substr(0, 5).compare("Mode:")) {
			int first = lastReadWord.find(":", 0);
			int last = lastReadWord.size() - first;
			wifi.mode = lastReadWord.substr(first + 1, last - 1);
		    }

		    if (0 == lastReadWord.substr(0, 10).compare("Frequency:")) {
			int first = lastReadWord.find(":", 0);
			int last = lastReadWord.size() - first;
			wifi.frequency = lastReadWord.substr(first + 1, last - 1);
			wifiFile >> lastReadWord;
			wifi.frequency += " " + lastReadWord;
		    }

		    if (0 == lastReadWord.substr(0, 6).compare("Point:")) {
			wifiFile >> lastReadWord;
			wifi.apMac = lastReadWord;
			;
		    }

		    if (0 == lastReadWord.substr(0, 5).compare("Rate=")) {
			int first = lastReadWord.find("=", 0);
			int last = lastReadWord.size() - first;
			wifi.bitRate = atoi(lastReadWord.substr(first + 1, last).c_str());
			//						cout <<lastReadWord<<" "<< first << " " << last << " " << wifi.bitRate << endl;
		    }

		    if (0 == lastReadWord.substr(0, 9).compare("Tx-Power=")) {
			int first = lastReadWord.find("=", 0);
			int last = lastReadWord.size() - first;
			//If W is the power in Watt, the power in W is P= 10^(dBm/10)*0.001
			int dBm = atoi(lastReadWord.substr(first + 1, last).c_str());
			wifi.txdBm = dBm;
			wifi.txPower = pow(10, ((float) dBm / 10.0)) * 0.001;
		    }

		    if (0 == lastReadWord.substr(0, 8).compare("Quality=")) {
			int first = lastReadWord.find("=", 0);
			int middle = lastReadWord.find("/", first);
			int last = lastReadWord.size() - first;

			float numerator = atof(lastReadWord.substr(first + 1, middle).c_str());
			float denominator = atof(lastReadWord.substr(middle + 1, last).c_str());
			wifi.linkQuality = numerator / denominator;
		    }

		    if (0 == lastReadWord.substr(0, 6).compare("level=")) {
			int first = lastReadWord.find("=", 0);
			int last = lastReadWord.size() - first;
			float dBm = atoi(lastReadWord.substr(first + 1, last).c_str());
			wifi.signalLevel = dBm;
			wifi.signalPower = pow(10, ((float) dBm / 10.0)) * 0.001;
		    }
		}

		wifiFile.close();
	    }

	    if (remove("wifiLink.txt") != 0) {
		ROS_ERROR( "Error deleting wifiLink.txt file" );
	    }

	}

	wifi.header.stamp = ros::Time::now();
	wifi_pub.publish(wifi);
	diagArray.header.stamp = ros::Time::now();
	diagArray.header.frame_id = "base_link";
	diagArray.status.push_back(getLinkQualityStatus());
	diagArray.status.push_back(getSignalLevelStatus());
	diagArray.status.push_back(getTxLevelStatus());
	diagArray.status.push_back(getConnectionStatus());
	diagArray.status.push_back(getBitRateStatus());
	diagnostic_pub.publish(diagArray);

	ros::spinOnce();
	loop_time.sleep();

    }

}
