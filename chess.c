#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<conio.h>
#define BoardL 8
#define BoardW 8
#define SquareL 5
#define SquareW 11

const char* King_Movement = "y,x = +- 0&1";
const char* Rook_Movement = "y|X = +- BoardW|BoardL";

const bool Rook_design[SquareL][SquareW] = {    {0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
                                                {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
                                                {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
                                                {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
                                                {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0}   };

const bool King_design[SquareL][SquareW] = {    {0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
                                                {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
                                                {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
                                                {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
                                                {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0}   };

const bool null_design[SquareW] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

typedef struct{
    int X;
    int Y;
} Position;

typedef struct{
    char shape;
    Position* pp;
    char equation[40];
    char Team; // White|Black
    char color; // @   |  .
} Piece;

typedef struct{
    int boardL;
    int boardW;
    Piece* board[ BoardL][BoardW];
} Board;

bool is_checkmate( Board* Board, Piece* king);
bool is_stalemate( Board* Board, Piece* king);
bool game_finished( Board* Board, Piece* king);
bool Move_piece( Board* Board, Piece* figure, Position* move_to);
bool Point_safe( Board* Board, Piece* safe_for, Position* point);

Position* position_init( int new_x, int new_y){
    Position* new_point = malloc( sizeof( Position));
    new_point->X = new_x;
    new_point->Y = new_y;
    return new_point;
}

Piece* Piece_init( const char new_equation[], char new_shape, int x_coordinate, int y_coordinate, char new_team, char new_color){
    Piece* new_figure = malloc( sizeof( Piece));
    new_figure->shape = new_shape;
    new_figure->pp = position_init( x_coordinate, y_coordinate);
    strcpy( new_figure->equation, new_equation);
    new_figure->Team = new_team;
    new_figure->color = new_color;
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
        const bool* slice;
        if( there_piece( Board, p)){
            if( strcmp( Board->board[ p->X][ p->Y]->equation, Rook_Movement) == 0) slice = Rook_design[ i_sl];
            if( strcmp( Board->board[ p->X][ p->Y]->equation, King_Movement) == 0) slice = King_design[ i_sl];
        }else{
            slice = null_design;
        }

        for( int i_sw = 0; i_sw < SquareW; i_sw++){
            if( there_piece( Board, p) && slice[ i_sw]){
                printf("%c", Board->board[ p->X][ p->Y]->color);
            }else{
                if( (i_bl + i_bw)% 2 == 1) printf( " ");
                if( (i_bl + i_bw)% 2 == 0) printf( "*");
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

void main(void){
    srand( time(NULL));
    Board* Chess_board = board_init( BoardL, BoardW);

    char pieces[4][2] = {"WR","WR","WK","BK"};
    Piece* kings[2];
    Piece* iter = NULL;
    int i_x, i_y;
    char color;
    int times = 0;

    for( int i = 0; i < 4; i++){
        do{
            times++;
            if( iter != NULL) Chess_board->board[ iter->pp->X][ iter->pp->Y] = NULL;

            do i_x = rand()%Chess_board->boardL, i_y = rand()%Chess_board->boardW;
            while( Chess_board->board[i_x][i_y] != NULL);

            if( pieces[i][0] == 'B') color = '.';
            else color = '@';

            if( pieces[i][1] == 'K') iter = Piece_init( King_Movement, 'K', i_x, i_y, pieces[i][0], color);
            else                     iter = Piece_init( Rook_Movement, 'R', i_x, i_y, pieces[i][0], color);
        }while( Point_safe( Chess_board, iter, iter->pp) == 0);
        Chess_board->board[ iter->pp->X][ iter->pp->Y] = iter;
        if( pieces[i][1] == 'K') kings[i/2] = iter;
        iter = NULL;
    }
    Piece_free( iter);
    int i = 0;
    Piece_free( iter);


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

        Position* to = position_init( 0, 0);
        for(int i = 0; i < Chess_board->boardW/2; i++);
            for( int j = 0; j < SquareW; j++)
                printf(" ");
        printf(" X and Y of place you want to move piece ");
        scanf("%d %d", &to->X, &to->Y);
        printf("\n");

        from->X--;
        from->Y--;
        to->X--;
        to->Y--;
        if( Move_piece( Chess_board, Chess_board->board[ from->X][ from->Y], to) == 0){
            for(int i = 0; i < Chess_board->boardW/2; i++);
                for( int j = 0; j < SquareW; j++)
                    printf(" ");
            printf("move not viable");
        }
        i++;
        //getchar();
        system("cls");
        if( is_checkmate( Chess_board, kings[1])){
            printf("\n Black is checkmate");
        }
        if( is_stalemate( Chess_board, kings[1])){
            printf("\n Black is stalemate");
        }
        if( game_finished( Chess_board, kings[1])){
            printf("\n game finished. Black lost");
        }
        getchar();


    }while( i < 10);
    //}while( game_finished( Chess_board, kings[0]) == 0 && game_finished( Chess_board, kings[1]) == 0);
    drawBoard( Chess_board);
    Board_free( Chess_board);
    Piece_free( kings[ 0]);
    Piece_free( kings[ 1]);
}

bool Move_piece( Board* Board, Piece* figure, Position* move_to){
    if( Board == NULL) return 0;
    if( figure == NULL) return 0;
    if( move_to == NULL) return 0;
    if( move_to->X == figure->pp->X && move_to->Y == figure->pp->Y) return 0;
    if( figure->pp->X < 0 || figure->pp->X > Board->boardL) return 0;
    if( figure->pp->Y < 0 || figure->pp->Y > Board->boardW) return 0;
    if( move_to->X < 0 || Board->boardL < move_to->X) return 0;
    if( move_to->Y < 0 || Board->boardW < move_to->Y) return 0;
    //printf("\n hello");

    if( Board->board[ move_to->X][ move_to->Y] != NULL)
            if(Board->board[ move_to->X][ move_to->Y]->Team == figure->Team) return 0;
    //printf("\n hoe");
    if( strcmp( figure->equation, King_Movement) == 0){
        if( move_to->X - figure->pp->X > 1 || move_to->X - figure->pp->X < -1) return 0;
        if( move_to->Y - figure->pp->Y > 1 || move_to->Y - figure->pp->Y < -1) return 0;
        if( Point_safe( Board, figure, move_to) == 0) return 0;
        printf("\n we");
        if( Board->board[ move_to->X][ move_to->Y] != NULL)
            Piece_free( Board->board[ move_to->X][ move_to->Y]);
        Board->board[ move_to->X][ move_to->Y] = figure;
        Board->board[ figure->pp->X][ figure->pp->Y] = NULL;
        figure->pp->X = move_to->X;
        figure->pp->Y = move_to->Y;
        printf("\n good?");
    }else if( strcmp( figure->equation, Rook_Movement) == 0){
        if( move_to->X != figure->pp->X && move_to->Y != figure->pp->Y) return 0;
        //printf("\n whats");
        int m_x = move_to->X - figure->pp->X; // m_x = movement_x
        int m_y = move_to->Y - figure->pp->Y; // m_y = movement_y
        if( m_x > 0) m_x = 1;
        else if( m_x < 0) m_x = -1;
        if( m_y > 0) m_y = 1;
        else if( m_y < 0) m_y = -1;
        //printf("\n up");
        for( int i_x = figure->pp->X + m_x, i_y = figure->pp->Y + m_y;    i_x != move_to->X || i_y != move_to->Y;    i_x += m_x, i_y += m_y)
            if( Board->board[i_x][i_y] != NULL) return 0;
        //printf("\n ?");
        if( Board->board[ move_to->X][ move_to->Y] != NULL){
            if( Board->board[ move_to->X][ move_to->Y]->Team == figure->Team) return 0;
            else Piece_free( Board->board[ move_to->X][ move_to->Y]);
        }
        //printf("\n cuz");
        Board->board[ move_to->X][ move_to->Y] = figure;
        Board->board[ figure->pp->X][ figure->pp->Y] = NULL;
        figure->pp->X = move_to->X;
        figure->pp->Y = move_to->Y;
        //printf("\n im good uhuhuhuhuhuhuhuhuhuh");
    }
    return 1;
}

bool Point_safe( Board* Field, Piece* safe_for, Position* point){
    if( safe_for == NULL) return 0;
    if(  Field   == NULL) return 0;
    if(  point   == NULL) return 0;
    if( point->X >= Field->boardL) return 0;
    if( point->X < 0) return 0;
    if( point->Y >= Field->boardW) return 0;
    if( point->Y < 0) return 0;

    Field->board[ safe_for->pp->X][ safe_for->pp->Y] = NULL; // we lift the piece
    bool dangerous = 0;

    for( int i = 1; point->X + i < Field->boardL && dangerous == 0; i++)
        if( Field->board[ point->X+i][ point->Y] != NULL)
            if( strcmp( Field->board[ point->X+i][ point->Y]->equation, King_Movement) == 0) break;
            else if( Field->board[ point->X+i][ point->Y]->Team != safe_for->Team) dangerous = 1;

    for( int i = 1; point->X - i > -1 && dangerous == 0; i++)
        if( Field->board[ point->X-i][ point->Y] != NULL)
            if( strcmp( Field->board[ point->X-i][ point->Y]->equation, King_Movement) == 0) break;
            else if( Field->board[ point->X-i][ point->Y]->Team != safe_for->Team) dangerous = 1;

    for( int i = 1; point->Y + i < Field->boardW && dangerous == 0; i++)
        if( Field->board[ point->X][ point->Y+i] != NULL)
            if( strcmp( Field->board[ point->X][ point->Y+i]->equation, King_Movement) == 0) break;
            else if( Field->board[ point->X][ point->Y+i]->Team != safe_for->Team) dangerous = 1;

    for( int i = 1; point->Y - i > -1 && dangerous == 0; i++)
        if( Field->board[ point->X][ point->Y-i] != NULL)
            if( strcmp( Field->board[ point->X][ point->Y-i]->equation, King_Movement) == 0) break;
            else if( Field->board[ point->X][ point->Y-i]->Team != safe_for->Team) dangerous = 1;

    Field->board[ safe_for->pp->X][ safe_for->pp->Y] = safe_for; // we place it back down
    if( dangerous == 1) return 0;

    Piece* iter;
    for( int i = - 1; i < 2; i++){
        for( int j = - 1; j < 2; j++){
            if( i == 0 && j == 0) j++;
            if( point->X+i < 0 || point->X+i >= Field->boardL) break;
            if( point->Y+j >= Field->boardW) break;
            if( point->Y+j >= 0){
                iter = Field->board[ point->X+i][ point->Y+j];
                if( iter != NULL)
                    if( strcmp( iter->equation, King_Movement) == 0)
                        if( iter->Team != safe_for->Team) return 0;
            }
        }
    }

    return 1;
}

bool is_checkmate( Board* Board, Piece* king){
    if( strcmp( king->equation, King_Movement) != 0) return 0;
    if( king->pp->X >= Board->boardL || king->pp->X < 0) return 0;
    if( king->pp->Y >= Board->boardW || king->pp->Y < 0) return 0;

    Position* iter = position_init( 0, 0);
    for( int i = -1; i < 2; i++){
        for( int j = -1; j < 2; j++){
            iter->X = king->pp->X+i;
            iter->Y = king->pp->Y+j;
            if( Point_safe( Board, king, iter)) return 0;// safe around
        }
    }
    free( iter);
    // if here then all around king is dangerous => checkmate
    return 1;
}

bool is_stalemate( Board* Board, Piece* king){
    if( strcmp( king->equation, King_Movement) != 0) return 0;
    if( king->pp->X > Board->boardL || king->pp->X < 0) return 0;
    if( king->pp->Y > Board->boardW || king->pp->Y < 0) return 0;

    Position* iter = position_init( 0, 0);
    for( int i = -1; i < 2; i++){
        for( int j = -1; j < 2; j++){
            if( i == 0 && j == 0) j++;
            iter->X = king->pp->X+i;
            iter->Y = king->pp->Y+j;
            if( Board->board[ iter->X][ iter->Y] == king);
            else if( Point_safe( Board, king, iter)) return 0;// safe around
        }
    }
    free( iter);
    // if here then all around king is dangerous
    if( Point_safe( Board, king, king->pp) == 0) return 0; // not stale but checkmate
    return 1;
}

bool game_finished( Board* Board, Piece* king){
    if( is_checkmate( Board, king) == 1) return 1;
    if( is_stalemate( Board, king) == 1) return 1;
    return 0;
}

/*
    for( int i = 0; i < Chess_board->boardL; i++){
        for( int j = 0; j < Chess_board->boardW; j++){
            iter = Chess_board->board[i][j];
            if( iter != NULL)
                printf("\n Piece: X-%d Y-%d shape-%c color-%c Team-%c movement-%s", iter->pp->X, iter->pp->Y, iter->shape, iter->color, iter->Team, iter->equation);
        }
    }
*/


/*
        Piece* BK = Piece_init( King_Movement, 'K', 2, 1, 'B', '.');
        Piece* WK = Piece_init( King_Movement, 'K', 2, 3, 'W', '@');
        Piece* WR1 = Piece_init( Rook_Movement, 'R', 7, 0, 'W', '@');
        Piece* WR2 = Piece_init( Rook_Movement, 'R', 7, 1, 'W', '@');
        Chess_board->board[2][1] = BK;
        Chess_board->board[2][3] = WK;
        Chess_board->board[7][0] = WR1;
        Chess_board->board[7][1] = WR2;
drawBoard( Chess_board);

        printf("\n Game is %s finished", game_finished( Chess_board, BK) ? "" : "not");
        printf("\n Game is %s checkmate", is_checkmate( Chess_board, BK) ? "" : "not");
        printf("\n Game is %s stalemate", is_stalemate( Chess_board, BK) ? "" : "not");

        Position* iter = position_init( 0, 0);
        for( int x = -1; x < 2; x++){
            for( int y = -1; y < 2; y++){
                iter->X = BK->pp->X + x;
                iter->Y = BK->pp->Y + y;
                if( iter->X >= 0 && iter->X < Chess_board->boardL && iter->Y >= 0 && iter->Y < Chess_board->boardW )
                    printf("\n black %s safe at %d %d", Point_safe( Chess_board, BK, iter) ? "is" : "is not", iter->X, iter->Y);
            }
        }
        free( iter);
*/
