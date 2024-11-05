#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BOARD_SIZE 8

typedef struct {
    char color;
    char type;
    int row, col;
} Piece;

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    char move_sequence[100];
} Position;

typedef struct {
    Position position;
    char color;
    int depth;
} ThreadData;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int max_depth;
//vreau sa fie un vector care retine secvente de mutari care sunt maximale si care duc la checkmate
//o secventa de mutari este un string de forma "pna1a2_knb1c3_..._kna8a7" unde fiecare mutare
//este de forma "tip_piesa_culoare_pozitie_initiala_pozitie_finala", si vreau sa retin secventa intr-un vector de stringuri ce se termina cu '\0'
char final_move_sequence[100][100];
int final_move_sequence_index = 0;
int lungime_secventa = 0;



void init_board(Position *pos, Piece *pieces, int num_pieces) {
    memset(pos->board, '_', sizeof(pos->board));
    for (int i = 0; i < num_pieces; i++) {
        pos->board[pieces[i].row][pieces[i].col] = pieces[i].type;
    }
    strcpy(pos->move_sequence, "---");
}

int is_stalemate(Position *pos, char color) {
    // Implement a function to check if the given color is in stalemate.
    // Return 1 if the position is stalemate, otherwise return 0.
    if(color == 'a'){
        for(int i = 0;i<BOARD_SIZE;i++){
            for(int j = 0;j<BOARD_SIZE;j++){
                if(pos->board[i][j] == 'K'){
                    int poz1,poz2;
                    for(poz1=i-1,poz2=j-1;poz1>=0 && poz2>=0;){
                        if(pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'b'){
                            return 1;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            break;
                        }
                        poz1--;
                        poz2--;
                    }
                    for(poz1=i+1,poz2=j+1;poz1<=7 && poz2<=7;){
                        if(pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'b'){
                            return 1;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            break;
                        }
                        poz1++;
                        poz2++;
                    }
                    for(poz1=i,poz2=j+1;poz2<=7;){
                        if(pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r'){
                            return 1;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            break;
                        }
                        poz2++;
                    }
                    for(poz1=i,poz2=j-1;poz2>=0;){
                        if(pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r'){
                            return 1;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            break;
                        }
                        poz2--;
                    }
                    for(poz1=i-1,poz2=j;poz1>=0;){
                        if(pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r'){
                            return 1;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            break;
                        }
                        poz1--;
                    }
                    for(poz1=i+1,poz2=j;poz1<=7;){
                        if(pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r'){
                            return 1;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            break;
                        }
                        poz1++;
                    }
                    for(poz1=i-1,poz2=j+1;poz1>=0 && poz2<=7;){
                        if(pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'b'){
                            return 1;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            break;
                        }
                        poz1--;
                        poz2++;
                    }
                    for(poz1=i+1,poz2=j-1;poz1<=7 && poz2>=0;){
                        if(pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'b'){
                            return 1;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            break;
                        }
                        poz1++;
                        poz2--;
                    }
                    if(i >= 2 && j >= 1) {
                        if(pos->board[i-2][j-1] == 'n') {
                            return 1;
                        }
                    }
                    if(i >= 2 && j <= 6) {
                        if(pos->board[i-2][j+1] == 'n') {
                            return 1;
                        }
                    }
                    if(i >= 1 && j >= 2) {
                        if(pos->board[i-1][j-2] == 'n') {
                            return 1;
                        }
                    }
                    if(i >= 1 && j <= 5) {
                        if(pos->board[i-1][j+2] == 'n') {
                            return 1;
                        }
                    }
                    if(i <= 6 && j >= 2) {
                        if(pos->board[i+1][j-2] == 'n') {
                            return 1;
                        }
                    }
                    if(i <= 6 && j <= 5) {
                        if(pos->board[i+1][j+2] == 'n') {
                            return 1;
                        }
                    }
                    if(i <= 5 && j >= 1) {
                        if(pos->board[i+2][j-1] == 'n') {
                            return 1;
                        }
                    }
                    if(i <= 5 && j <= 6) {
                        if(pos->board[i+2][j+1] == 'n') {
                            return 1;
                        }
                    }
                    if(i <= 6 && j <= 6) {
                        if(pos->board[i+1][j+1] == 'p') {
                            return 1;
                        }
                    }
                    if(i <= 6 && j >= 1) {
                        if(pos->board[i+1][j-1] == 'p') {
                            return 1;
                        }
                    }
                }
            }
        }
    }else{
        if(color == 'n'){
            for(int i = 0;i<BOARD_SIZE;i++){
                for(int j = 0;j<BOARD_SIZE;j++){
                    if(pos->board[i][j] == 'k'){
                        int poz1,poz2;
                        for(poz1=i-1,poz2=j-1;poz1>=0 && poz2>=0;){
                            if(pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'B'){
                                return 1;
                            }
                            if(pos->board[poz1][poz2] != '_'){
                                break;
                            }
                            poz1--;
                            poz2--;
                        }
                        for(poz1=i+1,poz2=j+1;poz1<=7 && poz2<=7;){
                            if(pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'B'){
                                return 1;
                            }
                            if(pos->board[poz1][poz2] != '_'){
                                break;
                            }
                            poz1++;
                            poz2++;
                        }
                        for(poz1=i,poz2=j+1;poz2<=7;){
                            if(pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R'){
                                return 1;
                            }
                            if(pos->board[poz1][poz2] != '_'){
                                break;
                            }
                            poz2++;
                        }
                        for(poz1=i,poz2=j-1;poz2>=0;){
                            if(pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R'){
                                return 1;
                            }
                            if(pos->board[poz1][poz2] != '_'){
                                break;
                            }
                            poz2--;
                        }
                        for(poz1=i-1,poz2=j;poz1>=0;){
                            if(pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R'){
                                return 1;
                            }
                            if(pos->board[poz1][poz2] != '_'){
                                break;
                            }
                            poz1--;
                        }
                        for(poz1=i+1,poz2=j;poz1<=7;){
                            if(pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R'){
                                return 1;
                            }
                            if(pos->board[poz1][poz2] != '_'){
                                break;
                            }
                            poz1++;
                        }
                        for(poz1=i-1,poz2=j+1;poz1>=0 && poz2<=7;){
                            if(pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'B'){
                                return 1;
                            }
                            if(pos->board[poz1][poz2] != '_'){
                                break;
                            }
                            poz1--;
                            poz2++;
                        }
                        for(poz1=i+1,poz2=j-1;poz1<=7 && poz2>=0;){
                            if(pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'B'){
                                return 1;
                            }
                            if(pos->board[poz1][poz2] != '_'){
                                break;
                            }
                            poz1++;
                            poz2--;
                        }
                        if(i >= 2 && j >= 1) {
                            if(pos->board[i-2][j-1] == 'N') {
                                return 1;
                            }
                        }
                        if(i >= 2 && j <= 6) {
                            if(pos->board[i-2][j+1] == 'N') {
                                return 1;
                            }
                        }
                        if(i >= 1 && j >= 2) {
                            if(pos->board[i-1][j-2] == 'N') {
                                return 1;
                            }
                        }
                        if(i >= 1 && j <= 5) {
                            if(pos->board[i-1][j+2] == 'N') {
                                return 1;
                            }
                        }
                        if(i <= 6 && j >= 2) {
                            if(pos->board[i+1][j-2] == 'N') {
                                return 1;
                            }
                        }
                        if(i <= 6 && j <= 5) {
                            if(pos->board[i+1][j+2] == 'N') {
                                return 1;
                            }
                        }
                        if(i <= 5 && j >= 1) {
                            if(pos->board[i+2][j-1] == 'N') {
                                return 1;
                            }
                        }
                        if(i <= 5 && j <= 6) {
                            if(pos->board[i+2][j+1] == 'N') {
                                return 1;
                            }
                        }
                        if(i <= 6 && j <= 6) {
                            if(pos->board[i+1][j+1] == 'P') {
                                return 1;
                            }
                        }
                        if(i <= 6 && j >= 1) {
                            if(pos->board[i+1][j-1] == 'P') {
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
char* transforma_pozitie_string(char* pozitie, int i, int j,char culoare,char tip_piesa,int i_final,int j_final) {
    pozitie[0] = tip_piesa;
    pozitie[1] = culoare;
    pozitie[2] = i + 'a';
    pozitie[3] = j + '1';
    pozitie[4] = i_final + 'a';
    pozitie[5] = j_final + '1';
    pozitie[6] = '\0';
    return pozitie;
}

void generate_moves(Position *pos, char color, Position *new_positions, int *num_new_positions) {
    // Implement a function to generate all valid moves for the given color.
    // Populate the new_positions array and update num_new_positions.
    if(color == 'a') {
        //daca culoarea este alba, piesele sunt scrise cu litere mari
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (pos->board[i][j] == 'P') {
                    if(pos->board[i + 1][j] == '_'){
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        if(i == 6){
                            new_pos_copie.board[i + 1][j] = 'Q';
                        }else{
                            new_pos_copie.board[i + 1][j] = 'P';
                        }
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            if(i == 6){
                                new_positions[*num_new_positions].board[i + 1][j] = 'Q';
                            }else{
                                new_positions[*num_new_positions].board[i + 1][j] = 'P';
                            }
                            //functie care sa transforme pozitia in string
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'P', i - 1, j));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'P', i - 1, j));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i == 1 && pos->board[i + 1][j] == '_' && pos->board[i + 2][j] == '_'){
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i + 2][j] = 'P';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i + 2][j] = 'P';
                            //functie care sa transforme pozitia in string
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'P', i - 2, j));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'P', i - 2, j));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if((i + 1 <= 7) && (j + 1 <= 7) && (pos->board[i+1][j+1] == 'q' || pos->board[i+1][j+1] == 'r' || pos->board[i+1][j+1] == 'b' || pos->board[i+1][j+1] == 'n' || pos->board[i+1][j+1] == 'p')){
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        if(i + 1 == 7){
                            new_pos_copie.board[i + 1][j + 1] = 'Q';
                        }else{
                            new_pos_copie.board[i + 1][j + 1] = 'P';
                        }
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            if(i + 1 == 7){
                                new_positions[*num_new_positions].board[i + 1][j + 1] = 'Q';
                            }else{
                                new_positions[*num_new_positions].board[i + 1][j + 1] = 'P';
                            }
                            //functie care sa transforme pozitia in string
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'P', i + 1, j + 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'P', i + 1, j + 1));
                            }
                            (*num_new_positions)++;
                        }

                    }
                    if((i + 1 <= 7) && (j - 1 >= 0) && (pos->board[i+1][j-1] == 'q' || pos->board[i+1][j-1] == 'r' || pos->board[i+1][j-1] == 'b' || pos->board[i+1][j-1] == 'n' || pos->board[i+1][j-1] == 'p')){
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        if(i + 1 == 7){
                            new_pos_copie.board[i + 1][j - 1] = 'Q';
                        }else{
                            new_pos_copie.board[i + 1][j - 1] = 'P';
                        }
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            if(i + 1 == 7){
                                new_positions[*num_new_positions].board[i + 1][j - 1] = 'Q';
                            }else{
                                new_positions[*num_new_positions].board[i + 1][j - 1] = 'P';
                            }
                            //functie care sa transforme pozitia in string
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'P', i + 1, j - 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'P', i + 1, j - 1));
                            }
                            (*num_new_positions)++;
                        }
                    }
                }
                if (pos->board[i][j] == 'N') {
                    if(i >= 2 && j >= 1) {
                        if(pos->board[i-2][j-1] == 'K' || pos->board[i-2][j-1] == 'Q' || pos->board[i-2][j-1] == 'R' || pos->board[i-2][j-1] == 'B' || pos->board[i-2][j-1] == 'N' || pos->board[i-2][j-1] == 'P' || pos->board[i-2][j-1] == 'k') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i - 2][j - 1] = 'N';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i - 2][j - 1] = 'N';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i - 2, j - 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i - 2, j - 1));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i >= 2 && j <= 6) {
                        if(pos->board[i-2][j+1] == 'K' || pos->board[i-2][j+1] == 'Q' || pos->board[i-2][j+1] == 'R' || pos->board[i-2][j+1] == 'B' || pos->board[i-2][j+1] == 'N' || pos->board[i-2][j+1] == 'P' || pos->board[i-2][j+1] == 'k') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i - 2][j + 1] = 'N';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i - 2][j + 1] = 'N';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i - 2, j + 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i - 2, j + 1));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i >= 1 && j >= 2) {
                        if(pos->board[i-1][j-2] == 'K' || pos->board[i-1][j-2] == 'Q' || pos->board[i-1][j-2] == 'R' || pos->board[i-1][j-2] == 'B' || pos->board[i-1][j-2] == 'N' || pos->board[i-1][j-2] == 'P' || pos->board[i-1][j-2] == 'k') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i - 1][j - 2] = 'N';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i - 1][j - 2] = 'N';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i - 1, j - 2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i - 1, j - 2));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i >= 1 && j <= 5) {
                        if(pos->board[i-1][j+2] == 'K' || pos->board[i-1][j+2] == 'Q' || pos->board[i-1][j+2] == 'R' || pos->board[i-1][j+2] == 'B' || pos->board[i-1][j+2] == 'N' || pos->board[i-1][j+2] == 'P' || pos->board[i-1][j+2] == 'k') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i - 1][j + 2] = 'N';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i - 1][j + 2] = 'N';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i - 1, j + 2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i - 1, j + 2));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i <= 6 && j >= 2) {
                        if(pos->board[i+1][j-2] == 'K' || pos->board[i+1][j-2] == 'Q' || pos->board[i+1][j-2] == 'R' || pos->board[i+1][j-2] == 'B' || pos->board[i+1][j-2] == 'N' || pos->board[i+1][j-2] == 'P' || pos->board[i+1][j-2] == 'k') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i + 1][j - 2] = 'N';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i + 1][j - 2] = 'N';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i + 1, j - 2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i + 1, j - 2));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i <= 6 && j <= 5) {
                        if(pos->board[i+1][j+2] == 'K' || pos->board[i+1][j+2] == 'Q' || pos->board[i+1][j+2] == 'R' || pos->board[i+1][j+2] == 'B' || pos->board[i+1][j+2] == 'N' || pos->board[i+1][j+2] == 'P' || pos->board[i+1][j+2] == 'k') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i + 1][j + 2] = 'N';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i + 1][j + 2] = 'N';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i + 1, j + 2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i + 1, j + 2));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i <= 5 && j >= 1) {
                        if(pos->board[i+2][j-1] == 'K' || pos->board[i+2][j-1] == 'Q' || pos->board[i+2][j-1] == 'R' || pos->board[i+2][j-1] == 'B' || pos->board[i+2][j-1] == 'N' || pos->board[i+2][j-1] == 'P' || pos->board[i+2][j-1] == 'k') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i + 2][j - 1] = 'N';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i + 2][j - 1] = 'N';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i + 2, j - 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i + 2, j - 1));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i <= 5 && j <= 6) {
                        if(pos->board[i+2][j+1] == 'K' || pos->board[i+2][j+1] == 'Q' || pos->board[i+2][j+1] == 'R' || pos->board[i+2][j+1] == 'B' || pos->board[i+2][j+1] == 'N' || pos->board[i+2][j+1] == 'P' || pos->board[i+2][j+1] == 'k') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i + 2][j + 1] = 'N';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i + 2][j + 1] = 'N';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i + 2, j + 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'N', i + 2, j + 1));
                            }
                            (*num_new_positions)++;
                        }
                    }
                }
                if (pos->board[i][j] == 'B') {
                    // Implement the moves for the white bishop.
                    int poz1,poz2;
                    int sw = 0;
                    for(poz1=i + 1,poz2 = j + 1;poz1<=7 && poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'B';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'B';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'B', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'B', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                        poz2++;
                    }
                    sw = 0;
                    for(poz1=i - 1,poz2 = j - 1;poz1>=0 && poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'B';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'B';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'B', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'B', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                        poz2--;
                    }
                    sw = 0;
                    for(poz1=i + 1,poz2 = j - 1;poz1<=7 && poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'B';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'B';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'B', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'B', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                        poz2--;
                    }
                    sw = 0;
                    for(poz1=i - 1,poz2 = j + 1;poz1>=0 && poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'B';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'B';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'B', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'B', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                        poz2++;
                    }
                }
                if (pos->board[i][j] == 'R') {
                    // Implement the moves for the white rook.
                    int poz1,poz2;
                    int sw = 0;
                    for(poz1=i + 1,poz2 = j;poz1<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'R';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'R';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'R', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'R', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                    }
                    sw=0;
                    for(poz1=i - 1,poz2 = j;poz1>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'R';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'R';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'R', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'R', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                    }
                    sw=0;
                    for(poz1=i,poz2 = j + 1;poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'R';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'R';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'R', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'R', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz2++;
                    }
                    sw=0;
                    for(poz1=i,poz2=j-1;poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'R';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'R';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'R', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'R', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz2--;
                    }
                }
                if (pos->board[i][j] == 'Q') {
                    int poz1,poz2;
                    int sw = 0;
                    for(poz1=i + 1,poz2 = j;poz1<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'Q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'Q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                    }
                    sw=0;
                    for(poz1=i - 1,poz2 = j;poz1>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'Q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'Q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                    }
                    sw=0;
                    for(poz1=i,poz2 = j + 1;poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'Q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'Q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz2++;
                    }
                    sw=0;
                    for(poz1=i,poz2=j-1;poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'Q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'Q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz2--;
                    }
                    sw = 0;
                    for(poz1=i + 1,poz2 = j + 1;poz1<=7 && poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'Q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'Q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                        poz2++;
                    }
                    sw = 0;
                    for(poz1=i - 1,poz2 = j - 1;poz1>=0 && poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'Q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'Q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                        poz2--;
                    }
                    sw = 0;
                    for(poz1=i + 1,poz2 = j - 1;poz1<=7 && poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'Q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'Q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                        poz2--;
                    }
                    sw = 0;
                    for(poz1=i - 1,poz2 = j + 1;poz1>=0 && poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'K' || pos->board[poz1][poz2] == 'Q' || pos->board[poz1][poz2] == 'R' || pos->board[poz1][poz2] == 'B' || pos->board[poz1][poz2] == 'N' || pos->board[poz1][poz2] == 'P' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'Q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'Q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'Q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                        poz2++;
                    }
                }
                if (pos->board[i][j] == 'K') {
                    if(i + 1 <= 7 && pos->board[i + 1][j] != 'K' && pos->board[i + 1][j] != 'Q' && pos->board[i + 1][j] != 'R' && pos->board[i + 1][j] != 'B' && pos->board[i + 1][j] != 'N' && pos->board[i + 1][j] != 'P' && pos->board[i + 1][j] != 'k'){
                        //daca exista pozitia i+2,j si este ocupata de regele negru, atunci nu se poate muta acolo si vreau sa se continue cu urmatoprul if
                        if(!(((i <= 5) && (pos->board[i+2][j] == 'k')) || ( (i <= 5) && (j <= 6) && pos->board[i+2][j+1] == 'k') || ( (i <= 5) && (j >= 1) && pos->board[i+2][j-1] == 'k'))){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i + 1][j] = 'K';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i + 1][j] = 'K';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i + 1, j));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i + 1, j));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(i - 1 >= 0 && pos->board[i - 1][j] != 'K' && pos->board[i - 1][j] != 'Q' && pos->board[i - 1][j] != 'R' && pos->board[i - 1][j] != 'B' && pos->board[i - 1][j] != 'N' && pos->board[i - 1][j] != 'P' && pos->board[i - 1][j] != 'k'){
                        if(!(((i >= 2) && (pos->board[i-2][j] == 'k')) || ( (i >= 2) && (j <= 6) && pos->board[i-2][j+1] == 'k') || ( (i >= 2) && (j >= 1) && pos->board[i-2][j-1] == 'k'))){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i - 1][j] = 'K';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i - 1][j] = 'K';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i - 1, j));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i - 1, j));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(j + 1 <= 7 && pos->board[i][j + 1] != 'K' && pos->board[i][j + 1] != 'Q' && pos->board[i][j + 1] != 'R' && pos->board[i][j + 1] != 'B' && pos->board[i][j + 1] != 'N' && pos->board[i][j + 1] != 'P' && pos->board[i][j + 1] != 'k'){
                        if(!(((j <= 5) && (pos->board[i][j+2] == 'k')) || ( (j <= 5) && (i <= 6) && pos->board[i+1][j+2] == 'k') || ( (j <= 5) && (i >= 1) && pos->board[i-1][j+2] == 'k'))){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i][j + 1] = 'K';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i][j + 1] = 'K';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i, j + 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i, j + 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(j - 1 >= 0 && pos->board[i][j - 1] != 'K' && pos->board[i][j - 1] != 'Q' && pos->board[i][j - 1] != 'R' && pos->board[i][j - 1] != 'B' && pos->board[i][j - 1] != 'N' && pos->board[i][j - 1] != 'P' && pos->board[i][j - 1] != 'k'){
                        if(!(((j >= 2) && (pos->board[i][j-2] == 'k')) || ( (j >= 2) && (i <= 6) && pos->board[i+1][j-2] == 'k') || ( (j >= 2) && (i >= 1) && pos->board[i-1][j-2] == 'k'))){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i][j - 1] = 'K';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i][j - 1] = 'K';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i, j - 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i, j - 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(i + 1 <= 7 && j + 1 <= 7 && pos->board[i + 1][j + 1] != 'K' && pos->board[i + 1][j + 1] != 'Q' && pos->board[i + 1][j + 1] != 'R' && pos->board[i + 1][j + 1] != 'B' && pos->board[i + 1][j + 1] != 'N' && pos->board[i + 1][j + 1] != 'P' && pos->board[i + 1][j + 1] != 'k'){
                        if(!(((j <= 5) && (pos->board[i][j+2] == 'k')) || ( (j <= 5) && (i <= 6) && pos->board[i+1][j+2] == 'k') || ( (j <= 5) && (i <= 5) && pos->board[i+2][j+2] == 'k') || ((i <= 5) && (j <= 6) && pos->board[i+2][j+1] == 'k') || ((i <= 5) && pos->board[i+2][j] == 'k') )){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i + 1][j + 1] = 'K';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){    
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i + 1][j + 1] = 'K';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i + 1, j + 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i + 1, j + 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(i + 1 <= 7 && j - 1 >= 0 && pos->board[i + 1][j - 1] != 'K' && pos->board[i + 1][j - 1] != 'Q' && pos->board[i + 1][j - 1] != 'R' && pos->board[i + 1][j - 1] != 'B' && pos->board[i + 1][j - 1] != 'N' && pos->board[i + 1][j - 1] != 'P' && pos->board[i + 1][j - 1] != 'k'){
                        if(!(((j >= 2) && (pos->board[i][j-2] == 'k')) || ( (j >= 2) && (i <= 6) && pos->board[i+1][j-2] == 'k') || ( (j >= 2) && (i <= 5) && pos->board[i+2][j-2] == 'k') || ((i <= 5) && (j >= 1) && pos->board[i+2][j-1] == 'k') || ((i <= 5) && pos->board[i+2][j] == 'k') )){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i + 1][j - 1] = 'K';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i + 1][j - 1] = 'K';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i + 1, j - 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i + 1, j - 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(i - 1 >= 0 && j + 1 <= 7 && pos->board[i - 1][j + 1] != 'K' && pos->board[i - 1][j + 1] != 'Q' && pos->board[i - 1][j + 1] != 'R' && pos->board[i - 1][j + 1] != 'B' && pos->board[i - 1][j + 1] != 'N' && pos->board[i - 1][j + 1] != 'P' && pos->board[i - 1][j + 1] != 'k'){
                        if(!(((j <= 5) && (pos->board[i][j+2] == 'k')) || ( (j <= 5) && (i >= 1) && pos->board[i-1][j+2] == 'k') || ( (j <= 5) && (i >= 2) && pos->board[i-2][j+2] == 'k') || ((i >= 2) && (j <= 6) && pos->board[i-2][j+1] == 'k') || ((i >= 2) && pos->board[i-2][j] == 'k') )){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i - 1][j + 1] = 'K';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i - 1][j + 1] = 'K';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i - 1, j + 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i - 1, j + 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(i - 1 >= 0 && j - 1 >= 0 && pos->board[i - 1][j - 1] != 'K' && pos->board[i - 1][j - 1] != 'Q' && pos->board[i - 1][j - 1] != 'R' && pos->board[i - 1][j - 1] != 'B' && pos->board[i - 1][j - 1] != 'N' && pos->board[i - 1][j - 1] != 'P' && pos->board[i - 1][j - 1] != 'k'){
                        if(!(((j >= 2) && (pos->board[i][j-2] == 'k')) || ( (j >= 2) && (i >= 1) && pos->board[i-1][j-2] == 'k') || ( (j >= 2) && (i >= 2) && pos->board[i-2][j-2] == 'k') || ((i >= 2) && (j >= 1) && pos->board[i-2][j-1] == 'k') || ((i >= 2) && pos->board[i-2][j] == 'k') )){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i - 1][j - 1] = 'K';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){    
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i - 1][j - 1] = 'K';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i - 1, j - 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'a', 'K', i - 1, j - 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }

                }
            }
        }
    } else {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (pos->board[i][j] == 'p') {
                    if(pos->board[i + 1][j] == '_'){
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        if(i == 6){
                            new_pos_copie.board[i + 1][j] = 'q';
                        }else{
                            new_pos_copie.board[i + 1][j] = 'p';
                        }
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            if(i == 6){
                                new_positions[*num_new_positions].board[i + 1][j] = 'q';
                            }else{
                                new_positions[*num_new_positions].board[i + 1][j] = 'p';
                            }
                            //functie care sa transforme pozitia in string
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'p', i - 1, j));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'p', i - 1, j));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i == 1 && pos->board[i + 1][j] == '_' && pos->board[i + 2][j] == '_'){
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i + 2][j] = 'p';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i + 2][j] = 'p';
                            //functie care sa transforme pozitia in string
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'p', i - 2, j));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'p', i - 2, j));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if((i + 1 <= 7) && (j + 1 <= 7) && (pos->board[i+1][j+1] == 'Q' || pos->board[i+1][j+1] == 'R' || pos->board[i+1][j+1] == 'B' || pos->board[i+1][j+1] == 'N' || pos->board[i+1][j+1] == 'P')){
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        if(i + 1 == 7){
                            new_pos_copie.board[i + 1][j + 1] = 'q';
                        }else{
                            new_pos_copie.board[i + 1][j + 1] = 'p';
                        }
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            if(i + 1 == 7){
                                new_positions[*num_new_positions].board[i + 1][j + 1] = 'q';
                            }else{
                                new_positions[*num_new_positions].board[i + 1][j + 1] = 'p';
                            }
                            //functie care sa transforme pozitia in string
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'p', i + 1, j + 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'p', i + 1, j + 1));
                            }
                            (*num_new_positions)++;
                        }

                    }
                    if((i + 1 <= 7) && (j - 1 >= 0) && (pos->board[i+1][j-1] == 'Q' || pos->board[i+1][j-1] == 'R' || pos->board[i+1][j-1] == 'B' || pos->board[i+1][j-1] == 'N' || pos->board[i+1][j-1] == 'P')){
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        if(i + 1 == 7){
                            new_pos_copie.board[i + 1][j - 1] = 'q';
                        }else{
                            new_pos_copie.board[i + 1][j - 1] = 'p';
                        }
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            if(i + 1 == 7){
                                new_positions[*num_new_positions].board[i + 1][j - 1] = 'q';
                            }else{
                                new_positions[*num_new_positions].board[i + 1][j - 1] = 'p';
                            }
                            //functie care sa transforme pozitia in string
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'p', i + 1, j - 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'p', i + 1, j - 1));
                            }
                            (*num_new_positions)++;
                        }
                    }
                }
                if (pos->board[i][j] == 'n') {
                    if(i >= 2 && j >= 1) {
                        if(pos->board[i-2][j-1] == 'k' || pos->board[i-2][j-1] == 'q' || pos->board[i-2][j-1] == 'r' || pos->board[i-2][j-1] == 'b' || pos->board[i-2][j-1] == 'n' || pos->board[i-2][j-1] == 'p' || pos->board[i-2][j-1] == 'K') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i - 2][j - 1] = 'n';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i - 2][j - 1] = 'n';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i - 2, j - 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i - 2, j - 1));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i >= 2 && j <= 6) {
                        if(pos->board[i-2][j+1] == 'k' || pos->board[i-2][j+1] == 'q' || pos->board[i-2][j+1] == 'r' || pos->board[i-2][j+1] == 'b' || pos->board[i-2][j+1] == 'n' || pos->board[i-2][j+1] == 'p' || pos->board[i-2][j+1] == 'K') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i - 2][j + 1] = 'n';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i - 2][j + 1] = 'n';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i - 2, j + 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i - 2, j + 1));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i >= 1 && j >= 2) {
                        if(pos->board[i-1][j-2] == 'k' || pos->board[i-1][j-2] == 'q' || pos->board[i-1][j-2] == 'r' || pos->board[i-1][j-2] == 'b' || pos->board[i-1][j-2] == 'n' || pos->board[i-1][j-2] == 'p' || pos->board[i-1][j-2] == 'K') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i - 1][j - 2] = 'n';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i - 1][j - 2] = 'n';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i - 1, j - 2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i - 1, j - 2));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i >= 1 && j <= 5) {
                        if(pos->board[i-1][j+2] == 'k' || pos->board[i-1][j+2] == 'q' || pos->board[i-1][j+2] == 'r' || pos->board[i-1][j+2] == 'b' || pos->board[i-1][j+2] == 'n' || pos->board[i-1][j+2] == 'p' || pos->board[i-1][j+2] == 'K') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i - 1][j + 2] = 'n';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i - 1][j + 2] = 'n';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i - 1, j + 2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i - 1, j + 2));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i <= 6 && j >= 2) {
                        if(pos->board[i+1][j-2] == 'k' || pos->board[i+1][j-2] == 'q' || pos->board[i+1][j-2] == 'r' || pos->board[i+1][j-2] == 'b' || pos->board[i+1][j-2] == 'n' || pos->board[i+1][j-2] == 'p' || pos->board[i+1][j-2] == 'K') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i + 1][j - 2] = 'n';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i + 1][j - 2] = 'n';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i + 1, j - 2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i + 1, j - 2));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i <= 6 && j <= 5) {
                        if(pos->board[i+1][j+2] == 'k' || pos->board[i+1][j+2] == 'q' || pos->board[i+1][j+2] == 'r' || pos->board[i+1][j+2] == 'b' || pos->board[i+1][j+2] == 'n' || pos->board[i+1][j+2] == 'p' || pos->board[i+1][j+2] == 'K') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i + 1][j + 2] = 'n';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i + 1][j + 2] = 'n';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i + 1, j + 2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i + 1, j + 2));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i <= 5 && j >= 1) {
                        if(pos->board[i+2][j-1] == 'k' || pos->board[i+2][j-1] == 'q' || pos->board[i+2][j-1] == 'r' || pos->board[i+2][j-1] == 'b' || pos->board[i+2][j-1] == 'n' || pos->board[i+2][j-1] == 'p' || pos->board[i+2][j-1] == 'K') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i + 2][j - 1] = 'n';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i + 2][j - 1] = 'n';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i + 2, j - 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i + 2, j - 1));
                            }
                            (*num_new_positions)++;
                        }
                    }
                    if(i <= 5 && j <= 6) {
                        if(pos->board[i+2][j+1] == 'k' || pos->board[i+2][j+1] == 'q' || pos->board[i+2][j+1] == 'r' || pos->board[i+2][j+1] == 'b' || pos->board[i+2][j+1] == 'n' || pos->board[i+2][j+1] == 'p' || pos->board[i+2][j+1] == 'K') {
                            //daca in pozitia in care vreau sa mut calul se afla o piesa de aceeasi culoare, nu pot muta
                            continue;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[i + 2][j + 1] = 'n';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[i + 2][j + 1] = 'n';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i + 2, j + 1));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', i + 2, j + 1));
                            }
                            (*num_new_positions)++;
                        }
                    }
                }
                if (pos->board[i][j] == 'b') {
                    // Implement the moves for the white bishop.
                    int poz1,poz2;
                    int sw = 0;
                    for(poz1=i + 1,poz2 = j + 1;poz1<=7 && poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'k'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'b';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'b';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'n', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'b', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                        poz2++;
                    }
                    sw = 0;
                    for(poz1=i - 1,poz2 = j - 1;poz1>=0 && poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'b';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'b';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'b', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'b', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                        poz2--;
                    }
                    sw = 0;
                    for(poz1=i + 1,poz2 = j - 1;poz1<=7 && poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'b';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'b';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'b', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'b', 'b', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                        poz2--;
                    }
                    sw = 0;
                    for(poz1=i - 1,poz2 = j + 1;poz1>=0 && poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'b';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'b';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'b', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'b', 'b', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                        poz2++;
                    }
                }
                if (pos->board[i][j] == 'r') {
                    // Implement the moves for the white rook.
                    int poz1,poz2;
                    int sw = 0;
                    for(poz1=i + 1,poz2 = j;poz1<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'r';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'r';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'r', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'r', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                    }
                    sw=0;
                    for(poz1=i - 1,poz2 = j;poz1>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'r';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'r';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'r', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'r', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                    }
                    sw=0;
                    for(poz1=i,poz2 = j + 1;poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'r';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'r';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'r', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'r', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz2++;
                    }
                    sw=0;
                    for(poz1=i,poz2=j-1;poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'r';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'r';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'r', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'r', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz2--;
                    }
                }
                if (pos->board[i][j] == 'q') {
                    int poz1,poz2;
                    int sw = 0;
                    for(poz1=i + 1,poz2 = j;poz1<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                    }
                    sw=0;
                    for(poz1=i - 1,poz2 = j;poz1>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                    }
                    sw=0;
                    for(poz1=i,poz2 = j + 1;poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz2++;
                    }
                    sw=0;
                    for(poz1=i,poz2=j-1;poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz2--;
                    }
                    sw = 0;
                    for(poz1=i + 1,poz2 = j + 1;poz1<=7 && poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                        poz2++;
                    }
                    sw = 0;
                    for(poz1=i - 1,poz2 = j - 1;poz1>=0 && poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                        poz2--;
                    }
                    sw = 0;
                    for(poz1=i + 1,poz2 = j - 1;poz1<=7 && poz2>=0 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1++;
                        poz2--;
                    }
                    sw = 0;
                    for(poz1=i - 1,poz2 = j + 1;poz1>=0 && poz2<=7 && sw==0;){
                        if(pos->board[poz1][poz2] == 'k' || pos->board[poz1][poz2] == 'q' || pos->board[poz1][poz2] == 'r' || pos->board[poz1][poz2] == 'b' || pos->board[poz1][poz2] == 'n' || pos->board[poz1][poz2] == 'p' || pos->board[poz1][poz2] == 'K'){
                            break;
                        }
                        if(pos->board[poz1][poz2] != '_'){
                            sw = 1;
                        }
                        Position new_pos_copie;
                        strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                new_pos_copie.board[k][l] = pos->board[k][l];
                            }
                        }
                        new_pos_copie.board[i][j] = '_';
                        new_pos_copie.board[poz1][poz2] = 'q';
                        //verific daca mutarea pune regele in sah
                        if(is_stalemate(&new_pos_copie,color)==0){
                            //new_positions[*num_new_positions] = o copie a pozitiei curente
                            strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                            //copiaza tabla in new_positions
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                }
                            }
                            new_positions[*num_new_positions].board[i][j] = '_';
                            new_positions[*num_new_positions].board[poz1][poz2] = 'q';
                            char pozitie[100];
                            if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            else {
                                strcat(new_positions[*num_new_positions].move_sequence, "_");
                                strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'q', poz1, poz2));
                            }
                            (*num_new_positions)++;
                        }
                        poz1--;
                        poz2++;
                    }
                }
                if (pos->board[i][j] == 'k') {
                    if(i + 1 <= 7 && pos->board[i + 1][j] != 'k' && pos->board[i + 1][j] != 'q' && pos->board[i + 1][j] != 'r' && pos->board[i + 1][j] != 'b' && pos->board[i + 1][j] != 'n' && pos->board[i + 1][j] != 'p' && pos->board[i + 1][j] != 'K'){
                        //daca exista pozitia i+2,j si este ocupata de regele negru, atunci nu se poate muta acolo si vreau sa se continue cu urmatoprul if
                        if(!(((i <= 5) && (pos->board[i+2][j] == 'K')) || ( (i <= 5) && (j <= 6) && pos->board[i+2][j+1] == 'K') || ( (i <= 5) && (j >= 1) && pos->board[i+2][j-1] == 'K'))){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i + 1][j] = 'k';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i + 1][j] = 'k';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i + 1, j));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i + 1, j));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(i - 1 >= 0 && pos->board[i - 1][j] != 'k' && pos->board[i - 1][j] != 'q' && pos->board[i - 1][j] != 'r' && pos->board[i - 1][j] != 'b' && pos->board[i - 1][j] != 'n' && pos->board[i - 1][j] != 'p' && pos->board[i - 1][j] != 'K'){
                        if(!(((i >= 2) && (pos->board[i-2][j] == 'K')) || ( (i >= 2) && (j <= 6) && pos->board[i-2][j+1] == 'K') || ( (i >= 2) && (j >= 1) && pos->board[i-2][j-1] == 'K'))){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i - 1][j] = 'k';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i - 1][j] = 'k';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i - 1, j));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i - 1, j));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(j + 1 <= 7 && pos->board[i][j + 1] != 'k' && pos->board[i][j + 1] != 'q' && pos->board[i][j + 1] != 'r' && pos->board[i][j + 1] != 'b' && pos->board[i][j + 1] != 'n' && pos->board[i][j + 1] != 'p' && pos->board[i][j + 1] != 'K'){
                        if(!(((j <= 5) && (pos->board[i][j+2] == 'K')) || ( (j <= 5) && (i <= 6) && pos->board[i+1][j+2] == 'K') || ( (j <= 5) && (i >= 1) && pos->board[i-1][j+2] == 'K'))){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i][j + 1] = 'k';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i][j + 1] = 'k';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i, j + 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i, j + 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(j - 1 >= 0 && pos->board[i][j - 1] != 'k' && pos->board[i][j - 1] != 'q' && pos->board[i][j - 1] != 'r' && pos->board[i][j - 1] != 'b' && pos->board[i][j - 1] != 'n' && pos->board[i][j - 1] != 'p' && pos->board[i][j - 1] != 'K'){
                        if(!(((j >= 2) && (pos->board[i][j-2] == 'K')) || ( (j >= 2) && (i <= 6) && pos->board[i+1][j-2] == 'K') || ( (j >= 2) && (i >= 1) && pos->board[i-1][j-2] == 'K'))){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i][j - 1] = 'k';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i][j - 1] = 'k';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i, j - 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i, j - 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(i + 1 <= 7 && j + 1 <= 7 && pos->board[i + 1][j + 1] != 'k' && pos->board[i + 1][j + 1] != 'q' && pos->board[i + 1][j + 1] != 'r' && pos->board[i + 1][j + 1] != 'b' && pos->board[i + 1][j + 1] != 'n' && pos->board[i + 1][j + 1] != 'p' && pos->board[i + 1][j + 1] != 'K'){
                        if(!(((j <= 5) && (pos->board[i][j+2] == 'K')) || ( (j <= 5) && (i <= 6) && pos->board[i+1][j+2] == 'K') || ( (j <= 5) && (i <= 5) && pos->board[i+2][j+2] == 'K') || ((i <= 5) && (j <= 6) && pos->board[i+2][j+1] == 'K') || ((i <= 5) && pos->board[i+2][j] == 'K') )){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i + 1][j + 1] = 'k';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){    
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i + 1][j + 1] = 'k';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i + 1, j + 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i + 1, j + 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(i + 1 <= 7 && j - 1 >= 0 && pos->board[i + 1][j - 1] != 'k' && pos->board[i + 1][j - 1] != 'q' && pos->board[i + 1][j - 1] != 'r' && pos->board[i + 1][j - 1] != 'b' && pos->board[i + 1][j - 1] != 'n' && pos->board[i + 1][j - 1] != 'p' && pos->board[i + 1][j - 1] != 'K'){
                        if(!(((j >= 2) && (pos->board[i][j-2] == 'K')) || ( (j >= 2) && (i <= 6) && pos->board[i+1][j-2] == 'K') || ( (j >= 2) && (i <= 5) && pos->board[i+2][j-2] == 'K') || ((i <= 5) && (j >= 1) && pos->board[i+2][j-1] == 'K') || ((i <= 5) && pos->board[i+2][j] == 'K') )){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i + 1][j - 1] = 'k';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i + 1][j - 1] = 'k';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i + 1, j - 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i + 1, j - 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(i - 1 >= 0 && j + 1 <= 7 && pos->board[i - 1][j + 1] != 'k' && pos->board[i - 1][j + 1] != 'q' && pos->board[i - 1][j + 1] != 'r' && pos->board[i - 1][j + 1] != 'b' && pos->board[i - 1][j + 1] != 'n' && pos->board[i - 1][j + 1] != 'p' && pos->board[i - 1][j + 1] != 'K'){
                        if(!(((j <= 5) && (pos->board[i][j+2] == 'K')) || ( (j <= 5) && (i >= 1) && pos->board[i-1][j+2] == 'K') || ( (j <= 5) && (i >= 2) && pos->board[i-2][j+2] == 'K') || ((i >= 2) && (j <= 6) && pos->board[i-2][j+1] == 'K') || ((i >= 2) && pos->board[i-2][j] == 'K') )){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i - 1][j + 1] = 'k';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i - 1][j + 1] = 'k';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i - 1, j + 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i - 1, j + 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }
                    if(i - 1 >= 0 && j - 1 >= 0 && pos->board[i - 1][j - 1] != 'k' && pos->board[i - 1][j - 1] != 'q' && pos->board[i - 1][j - 1] != 'r' && pos->board[i - 1][j - 1] != 'b' && pos->board[i - 1][j - 1] != 'n' && pos->board[i - 1][j - 1] != 'p' && pos->board[i - 1][j - 1] != 'K'){
                        if(!(((j >= 2) && (pos->board[i][j-2] == 'K')) || ( (j >= 2) && (i >= 1) && pos->board[i-1][j-2] == 'K') || ( (j >= 2) && (i >= 2) && pos->board[i-2][j-2] == 'K') || ((i >= 2) && (j >= 1) && pos->board[i-2][j-1] == 'k') || ((i >= 2) && pos->board[i-2][j] == 'K') )){
                            Position new_pos_copie;
                            strcpy(new_pos_copie.move_sequence, pos->move_sequence);
                            for (int k = 0; k < BOARD_SIZE; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    new_pos_copie.board[k][l] = pos->board[k][l];
                                }
                            }
                            new_pos_copie.board[i - 1][j - 1] = 'k';
                            new_pos_copie.board[i][j] = '_';
                            //verific daca mutarea pune regele in sah
                            if(is_stalemate(&new_pos_copie,color)==0){    
                                //new_positions[*num_new_positions] = o copie a pozitiei curente
                                strcpy(new_positions[*num_new_positions].move_sequence, pos->move_sequence);
                                //copiaza tabla in new_positions
                                for (int k = 0; k < BOARD_SIZE; k++) {
                                    for (int l = 0; l < BOARD_SIZE; l++) {
                                        new_positions[*num_new_positions].board[k][l] = pos->board[k][l];
                                    }
                                }
                                new_positions[*num_new_positions].board[i - 1][j - 1] = 'k';
                                new_positions[*num_new_positions].board[i][j] = '_';
                                char pozitie[100];
                                if(strlen(new_positions[*num_new_positions].move_sequence) == 0) {
                                    strcpy(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i - 1, j - 1));
                                }
                                else {
                                    strcat(new_positions[*num_new_positions].move_sequence, "_");
                                    strcat(new_positions[*num_new_positions].move_sequence, transforma_pozitie_string(pozitie, i, j, 'n', 'k', i - 1, j - 1));
                                }
                                (*num_new_positions)++;
                            }
                        }
                    }

                }
            }
        }
    }
}
int is_checkmate(Position *pos, char color) {
    // Implement a function to check if the given color is in checkmate.
    // For simplicity, we assume that 'k' represents the king for both colors.
    // Return 1 if the position is checkmate, otherwise return 0.
    Position new_positions[100];
    int num_new_positions = 0;
    generate_moves(pos, color, new_positions, &num_new_positions);
    if(is_stalemate(pos,color)==1 && num_new_positions == 0){
        return 1;
    }
    return 0; // Placeholder
}

