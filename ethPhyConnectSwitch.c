#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MDIOPATH1 "/sys/devices/platform/fe300000.ethernet/mdio_bus/stmmac-0/stmmac-0:00/phy_registers"
#define MDIOPATH2 "/sys/devices/platform/fe300000.ethernet/mdio_bus/stmmac-0/stmmac-0:01/phy_registers"
#define MDIOPATH3 "/sys/devices/platform/fe300000.ethernet/mdio_bus/stmmac-0/stmmac-0:02/phy_registers"
#define MDIOPATH4 "/sys/devices/platform/fe300000.ethernet/mdio_bus/stmmac-0/stmmac-0:03/phy_registers"
#define MDIOPATH5 "/sys/devices/platform/fe300000.ethernet/mdio_bus/stmmac-0/stmmac-0:04/phy_registers"
#define SWITCHPHYPATH "/sys/devices/platform/fe300000.ethernet/rocktech_netdevice"

static int current_phy=0;
static int prev_phy=0;
static int temp_phy=0;

int readPhyStatusFileByLines(const char* filename)
{
	char buffer[15];
	FILE *fin = NULL;
	
	fin = fopen(filename,"r");
	if (fin == NULL)
		return -1;
	
	fgets(buffer, 15, fin);
	fseek(fin, strlen(buffer)+1, SEEK_SET);
	fgets(buffer, 15, fin);
	fclose(fin);
	
	if (strstr(buffer, "0x782d") != NULL)
		return 0;
	else
		return -1;
}

int judgePhyStatus(int record)
{
	int count = -1;
	
	switch (record) 
	{
		case 0:
			if (readPhyStatusFileByLines(MDIOPATH1) == 0)
				count = 0;
			else
				count = -1;
			break;
		case 1:
			if (readPhyStatusFileByLines(MDIOPATH2) == 0)
				count = 1;
			else
				count = -1;
			break;
		case 2:
			if (readPhyStatusFileByLines(MDIOPATH3) == 0)
				count = 2;
			else
				count = -1;
			break;
		case 3:
			if (readPhyStatusFileByLines(MDIOPATH4) == 0)
				count = 3;
			else
				count = -1;
			break;
		case 4:
			if (readPhyStatusFileByLines(MDIOPATH5) == 0)
				count = 4;
			else
				count = -1;
			break;
	}
	return count;
}

int getActivePhyNum(void)
{
	int ret = 0;
	int tmp = 0;
	
	for (tmp=0; tmp<5; tmp++) {
		ret = judgePhyStatus(tmp);
		if (ret != -1)
			break;
	}
	return ret;	
}

void switchPhyNum(int phynum)
{
	switch (phynum)
	{
		case 0:
			system("echo 0 > /sys/devices/platform/fe300000.ethernet/rocktech_netdevice");
			break;
		case 1:
			system("echo 1 > /sys/devices/platform/fe300000.ethernet/rocktech_netdevice");
			break;
		case 2:
			system("echo 2 > /sys/devices/platform/fe300000.ethernet/rocktech_netdevice");
			break;
		case 3:
			system("echo 3 > /sys/devices/platform/fe300000.ethernet/rocktech_netdevice");
			break;
		case 4:
			system("echo 4 > /sys/devices/platform/fe300000.ethernet/rocktech_netdevice");
			break;
	}
}

int main(int argc, char *argv[]) 
{
	printf("main function enter.\n");
	while(1)
	{
		temp_phy = getActivePhyNum();
		printf("check eth status. active phy num =%d\n",temp_phy);
		if ((temp_phy >= 0) && (temp_phy <= 4))
		{
			current_phy = temp_phy;
			//printf("current_phy =%d, prev_phy =%d\n", current_phy, prev_phy);
			if(current_phy != prev_phy)
			{
				switchPhyNum(current_phy);
				prev_phy = current_phy;
				sleep(1);
			}
		}
		
		sleep(10);
	}
	return 0;
}

