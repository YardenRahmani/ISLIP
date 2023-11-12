#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int get_p(char* pInput, double* pAddr) {
	// get p from cmd arguments
	for (char* ptr = pInput; *ptr != '\0'; ptr++) {
		if (*ptr < '0' || *ptr > '9') { // only non-digit allowed is decimal point
			if (*ptr == '.' && ptr == pInput + 1) {
				continue;
			}
			fprintf(stderr, "for p argument use the format x.y, where x is 0 or 1 and y is a positive integer\n");
			return -1;
		}
	}
	*pAddr = strtod(pInput, NULL); // save p in main function
	if (*pAddr > 1 || *pAddr < 0) {
		fprintf(stderr, "p must be a number between 0 and 1\n");
		return -1;
	}
	return 0;
}

int get_int(char* charInput, int* intAddr, const char* argName) {
	// get integer argument
	for (char* ptr = charInput; *ptr != '\0'; ptr++) {
		if (*ptr < '0' || *ptr > '9') { // only digits allowed
			fprintf(stderr, "%s must be a positive integer\n", argName);
			return -1;
		}
	}
	*intAddr = atoi(charInput); // save argument in main function
	return 0;
}

int check_flag(char* arg) {
	// check the diagonal flag
	if (strcmp(arg, "-d") == 0) {
		return 1;
	}
	else {
		fprintf(stderr, "wrong 5th argument, will be ignored\n");
		return 0;
	}
}

int get_rand_port(int totalPortNum, int arivalPort, int dFlag) {
	
	double res = (double)rand() / RAND_MAX; // get a random number between 0 and 1
	if (!dFlag) { // uniform distribution
		for (int i = 0; i < totalPortNum; i++) {
			if (res <= ((double)i + 1) / totalPortNum) { // divide the range of 0 to 1 to N equal steps
				return i; // if i/N < res < (i+1)/N we choose port i as destination
			}
		}
	}
	else { // diagonal distribution
		if (res < (double)2 / 3) { // 2/3 chance to get destination = arival
			return arivalPort;
		}
		else { // 1/3 chance to get destination = arival + 1
			return (arivalPort + 1) % totalPortNum;
		}
	}
}

int main(int argc, char* argv[]) {
	int seed, T, N, dFlag = 0;
	double p;
	if (argc < 5) {
		fprintf(stderr, "Too few arguments\n");
		exit(-1);
	}
	else if (argc > 6) {
		fprintf(stderr, "Too many arguments, some might be ignored\n");
	}
	if (get_p(argv[4], &p) != 0) exit(-1);
	if (get_int(argv[3], &seed, "seed") != 0) exit(-1);
	if (get_int(argv[2], &T, "T") != 0) exit(-1);
	if (get_int(argv[1], &N, "N") != 0) exit(-1);
	if (argc == 6) dFlag = check_flag(argv[5]);
	srand(atoi(argv[3]));
	for (int t = 0; t < T; t++) { // for T time steps
		for (int port = 0; port < N; port++) { // for each port
			if ((double)rand() / RAND_MAX < p) { // get a random number between 0 and 1,
				// and check if its lower then p (= a chance of p)
				fprintf(stdout, "%d %d %d\n", t, port, get_rand_port(N, port, dFlag));
			}
		}
	}
	exit(0);
}