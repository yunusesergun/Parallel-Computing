/*
 * util.cpp
 *
 *  Created on: Oct 18, 2021
 *      Author: ali
 */

#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int read_file(char *file_name, int *numVertices, int *numEdges, int **offsets) {
	printf("Reading the file...\n");
	FILE *inputFile = fopen(file_name, "r");
	if (inputFile) {
		int success;

		success = fscanf(inputFile, "%d", &*numVertices);
		if (!success) {
			printf("Bad File format!\n");
			return 0;
		}
		success = fscanf(inputFile, "%d", &*numEdges);
		if (!success) {
			printf("Bad File format!\n");
			return 0;
		}

		//topologicalSize includes the edge weights
		int topologicalSize = (*numVertices + 1 + (*numEdges * 2)) * sizeof(int);

		printf("numVertices = %d, numEdges = %d\n", *numVertices, *numEdges);

		printf("Graph data footprint=%.3f KB ~ %.3f MB ~ %.3f GB\n", topologicalSize / 1024.0, topologicalSize / (1024.0*1024), topologicalSize / (1024.0*1024*1024) );

		*offsets = (int*) malloc(topologicalSize);

		for (int i = 0; i < (*numVertices + 1 + (*numEdges) * 2); i++) {
			success = fscanf(inputFile, "%d", &((*offsets)[i]));
			if (success == EOF || success == 0) {
				printf("Bad File format!\n");
				return 0;
			}
		}
		fclose(inputFile);
		return 1;
	}
	printf("Could not open the file!\n");
	return 0;
}

void printResults(char *fileName, int *distances, int numVertices) {
	FILE *inputFile = fopen(fileName, "w");
	if (inputFile) {
		//Output the final values
		for (int i = 0; i < numVertices; i++) {
			fprintf(inputFile, "%d\n", (int32_t) distances[i]);
		}
	} else {
		printf("Could not open the file!\n");
	}
}