void *solve_position(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    if (is_checkmate(&data->position, data->color)) {
        pthread_mutex_lock(&mutex);
        if (data->depth <= max_depth) {
            //daca am gasit o secventa de mutari mai buna, o retin
            if (data->depth > lungime_secventa) {
                lungime_secventa = data->depth;
                final_move_sequence_index = 0;
                strcpy(final_move_sequence[final_move_sequence_index], data->position.move_sequence);
                final_move_sequence_index++;
            }
            //daca am gasit o secventa de mutari cu aceeasi lungime, o adaug la vectorul de secvente
            else if (data->depth == lungime_secventa) {
                strcpy(final_move_sequence[final_move_sequence_index], data->position.move_sequence);
                final_move_sequence_index++;
            }
        }
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    if (data->depth >= max_depth) return NULL;

    Position new_positions[100];
    int num_new_positions = 0;
    generate_moves(&data->position, data->color, new_positions, &num_new_positions);

    pthread_t threads[num_new_positions];
    ThreadData thread_data[num_new_positions];
    for (int i = 0; i < num_new_positions; i++) {
        thread_data[i].position = new_positions[i];
        thread_data[i].color = (data->color == 'a') ? 'n' : 'a';
        thread_data[i].depth = data->depth + 1;
        pthread_create(&threads[i], NULL, solve_position, &thread_data[i]);
    }
    for (int i = 0; i < num_new_positions; i++) {
        pthread_join(threads[i], NULL);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s num_moves color piece1 piece2 ...\n", argv[0]);
        return 1;
    }

    max_depth = atoi(argv[1]);
    char color = argv[2][0];

    int num_pieces = (argc - 3);

    Piece pieces[num_pieces];
    for (int i = 0; i < num_pieces; i++) {
        pieces[i].color = argv[3 + i][0];
        //daca culoarea este alba, piesele sunt scrise cu litere mari
        if (pieces[i].color == 'a') {
            pieces[i].type = argv[3 + i][1] - 32;
        }
        else{
            pieces[i].type = argv[3 + i][1];
        }
        pieces[i].row = argv[3 + i][2] - 'a';
        pieces[i].col = argv[3 + i][3] - '1';
    }
    Position initial_position;
    init_board(&initial_position, pieces, num_pieces);
    //printeaza tabla initiala in fisierul out.txt
    FILE *f = fopen("out.txt", "w");
    fprintf(f, "Tabla initiala:\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            fprintf(f, "%c", initial_position.board[i][j]);
        }
        fprintf(f, "\n");
    }
    ThreadData initial_data = {initial_position, color, 0};
    pthread_t initial_thread;
    pthread_create(&initial_thread, NULL, solve_position, &initial_data);
    pthread_join(initial_thread, NULL);

    //printeaza secventa de mutari in fisierul out.txt
    fprintf(f, "Secventa de mutari:\n");
    fprintf(f, "Lungime: %d\n", lungime_secventa);
    for (int i = 0; i < final_move_sequence_index; i++) {
        fprintf(f, "%s\n", final_move_sequence[i]);
    }
    fclose(f);
    return 0;
}