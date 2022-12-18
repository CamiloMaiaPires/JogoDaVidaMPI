#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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

int getNeighbors(int** grid, int i, int j, int linhas, int colunas) {

  int vivos = 0,x,y;

  for(y=i-1; y<=i+1; y++){
    for(x=j-1; x<=j+1; x++){
      
      if(y == -1 && x == -1){
        if (grid[linhas-1][colunas-1] == 1) { vivos++;}
      } else if(y == -1 && x >= 0 && x <= colunas-1){
        if (grid[linhas-1][x] == 1) { vivos++;}
      } else if(y == -1 && x > colunas-1){
        if (grid[linhas-1][0] == 1) { vivos++;}
      } else if(x == -1 && y>=0 && y<=linhas-1){
        if (grid[y][colunas-1] == 1) { vivos++;}
      } else if(x == -1 && y > linhas-1){
        if (grid[0][colunas-1] == 1) { vivos++;}
      } else if(y > linhas-1 && x >=0 && x <= colunas-1){
        if (grid[0][x] == 1) { vivos++;}
      } else if(y > linhas-1 && x > colunas-1){
        if (grid[0][0] == 1) { vivos++;}
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
  int vivos,i,j;
  
  for(i=0;i<linhas;i++){
    for(j=0;j<colunas;j++){
      vivos = getNeighbors((*grid), i, j, linhas, colunas);
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
}

int soma_celulas(int*** grid, int linhas, int colunas){
  int soma = 0,i,j;
  for(i=0;i<linhas;i++){
    for(j=0;j<colunas;j++){
      soma += (*grid)[i][j];
    }
  }
  return soma;
}

int main(void) {

  struct timeval inicio, final;
  long long tmili;
  gettimeofday(&inicio, NULL);
  int linhas = 2048;
  int colunas = 2048;
  int geracoes = 2000;

  int **grid, **newgrid;
  aloca_matriz(linhas, colunas, &grid, &newgrid);

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

  for(int k=0; k<geracoes; k++){
    nova_geracao(&grid, &newgrid, linhas, colunas);
  }
  
  int total = soma_celulas(&grid, linhas, colunas);
  printf("%d\n", total);
  gettimeofday(&final, NULL);
  tmili = (int) (1000*(final.tv_sec - inicio.tv_sec) + (final.tv_usec - inicio.tv_usec) / 1000);
  printf("tempo decorrido: %lld  ms\n", tmili);
  
}