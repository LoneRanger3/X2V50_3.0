#include "WifiOpr.h"
#include <stdio.h>
#include <sstream> 
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "Log.h"

int CreateWifiParamFile(WIFI_PARAM_S* param)
{
    FILE* fp = fopen("/mnt/mtd/Config/hostapd.conf", "r");
	if (fp == NULL) {
		fp = fopen("/mnt/mtd/Config/hostapd.conf", "w+");
		if (!fp) {
			perror("fopen");
			return -1;
		}

		char buf[512] = { 0 };
		sprintf(buf, "interface=eth2\ndriver=nl80211\nctrl_interface=/var/hostapd\nssid=%s\nchannel=6\nieee80211n=1\nhw_mode=g\nignore_broadcast_ssid=0\nwpa=2\nrsn_pairwise=CCMP\nwpa_passphrase=%s\nmax_num_sta=1\nbeacon_int=50\ndtim_period=1\n", 
			param->name, param->password);
		fputs(buf, fp);
	}

	if (fp) {
		fclose(fp);
	}
}

int ChangeWifiParam(WIFI_PARAM_S* wifi_param, int level)
{
	if (!wifi_param) {
		XMLogE("wifi_param error!");
		return -1;
	}

	std::ifstream infile;
	infile.open("/mnt/mtd/Config/hostapd.conf", std::ios::in);
	if (!infile) {
		XMLogE("/mnt/mtd/Config/hostapd.conf open failed!");
		return -1;
	}
	std::string temp;
	std::string ssid;
	std::string password;
	int n = 0;
	while (getline(infile, temp, '\n')) {
		n++;
		if (4 == n)
			ssid = temp.substr(5);

		if (11 == n)
			password = temp.substr(15);
	}
	infile.close();

	XMLogI("Wifi name:%s, password:%s", wifi_param->name, wifi_param->password);
	FILE* fp = fopen("/mnt/mtd/Config/hostapd.conf", "w");
	if (fp) {
		if (1 == level) {
			ssid = wifi_param->name;
		}
		else if (2 == level) {
			password = wifi_param->password;
		}
		else {
			ssid = wifi_param->name;
			password = wifi_param->password;
		}
		char buf[256] = { 0 };
		sprintf(buf, "interface=eth2\ndriver=nl80211\nctrl_interface=/var/hostapd\nssid=%s\n", ssid.c_str());
		fputs(buf, fp);
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "channel=6\nieee80211n=1\nhw_mode=g\nignore_broadcast_ssid=0\nwpa=2\nrsn_pairwise=CCMP\nwpa_passphrase=%s\nmax_num_sta=1\nbeacon_int=50\ndtim_period=1\n", password.c_str());
		fputs(buf, fp);
		fclose(fp);
	}
	else {
		XMLogE("/mnt/mtd/Config/hostapd.conf open failed!");
		return -1;
	}

	return 0;
}