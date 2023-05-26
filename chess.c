#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#define BoardL 8
#define BoardW 8
#define SquareL 5
#define SquareW 11

const char* King_Movement = "y,x = +- 0&1";
const char* Rook_Movement = "y|X = +- BoardW|BoardL";

typedef struct{
    int X;
    int Y;
} Position;

/*
typedef struct{
    char equation[40]; // Bishop( y&x = +- o-BoardL); Knight( y,x = +- 1|2); King( y,x = +-0&1); Rook( y|X = +- BoardW|BoardL); ...
} Movement;
*/

typedef struct{
    char shape;
    Position* pp;
    char equation[40];
    char Team; // White|Black
} Piece;

typedef struct{
    int boardL;
    int boardW;
    Piece* board[ BoardL][BoardW];
} Board;

bool game_finished( Board* Board, Piece* king);
bool Move_piece( Board* Board, Piece* figure, Position* move_to);

Position* position_init( int new_x, int new_y){
    Position* new_point = malloc( sizeof( Position));
    new_point->X = new_x;
    new_point->Y = new_y;
    return new_point;
}

Piece* Piece_init( const char new_equation[], char new_shape, int x_coordinate, int y_coordinate, char new_team){
    Piece* new_figure = malloc( sizeof( Piece));
    new_figure->shape = new_shape;
    new_figure->pp = position_init( x_coordinate, y_coordinate);
    strcpy( new_figure->equation, new_equation);
    new_figure->Team = new_team;
    return new_figure;
}

Board* board_init( int new_boardL, int new_boardW){
    Board* new_board = malloc( sizeof( Board));
    new_board->boardL = new_boardL;
    new_board->boardW = new_boardW;
    for( int i = 0; i < new_board->boardL; i++){
        for( int j = 0; j < new_board->boardW; j++){
            new_board->board[i][j] = NULL;
        }
    }
    return new_board;
}

void Piece_free( Piece* fig){
    if( fig == NULL) return;
    free( fig->pp);
    free( fig);
}

void Board_free( Board* boar){
    for( int i = 0; i < boar->boardL; i++){
        for( int j = 0; j < boar->boardW; j++){
            Piece_free( boar->board[i][j]);
        }
    }
    free( boar);
}

bool there_piece( Board* chess, Position* point){
    return chess->board[ point->X][ point->Y] != NULL;
}

void drawSquare( Board* Board, int i_bl, int i_bw, int i_sl){
    if( i_bl == 0 && i_bw == 0){
        for( int i_sw = 0; i_sw < SquareW; i_sw++){
            printf(" ");
        }
    }else if( i_bl == 0){
        int this_square = SquareW;
        if( i_bw > 1){
            if( i_bw % 2 == 0) this_square++;
            else this_square--;
        }

        for( int i_sw = 0; i_sw < this_square; i_sw++){
            if( i_bw != 0 && i_sw == (this_square/2-1) && i_sl == SquareL/2) printf("%d", i_bw);
            else printf(" ");
        }
    }else if( i_bw == 0){
        for( int i_sw = 0; i_sw < SquareW; i_sw++){
            if( i_sl == SquareL/2 && i_sw == SquareW/2) printf("%d", i_bl);
            else printf(" ");
        }
    }else{
        int X = i_bl - 1, Y = i_bw - 1;
        Position* p = position_init( X, Y);
        bool sqareL_mid = i_sl > 0 && i_sl < SquareL - 1;
        for( int i_sw = 0; i_sw < SquareW; i_sw++){
            bool sqareW_mid = i_sw > 0 && i_sw < SquareW - 1;
            if( there_piece( Board, p) && sqareL_mid && sqareW_mid){
                if( i_sl == SquareL/2 && i_sw == SquareW/2 ){
                    printf("%c", Board->board[p->X][p->Y]->shape);
                }else printf(" ");
            }else{
                if( (i_bl + i_bw)% 2 == 1) printf( " ");
                if( (i_bl + i_bw)% 2 == 0) printf( "#");
            }
        }
        free( p);
    }
}

