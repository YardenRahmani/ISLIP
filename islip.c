#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

enum matching {none, requested, granted, accepted};

typedef struct package {
	int arrivalTime;
	int arrivalPort;
	int destinationPort;
	struct package* next;
} package;

typedef struct inputPort {
	package** queueHead;
	int* size;
	int lastMatched;
} inputPort;

typedef struct outputPort {
	int lastMatched;
} outputPort;

int get_int(char* charInput, int* intAddr, const char* argName) {
	// get argument and save it in the main function
	for (char* ptr = charInput; *ptr != '\0'; ptr++) {
		if (*ptr < '0' || *ptr > '9') { // check validity
			fprintf(stderr, "%s must be a positive integer\n", argName);
			return -1;
		}
	}
	*intAddr = atoi(charInput);
	return 0;
}

FILE* setLog(int runId) {
	FILE* newFile = NULL;
	char* logName = NULL;
	int count = 0;

	// check how many chars required for the run number
	for (int temp = runId; temp > 0; temp = temp / 10) count++;
	// number of chars for file name is (chars for run number) + (length of .log) + ('\0')
	int logNameSize = count + strlen(".log") + 1;
	if (NULL == (logName = (char*)malloc(logNameSize * sizeof(char)))) {
		fprintf(stderr, "Failed to open log file\n");
		return NULL;
	}
	sprintf(logName, "%d.log", runId);
	if (NULL == (newFile = fopen(logName, "w"))) {
		fprintf(stderr, "Failed to open log file\n");
		return NULL;
	}
	free(logName);
	return newFile;
}

void writeLog(FILE* logFile, int curStep, inputPort* inputs, int N) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			fprintf(logFile, "%d %d %d ", curStep, i, j);
			fprintf(logFile, "%d\n", inputs[i].size[j]);
		}
	}

}

int set_ports(inputPort** inputsAddr, outputPort** outputsAddr, int N) {
	
	// generate N input ports
	if (NULL == (*inputsAddr = (inputPort*)malloc(N * sizeof(inputPort)))) return -1;
	// generate N output ports
	if (NULL == (*outputsAddr = (outputPort*)malloc(N * sizeof(outputPort)))) {
		free(*inputsAddr);
		return -1;
	} 
	for (int i = 0; i < N; i++) { // iterate over N input and output ports
		((*outputsAddr) + i)->lastMatched = N - 1; // initialize last granted to N-1, next priority will be index 0
		((*inputsAddr) + i)->lastMatched = N - 1; // initialize last accepted to N-1, next priority will be index 0
		// allocate N pointers to simulate N queues for each input
		if (NULL == (((*inputsAddr) + i)->queueHead = (package**)malloc(N * sizeof(package*)))) {
			// if some allocation failed - free all dynamic allocations
			for (int j = i - 1; j >= 0; j--) {
				free(((*inputsAddr) + j)->queueHead);
				free(((*inputsAddr) + j)->size);
			}
			free(*inputsAddr);
			free(*outputsAddr);
			return -1;
		}
		else {
			// initialize the N queues to be empty
			for (int j = 0; j < N; j++) {
				(((*inputsAddr)[i]).queueHead)[j] = NULL;
			}
		}
		if (NULL == (((*inputsAddr) + i)->size = (int*)malloc(N * sizeof(int)))) {
			// if some allocation failed - free all dynamic allocations
			for (int j = i; j >= 0; j--) {
				free(((*inputsAddr) + j)->queueHead);
				if (j != i) {
					free(((*inputsAddr) + j)->size);
				}
			}
			free(*inputsAddr);
			free(*outputsAddr);
			return -1;
		}
		else {
			// initialize size of all queues to 0
			for (int j = 0; j < N; j++) {
				(((*inputsAddr)[i]).size)[j] = 0;
			}
		}
	}
	return 0;
}

void free_ports(inputPort* inputs, outputPort* outputs, int N) {
	// free all input and output ports
	for (int i = 0; i < N; i++) {
		free((*(inputs + i)).queueHead);
		free((*(inputs + i)).size);
	}
	free(inputs);
	free(outputs);
}

void add_package_queue(inputPort* curInputPort, package* packagePtr) {
	// fifo insert the package to the desired output queue of the input port
	package* desQueue = curInputPort->queueHead[packagePtr->destinationPort];

	if (desQueue == NULL) {
		curInputPort->queueHead[packagePtr->destinationPort] = packagePtr;
	}
	else {
		package* iter = desQueue;
		while (iter->next != NULL) iter = iter->next;
		iter->next = packagePtr;
	}
	curInputPort->size[packagePtr->destinationPort]++; // adjust queue size
}

