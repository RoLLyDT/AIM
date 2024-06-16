//Maksim KOZLOV 20219332

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#include <string.h>
#include <time.h>
#include <limits.h>

// Global variables 
#define NUM_OF_RUNS 1
int MAX_TIME = 100;
int taskCount;

struct task
{
	int N;
	int* p;
	int M;
};

struct result
{
	int n;
	int** res;
};

// Random integer for shaking method
int randNum(int min, int max)
{
	int div = max - min + 1;
	int val = rand() % div + min;
	return val;
}

// Creates "matrix"
// returns a pointer to a two-dimensional array of integers. 
int** create_s0(struct task task)
{
	int n = task.N;
	int** matr = (int**)malloc(sizeof(int*) * n), i, j;
	for (i = 0; i < n; i++)
	{
		matr[i] = malloc(sizeof(int) * n);
	}
	for (i = 0; i < task.N; i++)
	{
		for (j = 0; j < task.N; j++)
		{
			matr[i][j] = task.p[i * n + j];
		}
	}
	return matr;
}

// Copies data so called square
int** copy(int** matr, struct task task)
{
	int** copy_matr = malloc(sizeof(int*) * task.N), i, j;
	for (i = 0; i < task.N; i++)
	{
		copy_matr[i] = (int*) malloc(sizeof(int) * task.N);
	}
	for (i = 0; i < task.N; i++)
	{
		for (j = 0; j < task.N; j++)
		{
			copy_matr[i][j] = (int)matr[i][j];
		}
	}
	return copy_matr;
}

// Free allocated memory
void free_matr(int** matr, struct task task)
{
	for (int i = 0; i < task.N; i++) 
	{
		free(matr[i]);
	}
	free(matr);
}

// Square cost calculation
int calculate(int** s, struct task task) 
{
	int diag_sum1 = 0, diag_sum2 = 0, obj = 0, i, j;
	for (i = 0; i < task.N; i++)
	{
		int row_sum = 0;
		int col_sum = 0;
		diag_sum1 += s[i][i];
		diag_sum2 += s[i][task.N - i - 1];
		for (j = 0; j < task.N; j++)
		{
			row_sum += s[i][j];
			col_sum += s[j][i];
		}
		obj += abs(row_sum - task.M) + abs(col_sum - task.M);
	}
	obj += abs(diag_sum1 - task.M) + abs(diag_sum2 - task.M);
	return obj;
}

// Elements swap
void swap_elem(int** s, int row1, int row2, int col1, int col2) 
{
	int temp = s[row1][col1];
	s[row1][col1] = s[row2][col2];
	s[row2][col2] = temp;
}

// Random swapping of elements
void shaking(int** s, struct task task, int k)
{
	for (int i = 0; i <= k; i++) 
	{
		int row1 = randNum(0, task.N - 1);
		int col1 = randNum(0, task.N - 1);
		int row2, col2;
		do {
			row2 = randNum(0, task.N - 1);
			col2 = randNum(0, task.N - 1);
		} while (abs(row1 - row2) + abs(col1 - col2) < k);
		
		swap_elem(s, row1, row2, col1, col2);
	}
}

// Change each cell with EACH different, keeping the correct changes
void swapping(int** s, struct task task, int* obj, bool* improve) 
{
	for (int row1 = 0; row1 < task.N; row1++)
	{
		for (int col1 = 0; col1 < task.N; col1++)
		{
			for (int row2 = row1; row2 < task.N; row2++)
			{
				for (int col2 = ((row1 == row2) ? col1 + 1 : 0); col2 < task.N; col2++) {
					swap_elem(s, row1, row2, col1, col2);
					int new_obj = calculate(s, task);
					if (new_obj < (*obj)) 
					{
						(*obj) = new_obj;
						(*improve) = true;
					}
					else 
					{
						swap_elem(s, row1, row2, col1, col2);
					}
				}
			}
		}
	}
}

// Finding for at least ONE improvement
void local_search(int **s, struct task task) 
{
	int obj = calculate(s, task);
	int new_obj;
	for (int row1 = 0; row1 < task.N; row1++) {
		for (int col1 = 0; col1 < task.N; col1++) {
			for (int row2 = 0; row2 < task.N; row2++) {
				for (int col2 = 0; col2 < task.N; col2++) {
					if (row1 != row2 || col1 != col2) 
					{
						swap_elem(s, row1, row2, col1, col2);
						new_obj = calculate(s, task);
						if (new_obj < obj) 
						{
							obj = new_obj;
							return;
						}
						swap_elem(s, row1, row2, col1, col2);
					}
				}
			}
		}
	}
}

