#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"
#include <math.h>

void aloca_matriz(int l, int c, int*** grid, int*** newgrid){
  
  int i, j;
  
  *grid = malloc(l * sizeof(int*));
  *newgrid = malloc(l * sizeof(int*));

  for(i=0; i<l; i++){
    (*grid)[i] = malloc(c * sizeof(int));
    (*newgrid)[i] = malloc(c * sizeof(int));
  }

  //zerando os valores
  for(i=0;i<l;i++){
    for(j=0;j<c;j++){
      (*grid)[i][j] = 0;
      (*newgrid)[i][j] = 0;
    }
  }
}

int getNeighbors(int** grid, int i, int j, int linhas, int colunas, int* previousRow, int* followingRow) {

  int vivos = 0, x, y;
  
  
  for(y=i-1; y<=i+1; y++){
    for(x=j-1; x<=j+1; x++){
      if(y == -1 && x == -1){
        if (previousRow[colunas-1] == 1) { vivos++;}
      } else if(y == -1 && x >= 0 && x <= colunas-1){
        if (previousRow[x] == 1) { vivos++;}
      } else if(y == -1 && x > colunas-1){
        if (previousRow[0] == 1) { vivos++;}
      } else if(x == -1 && y>=0 && y<=linhas-1){
        if (grid[y][colunas-1] == 1) { vivos++;}
      } else if(x == -1 && y > linhas-1){
        if (followingRow[colunas-1] == 1) { vivos++;}
      } 
      else if(y > linhas-1 && x >=0 && x <= colunas-1){
        if (followingRow[x] == 1) { vivos++;}
      } else if(y > linhas-1 && x > colunas-1){
        if (followingRow[0] == 1) { vivos++;}
      } else if(x > colunas-1 && y >=0 && y <= linhas-1){
        if (grid[y][0] == 1) { vivos++;}
      } else{
        if (grid[y][x] == 1) { vivos++;}
      }
    }
  }



  if(grid[i][j] == 1){ vivos--;}
  return vivos;
}

void nova_geracao(int*** grid, int*** newgrid, int linhas, int colunas){

  int vivos,i,j, processId, noProcesses, *previousRow, *followingRow;
  MPI_Status status;

  previousRow = (int*) malloc(colunas * sizeof(int));
  followingRow = (int*) malloc(colunas * sizeof(int));

  MPI_Comm_rank(MPI_COMM_WORLD, &processId);
  MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);
  
  //trocas de informação
  if(processId == 0){

    MPI_Recv(previousRow, colunas, MPI_INT, noProcesses-1, 13, MPI_COMM_WORLD, &status);
    MPI_Recv(followingRow, colunas, MPI_INT, 1, 14, MPI_COMM_WORLD, &status);

    MPI_Send((*grid)[linhas-1], colunas, MPI_INT, 1, 13, MPI_COMM_WORLD);
    MPI_Send((*grid)[0], colunas, MPI_INT, noProcesses-1, 14, MPI_COMM_WORLD);

  } else if (processId == noProcesses-1){

    MPI_Send((*grid)[linhas-1], colunas, MPI_INT, 0, 13, MPI_COMM_WORLD);
    MPI_Send((*grid)[0], colunas, MPI_INT, processId-1, 14, MPI_COMM_WORLD);
    MPI_Recv(previousRow, colunas, MPI_INT, processId-1, 13, MPI_COMM_WORLD, &status);
    MPI_Recv(followingRow, colunas, MPI_INT, 0, 14, MPI_COMM_WORLD, &status);

  } else{

    MPI_Send((*grid)[linhas-1], colunas, MPI_INT, processId+1, 13, MPI_COMM_WORLD);
    MPI_Send((*grid)[0], colunas, MPI_INT, processId-1, 14, MPI_COMM_WORLD);

    MPI_Recv(previousRow, colunas, MPI_INT, processId-1, 13, MPI_COMM_WORLD, &status);
    MPI_Recv(followingRow, colunas, MPI_INT, processId+1, 14, MPI_COMM_WORLD, &status);
  };

  for(i=0;i<linhas;i++){
    for(j=0;j<colunas;j++){
      vivos = getNeighbors((*grid), i, j, linhas, colunas, previousRow, followingRow);
      if((*grid)[i][j] == 1 && vivos == 2 || vivos == 3){
        (*newgrid)[i][j] = 1;
      } else if((*grid)[i][j] == 0 && vivos == 3){
        (*newgrid)[i][j] = 1;
      } else{
        (*newgrid)[i][j] = 0;
      }
    }
  }
  for(i=0;i<linhas;i++){
    for(int j=0;j<colunas;j++){
      (*grid)[i][j] = (*newgrid)[i][j];
    }
  }

  free(followingRow);
  free(previousRow);
}

