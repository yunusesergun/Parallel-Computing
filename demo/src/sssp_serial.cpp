//Skeleton SSSP serial

#include <iostream>
#include "util.h"

int main(int argc, char **argv) {
	if (argc != 4)
	{
		std::cout << "Wrong argments usage: sssp_serial [DATA_FILE] [sourceVertex] [OUTPUT_FILE]" << std::endl;
	}

	int numVertices, numEdges, *vertices, *edges, *weights;
	//Reading the input
	int success = read_file(argv[1], &numVertices, &numEdges, &vertices);
	if (success) {
		edges = &(vertices[numVertices + 1]);
		weights = &(vertices[numVertices + 1 + numEdges]);

        int sourceVertex = std::stoi(argv[2]);  
        int *resultDistances = new int[numVertices];
		int *resultDistances_temp = new int[numVertices];
		
		// Rows --> Vertices, Columns --> Edges, Value --> Weights

		/////////////////////////////////////////////////
		//         DO YOUR COMPUTATIONS      ////////////
		/////////////////////////////////////////////////
		
		for (int i=0; i<numVertices;i++)
		{
			if (i != sourceVertex)
			{
				resultDistances[i] = INT32_MAX;
			}
			else
			{
				resultDistances[i] = 0;
			}
		}

		int flag_diff =0;
		int bbb = 0;
		for (int i=0; i<numVertices-1; i++)
		{
			bbb = i;
			int weight_index = 0;
			int flag=0;
			for (int a=0; a<numVertices; a++)
			{
				resultDistances_temp[a] = resultDistances[a];
			}
			
			// Computing
			for (int k=0; k<numVertices; k++)
			{
				int min = INT32_MAX;	
				for (int j=0; j<vertices[k+1]-vertices[k];j++)
				{
					if (resultDistances[edges[weight_index]] != INT32_MAX && weights[weight_index] != INT32_MAX)
					{
						if (resultDistances[edges[weight_index]] + weights[weight_index] < min)
						{
							min = resultDistances[edges[weight_index]] + weights[weight_index];
						}
					}
					weight_index++;
				}
				if (resultDistances[k] < min)
				{
					min = resultDistances[k];
				}
				resultDistances[k] = min;
			}

			//Checking if there is any change
			for (int a=0; a<numVertices;a++)
			{
				if (resultDistances_temp[a] != resultDistances[a])
				{
					flag++;
					break;
				}
			}

			if (flag==0) //If there is no change, for loop is terminated
			{
				break;
			}
		}
		
        //Outputting the results
		printResults(argv[3], resultDistances, numVertices);

		//Free the memory
		delete vertices;
		delete resultDistances;
	}

	return 0;
}
