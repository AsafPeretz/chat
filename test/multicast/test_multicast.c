#include <stdio.h>
#include <unistd.h>
#include "multicast.h"

int main() {
	Multicast_Create("225.1.1.1", 1234, "ziv","ziv");
	Multicast_Create("226.1.1.1", 1234, "not", "not");

	sleep(1);
	Multicast_Destroy("ziv","ziv");

	sleep(1);
	Multicast_Destroy("not", "not");
	
	return 0;
}