// Best descending function from provided files on moodle
void best_descending(int** s, int*** best, int* k, struct task task, time_t start, int* best_obj)
{
	bool improve;
	do
	{
		improve = false;
		int new_obj = calculate(s, task);
		swapping(s, task, &new_obj, &improve); // Square change
		time_t end = time(NULL);
		if ((end - start) >= MAX_TIME)
		{
			break;
		}
		if ((*best_obj) > new_obj) // If it was improved then save changes
		{
			free_matr(*best, task);
			(*best) = copy(s, task);
			(*best_obj) = new_obj;
			if (best_obj == 0)
			{
				break;
			}
			(*k) = 1;
		}
		else
		{
			(*k)++;
		}
	} while (improve);
}

// Main variable neighborhood search function
void vns(int** s, struct task task, struct result* result, time_t start)
{
	int** best = copy(s, task), best_obj = calculate(s, task);
	int kmax = (int)(log(task.N) / log(2)); // Amount
	int k, iter = 0;
	while(true) // Until the time is over or price of solution (obj = 0)
	{
		srand(iter);
		k = 1;
		while (k <= kmax)
		{
			shaking(s, task, k); // Call for random square changes
			local_search(s, task); // Producing all the works in the hope of one improving
			best_descending(s, &best, &k, task, start, &best_obj); // Search for the best square

			time_t end = time(NULL);
			if ((end - start) >= MAX_TIME)
			{
				break;
			}
		}
		int obj = calculate(s, task);
		if (obj < best_obj) 
		{
			free_matr(best, task);
			best = copy(s, task);
			best_obj = obj;
		}
		if (best_obj == 0) 
		{
			break;
		}
		time_t end = time(NULL);
		if ((end - start) >= MAX_TIME)
		{
			break;
		}
		iter++;
	}
	int final_obj = calculate(s, task);
	if (result->n > final_obj || result->n > best_obj)
	{
		result->res = final_obj < best_obj ? s : best;
		result->n = final_obj < best_obj ? final_obj : best_obj;
	}
}

// File reader function takes all jobs from the file and writes them to an array
struct task* readFile(char* filename) 
{
	FILE* file;
	if ((file = fopen(filename, "r")) == NULL)
	{
		printf("File not found");
		return 0;
	}
	fscanf(file, "%d", &taskCount);
	struct task* tasks = malloc(sizeof(struct task) * taskCount);
	for (int i = 0; i < taskCount; i++)
	{
		fscanf(file, "%d %d", &tasks[i].N, &tasks[i].M);
		tasks[i].p = (int**) malloc(sizeof(int) * pow(tasks[i].N, 2));
		int num = 0;
		for(int j = 0; j < pow(tasks[i].N, 2); j++)
		{
			fscanf(file, "%d", &num);
			tasks[i].p[j] = (int) num;
		}
	}
	
	fclose(file);
	return tasks;
}

// Function to write all decisions to a file
void toFile(char* filename, struct result* results, struct task* tasks)
{
	FILE* file;
	if ((file = fopen(filename, "w")) == NULL)
	{
		printf("Can't create the file");
		return 0;
	}
	fprintf(file, "%d\n", taskCount);
	for (int i = 0; i < taskCount; i++)
	{
		fprintf(file, "%d\n", results[i].n);
		for (int k = 0; k < tasks[i].N; k++)
		{
			for (int j = 0; j < tasks[i].N; j++)
			{
				fprintf(file, "%d ", results[i].res[k][j]);
			}
			fprintf(file, "\n");
		}
	}
	fclose(file);
}

int main(int argc, char *argv[])
{
	char *input_filename = "ms-test.txt"; // Default input file name
    char *output_filename = "solution_file.txt"; // Default output file name

	// In case of running program with specific cases being able to change
	for (int i = 1; i < argc; i++) 
	{
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) 
		{
            input_filename = argv[++i];
        } 
		else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) 
		{
            output_filename = argv[++i];
        } 
		else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) 
		{
            MAX_TIME = atoi(argv[++i]);
        }
    }

	
	struct task* tasks = readFile(input_filename); // Takes all the tasks from the file named "ms-test"
	struct result* results = malloc(sizeof(struct result) * taskCount);  // Create a set of solutions
	for (int i = 0; i < taskCount; i++)
	{
		results[i].n = INT_MAX;
		for (int run = 0; run < NUM_OF_RUNS; run++)
		{
			time_t start = time(NULL);
			srand(time(NULL));
			int** matr = create_s0(tasks[i]);
			vns(matr, tasks[i], &results[i], start); // Looking for a solution
			printf("%d\n", (int)results[i].n);
			for (int k = 0; k < tasks[i].N; k++)
			{
				for (int j = 0; j < tasks[i].N; j++)
				{
					printf("%d ", (int)results[i].res[k][j]);
				}
				printf("\n");
			}
			printf("\n");

		}
	}
	toFile(output_filename, results, tasks); // Save the result into a file named "solution_file.txt"

	return 0;
}