package* read_line(int* flagPtr) {
	// read a line from stdin and return the data in package struct
	package* newPackage = NULL;
	char c;
	char* tempStr = NULL;
	int words = 0, wordLen = 0, tempLen = 10;

	if (NULL == (tempStr = (char*)malloc(tempLen*sizeof(char)))) {
		fprintf(stderr, "Allocation for a package failed, package dropped\n");
		return NULL;
	}
	if (NULL == (newPackage = (package*)malloc(sizeof(package)))) {
		fprintf(stderr, "Allocation for a package failed, package dropped\n");
		free(tempStr);
		return NULL;
	}
	while (1) {
		if (-1 == (c = getc(stdin))) {
			*flagPtr = 0; // mark end of file
			free(tempStr);
			free(newPackage);
			return NULL;
		}
		if (c == ' ') { // end of a word
			tempStr[wordLen] = '\0';
			// if its the first space, its the arrival time, else its the arrival port
			if (words == 0) newPackage->arrivalTime = atoi(tempStr);
			else newPackage->arrivalPort = atoi(tempStr);
			words++;
			wordLen = 0;
			continue;
		}
		else if (c == '\n') {
			if (words == 0) { // empty lines
				free(tempStr);
				free(newPackage);
				return NULL;
			}
			tempStr[wordLen] = '\0';
			newPackage->destinationPort = atoi(tempStr); // last word in line is the des port
			free(tempStr);
			newPackage->next = NULL;
			return newPackage;
		}
		if (c < '0' || c > '9') { // non-digit input error
			fprintf(stderr, "Non-digit character in file, package dropped\n");
			free(tempStr);
			free(newPackage);
			return NULL;
		}
		if (tempLen < wordLen + 1) { // extend tempStr if needed
			wordLen = wordLen * 2;
			if (NULL == (tempStr = (char*)realloc(tempStr, wordLen))) {
				fprintf(stderr, "Allocation for a package failed, package dropped\n");
				free(newPackage);
				return NULL;
			}
		}
		tempStr[wordLen] = c;
		wordLen++;
	}
}

void request(inputPort* inputs, int N, enum matching** permutation, int* outBusy, int* inBusy) {
	inputPort curInput;
	package* curInputQueue;
	// fill the requests in an NxN matrix 
	for (int i = 0; i < N; i++) {
		if (inBusy[i] == 1) continue; // input already choosed in last islip iteration
		curInput = inputs[i];
		for (int j = 0; j < N; j++) {
			if (outBusy[j] == 1) continue; // output already choosed in last islip iteration
			curInputQueue = *(curInput.queueHead + j);
			if (curInputQueue != NULL) { // some packages available for current input to current output
				permutation[i][j] = requested;
			}
		}
	}
}

void grant(outputPort** outputsPtr, int N, enum matching** permutation) {
	outputPort curOutput;

	for (int j = 0; j < N; j++) {
		curOutput = (*outputsPtr)[j]; // iterate all N outputs
		int i = curOutput.lastMatched;
		// start from the next after last matched (do while)
		do {
			i++;
			if (i == N) i = 0; // make the iteration circular
			if (permutation[i][j] == requested) {
				permutation[i][j] = granted; // find the first request and grant it
				break; // after some request was granted, move on to next output
			}
		} while (i != curOutput.lastMatched); // search will stop after a full cycle if no requests
	}
}

void accept(inputPort** inputsPtr, outputPort** outputsPtr, int N, enum matching** permutation, int* outBusy, int* inBusy) {
	inputPort curInput;

	for (int i = 0; i < N; i++) {
		curInput = (*inputsPtr)[i]; // iterate all N inputs
		int j = curInput.lastMatched;
		// start from the next after last matched (do while)
		do {
			j++;
			if (j == N) j = 0; // make the iteration circular
			if (permutation[i][j] == granted) {
				permutation[i][j] = accepted; // find the first grant and accept it
				inBusy[i] = 1; // mark that input as busy for next iterations in current step
				outBusy[j] = 1; // mark that output as busy for next iterations in current step
				(*inputsPtr)[i].lastMatched = j; // update the input last match
				(*outputsPtr)[j].lastMatched = i; // update the output last match
				break; // after some grant was accepted, move on to next input
			}
		} while (j != curInput.lastMatched); // search will stop after a full cycle if no grants
	}
}

int sendPackages(inputPort* inputs, int N, enum matching** permutation, int step) {
	inputPort curInput;
	package* temp;
	int count = 0; // counter for number of sent packages in current step
	// iterate over the NxN array
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (permutation[i][j] == accepted) { // look for accepted connections
				// take the first package in queue and simulate it as scheduled
				curInput = inputs[i];
				temp = (curInput.queueHead)[j];
				(curInput.queueHead)[j] = temp->next;
				fprintf(stdout, "%d %d ", temp->arrivalTime, temp->arrivalPort);
				fprintf(stdout, "%d %d\n", temp->destinationPort, step);
				free(temp);
				count++;
				curInput.size[j]--; // update current queue size
				break;
			}
		}
	}
	return count;
}