void drawBoard( Board* board){
    for( int i_bl = 0; i_bl <= BoardL; i_bl++){
        for( int i_sl = 0; i_sl < SquareL; i_sl++){
            for( int i_bw = 0; i_bw <= BoardW; i_bw++){
                drawSquare( board, i_bl, i_bw, i_sl);
            }
            printf( "\n");
        }
    }
}

bool Point_safe( Board* Board, Position* point, char safe_for){
    for( int i = 0, j = 0; i < Board->boardL || j < Board->boardW; i++, j++){
        if( i == 0 && j == 0) i++, j++; // just skip point cuz we can eat pieces we move to;
        if( Board->board[i][ point->Y] != NULL)
            if( strcmp( Board->board[i][ point->Y]->equation, Rook_Movement) == 0)
                if( Board->board[i][ point->Y]->Team != safe_for) return 0;
        if( Board->board[ point->X][j] != NULL)
            if( strcmp( Board->board[ point->X][j]->equation, Rook_Movement) == 0)
                if( Board->board[ point->X][j]->Team != safe_for) return 0;
    }
    Piece* iter;
    for( int i = - 1; i < 2; i++){
        for( int j = - 1; j < 2; j++){
            if( i == 0 && j == 0) j++;
            if( point->X+i < 0 || point->X+i >= Board->boardL) break;
            if(point->Y+j >= Board->boardW) break;
            if( point->Y+j >= 0){
                iter = Board->board[ point->X+i][ point->Y+j];
                if( iter != NULL)
                    if( strcmp( iter->equation, King_Movement) == 0)
                        if( iter->Team != safe_for) return 0;
            }
        }
    }
    return 1;
}

void main(void){
    srand( time(NULL));
    Board* Chess_board = board_init( BoardL, BoardW);

    char pieces[4][2] = {"WR","WR","WK","BK"};
    Piece* kings[2];
    Piece* iter;
    int i_x, i_y;
    for( int i = 0; i < 4; i++){
        do{
            do i_x = rand()%Chess_board->boardL, i_y = rand()%Chess_board->boardW;
            while( Chess_board->board[i_x][i_y] != NULL);

            if( pieces[i][1] == 'K') iter = Piece_init( King_Movement, 'K', i_x, i_y, pieces[i][0]);
            else                     iter = Piece_init( Rook_Movement, 'R', i_x, i_y, pieces[i][0]);
        }while( Point_safe( Chess_board, iter->pp, iter->Team) == 0);
        Chess_board->board[ iter->pp->X][ iter->pp->Y] = iter;
        if( pieces[i][1] == 'K') kings[i/2] = iter;
    }
    int i = 0;
    do{
        drawBoard( Chess_board);

        printf(" \n");
        Position* from = position_init( 0, 0);
        for(int i = 0; i < Chess_board->boardW/2; i++);
            for( int j = 0; j < SquareW; j++)
                printf(" ");
        printf(" X and Y of piece you wanna move ");
        scanf("%d %d", &from->X, &from->Y);
        printf("\n");

        Position* to;
        for(int i = 0; i < Chess_board->boardW/2; i++);
            for( int j = 0; j < SquareW; j++)
                printf(" ");
        printf(" X and Y of place you want to move piece ");
        scanf("%d %d", &to->X, &to->Y);
        printf("\n");

        printf("XXX");
/*
        if( Move_piece( Chess_board, Chess_board->board[ from->X][ from->Y], to) == 0){
            printf("YYY");
            for(int i = 0; i < Chess_board->boardW/2; i++);
                for( int j = 0; j < SquareW; j++)
                    printf(" ");
            printf("move not viable");
        }
*/
    system("cls");
    i++;
    }while( i < 5);
    //}while( game_finished( Chess_board, kings[0]) == 0 || game_finished( Chess_board, kings[1]) == 0);

    Board_free( Chess_board);
}

