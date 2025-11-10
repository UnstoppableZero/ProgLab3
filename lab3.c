#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"

extern int** sudoku_board;
int* worker_validation;

int** read_board_from_file(char* filename){
    FILE *fp = NULL;
    int** board = NULL;

    // replace this comment with your code
    fp = fopen(filename, "r");
    if (fp == NULL){
        perror("Error opening file");
        return NULL;
    }
    board = (int**)(malloc(9 * sizeof(int*)));
    for (int i = 0; i < 9; i++){
        board[i] = (int*)malloc(9*sizeof(int));
        for(int j = 0; j < 9; j++){
            fscanf(fp, "%d%*c", &board[i][j]);
        }
    }
    fclose(fp);

    return board;
}

void* worker_function(void*args){
    param_struct* parameter = (param_struct*)args;
    int checklist[10];
    for(int i = 0; i < 10; i++){
        checklist[i] = 0;
    }
    int is_valid = 1;
    for(int r = parameter->starting_row; r <= parameter->ending_row; r++ ){
        for(int c = parameter->starting_col; c <= parameter->ending_col; c ++){
            int num = sudoku_board[r][c];
            if(num < 1 || num > 9){
                is_valid = 0;
                break;
            }else if (checklist[num] == 1)
            {
                is_valid = 0;
                break;
            }else{
                checklist[num] = 1;
            }
        }
        if(is_valid == 0){
            break;
        }
    }
    worker_validation[parameter->id] = is_valid;
    free(parameter);
    return NULL;
}


int is_board_valid(){
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameter;
    
    // replace this comment with your code
    tid = (pthread_t*)(malloc(27 * sizeof(pthread_t)));
    worker_validation = (int*)(malloc(27 * sizeof(int)));
    pthread_attr_init(&attr);


    for (int i = 0; i < 27; i++){
        parameter = (param_struct*)malloc(sizeof(param_struct));
        parameter->id = i;
        if (i < 9){
            parameter->starting_row = i;
            parameter->starting_col = 0;
            parameter->ending_row = i;
            parameter->ending_col = 8;

        }else if (i < 18)
        {
            int col_index = i - 9;
            parameter->starting_row = 0;
            parameter->starting_col = col_index;
            parameter->ending_row = 8;
            parameter->ending_col = col_index;
        }else if (i < 27)
        {
            int grid_index = i - 18;

            int top_row = (grid_index / 3) * 3;
            int left_col = (grid_index % 3) * 3;

            parameter->starting_row = top_row;
            parameter->starting_col = left_col;
            parameter->ending_row = top_row + 2;
            parameter->ending_col = left_col + 2;
        }
        pthread_create(&tid[i], &attr, worker_function, (void*)parameter);
    }
        for(int i = 0; i < 27; i ++){
            pthread_join(tid[i], NULL);
        }
        
        for(int i = 0; i < 27; i++){
            if(worker_validation[i] == 0){
                free(tid);
                free(worker_validation);
                return 0;
            }
        }
        free(tid);
        free(worker_validation);
        return 1;
    
}
