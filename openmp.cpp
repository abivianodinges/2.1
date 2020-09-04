#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <vector>
#include <time.h>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <unistd.h>
#include <omp.h>

using namespace std;
int cores = 1;
int SZ = 5;
int **A, **B, **C;

void init(int **&A);
void *add(void *block_id);
void print(int **A);
void *multiply(void *args);
void add_omp();
int step_i = 0;
int main()
{

    init(A);
    init(B);
    init(C);

    for (int i = 0; i < SZ; i++)
    {
        for (int j = 0; j < SZ; j++)
        {
            C[i][j] = 0;
        }
    }

    double time_elapsed = 0.0;
    string myText;

    ofstream myfile;
    myfile.open("myfile.txt");

    auto start = std::chrono::high_resolution_clock::now();

    add_omp();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> execution_time_md = end - start;
                                                                 
    cout << "Exection time openmp: " << execution_time_md.count() << "\n";
    for (int i = 0; i < SZ; ++i)
    {
        for (int j = 0; j < SZ; ++j)
            myfile << C[i][j] << '\t';
        myfile << '\n';
    }
    myfile.close();

    // ifstream myReadFile("myfile.txt");
    // while (getline(myReadFile, myText))
    // {
    //     cout << myText << '\n';
    // }
    // myReadFile.close();

    // print(A);
    // print(B);
    // print(C);
    for (long i = 0; i < SZ; ++i)
    {
        free(A[i]);
    }
    free(A);
    for (long i = 0; i < SZ; ++i)
    {
        free(B[i]);
    }
    free(B);
    for (long i = 0; i < SZ; ++i)
    {
        free(C[i]);
    }
    free(C);
}
void print(int **A)
{
    for (long i = 0; i < SZ; i++)
    {
        for (long j = 0; j < SZ; j++)
        {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    printf("---------------\n");
}

void init(int **&A)
{
    A = (int **)malloc(sizeof(int *) * SZ);
    for (int i = 0; i < SZ; i++)
    {
        A[i] = (int *)malloc(sizeof(int) * SZ);
    }
    for (long i = 0; i < SZ; i++)
    {
        for (long j = 0; j < SZ; j++)
        {
            A[i][j] = rand() % 10;
        }
    }
}

void *add(void *block_id)
{
    long block_sz = (SZ + 1) / cores;
    long start = (long)block_id * block_sz;
    long end = start + block_sz;
    if (end > SZ)
        end = SZ;

    for (long i = start; i < end; i++)
    {
        for (long j = 0; j < SZ; j++)
        {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
}

void *multiply(void *args)
{

    int core_num = step_i++;

    for (int i = core_num * SZ / cores; i < (core_num + 1) * SZ / cores; i++)
    {
        for (int j = 0; j < SZ; j++)
        {
            for (int k = 0; k < SZ; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void add_omp(){
    #pragma omp parallel num_threads(cores)
    {
        #pragma omp for
        for(long i=0; i < SZ;i++){
            #pragma omp ection master
            for(long j = 0; j < SZ; j++){
                for(long k=0; k<SZ; k++){
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }
}