int soma_celulas(int** grid, int linhas, int colunas){
  int soma = 0,i,j;
  for(i=0;i<linhas;i++){
    for(j=0;j<colunas;j++){
      soma += grid[i][j];
    }
  }
  return soma;
}

int main(int argc, char *argv[]) {

  struct timeval inicio, final;
  long long tmili;
  gettimeofday(&inicio, NULL);
  int linhas = 2048;
  int colunas = 2048;
  int geracoes = 200;

  //mpi
  int noProcesses, processId, localSize, first, noLines, *totalProcs;
  MPI_Status status;

  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &processId);

  int **grid, **newgrid;

  localSize = linhas/noProcesses;
  first = processId*localSize;

  if (processId==noProcesses-1) {
    noLines = linhas - first;
  }else 
    noLines = floor((float)linhas/noProcesses);

  printf("%d %d\n",  processId, noLines);
  
  aloca_matriz(noLines, colunas, &grid, &newgrid);
                                                                                                                                                                                                                                                                                                                                                                                   
  if(processId==0){
    //GLIDER
    int lin = 1, col = 1;
    grid[lin  ][col+1] = 1;
    grid[lin+1][col+2] = 1;
    grid[lin+2][col  ] = 1;
    grid[lin+2][col+1] = 1;
    grid[lin+2][col+2] = 1;

    //R-pentomino
    lin =10; col = 30;
    grid[lin  ][col+1] = 1;
    grid[lin  ][col+2] = 1;
    grid[lin+1][col  ] = 1;
    grid[lin+1][col+1] = 1;
    grid[lin+2][col+1] = 1;

  }

  MPI_Barrier(MPI_COMM_WORLD);
  
  for(int k=0; k<geracoes; k++){
    nova_geracao(&grid, &newgrid, noLines, colunas);
    MPI_Barrier(MPI_COMM_WORLD);
  }

  //aloca array para a soma de celulas vivas de cada processo
  totalProcs = (int *) malloc(noProcesses*sizeof(int));

  //calcula soma celulas vivas local
  totalProcs[processId] = soma_celulas(grid, noLines, colunas); 
  printf("total processo %d: %d\n", processId, totalProcs[processId]);

  if(processId == 0){
    //recebe a soma dos outros processos
    for(int id=1; id<noProcesses; id++){
      MPI_Recv(&totalProcs[id], 1, MPI_INT, id, 12, MPI_COMM_WORLD, &status);
    }
    //calcula a soma da grid toda
    int total = totalProcs[processId];
    for(int id=1; id<noProcesses; id++){
      total += totalProcs[id];
    }
    printf("total: %d\n", total);
  
  }else{
    MPI_Send(&totalProcs[processId], 1, MPI_INT, 0, 12, MPI_COMM_WORLD);
  }

  
  MPI_Finalize();  

  gettimeofday(&final, NULL);
  tmili = (int) (1000*(final.tv_sec - inicio.tv_sec) + (final.tv_usec - inicio.tv_usec) / 1000);
  printf("tempo decorrido: %lld  ms\n", tmili);
  
}