int allocateIslip(enum matching*** permutation, int** inputBusyAddr, int** outputBusyAddr, int N) {

	// allocate N integers to indicate an input who is matched from previous iteration
	if (NULL == ((*inputBusyAddr) = (int*)malloc(N * sizeof(int)))) {
		fprintf(stderr, "Allocation failed, islip step missed\n");
		return -1;
	}
	// allocate N integers to indicate an output who is matched from previous iteration
	if (NULL == ((*outputBusyAddr) = (int*)malloc(N * sizeof(int)))) {
		fprintf(stderr, "Allocation failed, islip step missed\n");
		free(*inputBusyAddr);
		return -1;
	}
	// allocate NxN array to indicate requests, grants and accepts between N inputs and N outputs
	if (NULL == ((*permutation) = (enum matching**)malloc(N * sizeof(enum matching*)))) {
		fprintf(stderr, "Allocation failed, islip step missed\n");
		free(*inputBusyAddr);
		free(*outputBusyAddr);
		return -1;
	}
	for (int i = 0; i < N; i++) {
		if (NULL == (*((*permutation) + i) = (enum matching*)malloc(N * sizeof(enum matching)))) {
			fprintf(stderr, "Allocation failed, islip step missed\n");
			for (int j = i - 1; j >= 0; j--) {
				free((*permutation)[j]);
			}
			free((*permutation));
			free((*inputBusyAddr));
			free((*outputBusyAddr));
			return -1;
		}
		(*(*inputBusyAddr + i)) = 0; // set all N inputs as not matched
		(*(*outputBusyAddr + i)) = 0; // set all N outputs as not matched
	}
	return 0;
}

int islip(inputPort* inputs, outputPort* outputs, int N, int k, int step) {
	enum matching** permutation = NULL;
	int count = 0, * inputBusy = NULL, * outputBusy = NULL;
	// allocate arrays used for islip single step
	if (0 != allocateIslip(&permutation, &inputBusy, &outputBusy, N)) return 0;
	// iterate k times
	for (int i = 0; i < k; i++) {
		// reset all non-accepted requests
		for (int j = 0; j < N; j++) {
			for (int l = 0; l < N; l++) {
				if (permutation[j][l] != accepted) permutation[j][l] = none;
			}
		}
		request(inputs, N, permutation, inputBusy, outputBusy);
		grant(&outputs, N, permutation);
		accept(&inputs, &outputs, N, permutation, inputBusy, outputBusy);
	}
	count = sendPackages(inputs, N, permutation, step);
	// free arrays used for islip single step
	for (int j = 0; j < N; j++) {
		free(*(permutation + j));
	}
	free(permutation);
	free(inputBusy);
	free(outputBusy);
	return count;
}

void schedule(inputPort* inputs, outputPort* outputs, int N, int k, FILE* logFile) {
	int flag = 1, curStep = 0, packagesWaiting = 0;
	package* packagePtr = NULL, * iter = NULL;

	while (flag != 0) {
		// read a line from input file (get a package)
		packagePtr = read_line(&flag);
		// if null returned, file ended
		if (packagePtr == NULL) continue;
		// if the current new package arrival time is greater then the current step,
		// the simulation need to procceed with time (simulate steps) before adding the new package
		while (packagePtr->arrivalTime > curStep) {
			// preform one islip step, update the total number of packages waiting
			packagesWaiting -= islip(inputs, outputs, N, k, curStep);
			if (logFile != NULL) writeLog(logFile, curStep, inputs, N);
			curStep++;
		}
		add_package_queue(inputs + packagePtr->arrivalPort, packagePtr);
		packagesWaiting++;
	}
	// packages done arriving and now the switch need to finish scheduling waiting packages
	while (packagesWaiting > 0) {
		// preform one islip step, update the total number of packages waiting
		packagesWaiting -= islip(inputs, outputs, N, k, curStep);
		if (logFile != NULL) writeLog(logFile, curStep, inputs, N);
		curStep++;
	}
}

int main(int argc, char* argv[]) {
	int r, k, N, dFlag = 0;
	inputPort* inputs = NULL;
	outputPort* outputs = NULL;
	FILE* logFile = NULL;

	if (argc != 4) {
		fprintf(stderr, "Wrong amount of arguments\n");
		return -1;
	}
	// get and check validity of arguments
	if (get_int(argv[3], &r, "r") != 0) return -1;
	if (get_int(argv[2], &k, "k") != 0) return -1;
	if (get_int(argv[1], &N, "N") != 0) return -1;
	// allocate and initialize input and output ports
	if (0 != set_ports(&inputs, &outputs ,N)) {
		fprintf(stderr, "Virtual ports creation failed\n");
		return -1;
	}
	// open log file with appropriate name
	logFile = setLog(r);
	// main functionality
	schedule(inputs, outputs, N, k, logFile);
	// free allocated ports
	free_ports(inputs, outputs, N);
	// close log file
	if (logFile != NULL) fclose(logFile);
	return 0;
}