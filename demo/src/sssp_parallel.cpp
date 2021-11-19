//Skeleton SSSP parallel


#include <iostream>
#include "util.h"

#include <mpi.h>

int main(int argc, char **argv)
{
	// Initialize the MPI environment
	MPI_Init(&argc, &argv);

	if (argc != 4)
	{
		std::cout << "Wrong argments usage: mpirun -np X sssp_parallel [DATA_FILE] [sourceVertex] [OUTPUT_FILE]" << std::endl;
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}

	int sourceVertex = std::stoi(argv[2]);

	int numVertices, numEdges;
	int *vertices, *edges, *weights;
	
	int *resultDistances;
	int *resultDistances_temp;
	int *resultDistances_temp_other;
	
	// Get the rank of the calling process
	int world_rank;
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	//Read the file
	if (world_rank == 0)
	{
		int success = read_file(argv[1], &numVertices, &numEdges, &vertices);
		if (success)
		{
			edges = &(vertices[numVertices + 1]);
			weights = &(vertices[numVertices + 1 + numEdges]);
		}
		else
		{
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		}
	}
	//printf("hello");
	/////////////////////////////////////////////////
    //         DO YOUR COMPUTATIONS      ////////////
	/////////////////////////////////////////////////
	if (world_rank == 0)
	{
		int part_cnt = numVertices/size;
		part_cnt = part_cnt + 1; // Vertice part array size
		int *vertice_part = new int[part_cnt]; //Vertice part array

		for (int i=1; i<size; i++)
		{
			MPI_Send(&part_cnt, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&numVertices, 1, MPI_INT, i, 6, MPI_COMM_WORLD);
		}

		int index = vertices[part_cnt-1];

		// Send row, column and weight infos
		for (int i=1; i<size; i++)
		{
			// Vertice part array is created
			for (int j=0;j<part_cnt; j++)
			{
				vertice_part[j] = vertices[i*(part_cnt-1)+j];
			}

			// Edges part and weight part arrays are created
			int *edges_part = new int[vertice_part[part_cnt-1]-vertice_part[0]];
			int *weight_part = new int[vertice_part[part_cnt-1]-vertice_part[0]];

			// Edges part and weight part arrays are created
			for (int j=0;j<vertice_part[part_cnt-1]-vertice_part[0]; j++)
			{
				edges_part[j] = edges[index];
				weight_part[j] = weights[index];
				index++;
			}

			// Send edge part size
			int edge_size_main = (vertice_part[part_cnt-1]-vertice_part[0]);
			MPI_Send(&edge_size_main, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

			// Vertice part is sent
			MPI_Send(vertice_part, part_cnt, MPI_INT, i, 2, MPI_COMM_WORLD);
			MPI_Send(edges_part, edge_size_main, MPI_INT, i, 3, MPI_COMM_WORLD);
			MPI_Send(weight_part, edge_size_main, MPI_INT, i, 4, MPI_COMM_WORLD);
		}

		// For root proc. arrays should be created
		// Vertice part array is created
		for (int j=0;j<part_cnt; j++)
		{
			vertice_part[j] = vertices[j];
		}
		// Edges part and weight part arrays are created
		int *edges_part_root = new int[vertice_part[part_cnt-1]-vertice_part[0]];
		int *weight_part_root = new int[vertice_part[part_cnt-1]-vertice_part[0]];

		// Edges part and weight part arrays are created
		index = 0;
		for (int j=0;j<vertice_part[part_cnt-1]-vertice_part[0]; j++)
		{
			edges_part_root[j] = edges[index];
			weight_part_root[j] = weights[index];
			index++;
		}

		resultDistances = new int[numVertices];
		int *R_root = new int[part_cnt-1];
		// Result Distance array geneation
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

		// R array generation
		for (int i=0; i<part_cnt-1;i++)
		{
			R_root[i] = resultDistances[(part_cnt-1)*world_rank+i];
		}
		MPI_Barrier(MPI_COMM_WORLD);

		// Computation part
		resultDistances_temp = new int[numVertices]; // R temp array to end computation early
		
		for (int i=0; i<numVertices-1; i++)
		{
			int weight_index = 0;
			int flag=0;

			//Checking if there is any change
			for (int a=0; a<numVertices; a++)
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

			for (int a=0; a<numVertices; a++)
			{
				resultDistances_temp[a] = resultDistances[a];
			}

			for (int k=0; k<part_cnt-1; k++)
			{
				int min = INT32_MAX;	
				for (int j=0; j<vertice_part[k+1]-vertice_part[k];j++)
				{
					if (resultDistances[edges_part_root[weight_index]] != INT32_MAX && weight_part_root[weight_index] != INT32_MAX)
					{
						if (resultDistances[edges_part_root[weight_index]] + weight_part_root[weight_index] < min)
						{
							min = resultDistances[edges_part_root[weight_index]] + weight_part_root[weight_index];
						}
					}
					weight_index++;
				}
				if (R_root[k] < min)
				{
					min = R_root[k];
				}
				R_root[k] = min;
			}

			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Allgather(R_root, part_cnt-1, MPI_INT, resultDistances, part_cnt-1, MPI_INT, MPI_COMM_WORLD);
		    MPI_Barrier(MPI_COMM_WORLD);
		}
	}
	else
	{
		int numVertices_all=0;

		// Vertice size is taken
		int vertice_size=0;
		int edge_size=0;

		MPI_Recv(&vertice_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&numVertices_all, 1, MPI_INT, 0, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&edge_size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		resultDistances = new int[numVertices_all];
		int *R               = new int[vertice_size-1];
		int part_cnt_other = numVertices_all/size;

		// Result Distance array geneation
		for (int i=0; i<numVertices_all;i++)
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

		// R array generation
		for (int i=0; i<vertice_size-1;i++)
		{
			R[i] = resultDistances[part_cnt_other*world_rank+i];
		}

		// Vertice array is taken
		int *vertice_part_other = new int[vertice_size];
		MPI_Recv(vertice_part_other, vertice_size, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		// Edge array is taken
		int *edge_part_other = new int[edge_size];
		MPI_Recv(edge_part_other, edge_size, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// Weight array is taken
		int *weight_part_other = new int[edge_size];
		MPI_Recv(weight_part_other, edge_size, MPI_INT, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		MPI_Barrier(MPI_COMM_WORLD);

		resultDistances_temp_other = new int[numVertices_all]; // R temp array to end computation early
		
		for (int z=0; z<numVertices_all-1; z++)
		{
			int weight_index = 0;
			int flag_other=0;
			
			//Checking if there is any change
			for (int a=0; a<numVertices_all;a++)
			{
				if (resultDistances_temp_other[a] != resultDistances[a])
				{
					flag_other++;
					break;
				}
			}

			if (flag_other == 0) //If there is no change, for loop is terminated
			{
				break;
			}
			for (int a=0; a<numVertices_all; a++)
			{
				resultDistances_temp_other[a] = resultDistances[a];
			}

			// Computing
			for (int aa=0; aa<vertice_size-1; aa++)
			{
				int min = INT32_MAX;	
				for (int bb=0; bb<vertice_part_other[aa+1]-vertice_part_other[aa];bb++)
				{
					if (resultDistances[edge_part_other[weight_index]] != INT32_MAX && weight_part_other[weight_index] != INT32_MAX)
					{
						if (resultDistances[edge_part_other[weight_index]] + weight_part_other[weight_index] < min)
						{
							min = resultDistances[edge_part_other[weight_index]] + weight_part_other[weight_index];
						}
					}
					weight_index++;
				}
				if (R[aa] < min)
				{
					min = R[aa];
				}
				R[aa] = min;
			}

			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Allgather(R, vertice_size-1, MPI_INT, resultDistances, vertice_size-1, MPI_INT, MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}

	if (world_rank==0)
	{
		//Outputting the results
		printResults(argv[3], resultDistances, numVertices);
		
		//free up the memory
	    delete vertices;
	    delete resultDistances;
	}

	// Finalize: Any resources allocated for MPI can be freed
	MPI_Finalize();
	return 0;
}