bool Move_piece( Board* Board, Piece* figure, Position* move_to){
    if( move_to->X == figure->pp->X && move_to->Y == figure->pp->Y) return 0;
    if( figure->pp->X < 0 || figure->pp->X > Board->boardL) return 0;
    if( figure->pp->Y < 0 || figure->pp->Y > Board->boardW) return 0;
    if( move_to->X < 0 || Board->boardL < move_to->X) return 0;
    if( move_to->Y < 0 || Board->boardW < move_to->Y) return 0;

    if( strcmp( figure->equation, King_Movement) == 0){
        if( move_to->X - figure->pp->X > 1 || move_to->X - figure->pp->X < -1) return 0;
        if( move_to->Y - figure->pp->Y > 1 || move_to->Y - figure->pp->Y < -1) return 0;
        if( Point_safe( Board, move_to, figure->Team) == 0) return 0;
        if( Board->board[ move_to->X][ move_to->Y] != NULL){
                if(Board->board[ move_to->X][ move_to->Y]->Team == figure->Team) return 0;
                else Piece_free( Board->board[ move_to->X][ move_to->Y]);
        }
        Board->board[ move_to->X][ move_to->Y] = figure;
        Board->board[ figure->pp->X][ figure->pp->Y] = NULL;
    }else if( strcmp( figure->equation, Rook_Movement) == 0){
        printf(" k?");
        if( move_to->X != figure->pp->X && move_to->Y != figure->pp->Y) return 0;
        int movement_x, movement_y;
        printf(" ok");
        if( move_to->X != figure->pp->X){
            movement_x = move_to->X - figure->pp->X;
            movement_y = 0;
        }else{
            movement_y = move_to->Y - figure->pp->Y;
            movement_x = 0;
        }
        printf(" ok1");
        for( int i_x = figure->pp->X, i_y = figure->pp->Y;    i_x != move_to->X || i_y != move_to->Y;    i_x += movement_x, i_y += movement_y)
            if( Board->board[i_x][i_y] != NULL) return 0;
        printf(" ok2");

        if( Board->board[ move_to->X][ move_to->Y] != NULL){
            if( Board->board[ move_to->X][ move_to->Y]->Team == figure->Team) return 0;
            else Piece_free( Board->board[ move_to->X][ move_to->Y]);
        }
        printf(" ok3");
        Board->board[ move_to->X][ move_to->Y] = figure;
        Board->board[ figure->pp->X][ figure->pp->Y] = NULL;

        printf(" ok4");
    }
    printf("\n");
    return 1;
}

bool is_checkmate( Board* Board, Piece* king){
    if( strcmp( king->equation, King_Movement) != 0) return 0;
    if( king->pp->X > Board->boardL || king->pp->X < 0) return 0;
    if( king->pp->Y > Board->boardW || king->pp->Y < 0) return 0;

    Piece* iter;
    for( int i = -1; i < 2; i++){
        for( int j = -1; j < 2; j++){
            iter = Board->board[ king->pp->X+i][ king->pp->Y+j];
            if( iter == NULL);
            else if( Point_safe( Board, iter->pp, king->Team)) return 0;// safe around
        }
    }
    // if here then all around king is dangerous => checkmate
    return 1;
}

bool is_stalemate( Board* Board, Piece* king){
    if( strcmp( king->equation, King_Movement) != 0) return 0;
    if( king->pp->X > Board->boardL || king->pp->X < 0) return 0;
    if( king->pp->Y > Board->boardW || king->pp->Y < 0) return 0;

    Piece* iter;
    for( int i = -1; i < 2; i++){
        for( int j = -1; j < 2; j++){
            iter = Board->board[ king->pp->X+i][ king->pp->Y+j];
            if( iter == NULL);
            else if( iter == king);
            else if( Point_safe( Board, iter->pp, king->Team)) return 0;// safe around
        }
    }
    // if here then all around king is dangerous
    if( Point_safe( Board, king->pp, king->Team) == 0) return 0; // not stale but checkmate
    return 1;
}

bool game_finished( Board* Board, Piece* king){
    if( is_checkmate( Board, king) == 0) return 0;
    if( is_stalemate( Board, king) == 0) return 0;
    return 1;
}
