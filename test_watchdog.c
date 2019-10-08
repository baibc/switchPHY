#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h> 
#include <linux/watchdog.h>
  

int main(int argc, char **argv)
{
	int fd = open("/dev/watchdog", O_WRONLY); //通过open来启动watchdog
	int ret = 0;
	int i = 0;
	
	if (fd == -1) {
		printf("watchdog open failed! errno(%s)\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	int interval = 10;
	if (argc >= 2) interval = atoi(argv[1]);

	int margin = 10;
	if (argc >= 3) margin = atoi(argv[2]);

	printf("started (interval %d, margin %d)!\n", interval, margin);
	
	int timeout = interval + margin;
	ret = ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
	if (ret) {
		printf("Failed to set timeout to %d: %s\n", timeout, strerror(errno));
		ret = ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
		if (ret) {
				printf("Failed to get timeout: %s\n", strerror(errno));
		} else {
				if (timeout > margin) {
					interval = timeout - margin;
				} else {
					interval = 1;
				}
				printf("Adjusted interval to timeout returned by driver:"
						" timeout %d, interval %d, margin %d\n",
						timeout, interval, margin);
		}
	}
	//while (1) {
	while(i < 3) {//only feed 50s for test
		ret = write(fd, "\0", 1); //通过write来喂狗
		if (ret != 1) {
			ret = -1;
			break;
		}
		i++;//for test
		sleep(10);
	}
	close(fd);
	return ret;
}