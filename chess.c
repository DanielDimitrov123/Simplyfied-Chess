#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<math.h>
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
                                                {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
                                                {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0}   };

const bool King_design[SquareL][SquareW] = {    {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
                                                {0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0},
                                                {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
                                                {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
                                                {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0}   };

const bool null_design[SquareW] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const int infinity =  2147483647;

typedef struct{
    int X;
    int Y;
} Position;

typedef struct{
    char shape;
    Position* pp;
    char equation[40];
    char Team; // White|Black
    char color; // @   |  :
} Piece;

typedef struct ll_pieces_temp{
    struct ll_pieces_temp* next_node;
    Piece* current_figure;
} LL_PIECES;

typedef struct{
    int boardL;
    int boardW;
    LL_PIECES* White_team;
    LL_PIECES* Black_team;
    LL_PIECES* dangerous[ BoardL][ BoardW]; // faster determining point safe and who is attacking
    Piece* board[ BoardL][BoardW];
} Board;

typedef struct{
    Position* from;
    Position* to;
    int id;
} A_MOVE;

typedef struct lots_moves_temp{
    struct lots_moves_temp* next_move;
    A_MOVE* curr_move;
} LOTS_MOVES;

bool is_checkmate( Board* Board, Piece* king);
bool is_stalemate( Board* Board, Piece* king);
bool is_draw( Board* Board);
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
            new_board->dangerous[ i][ j] = NULL;
        }
    }
    new_board->Black_team = NULL;
    new_board->White_team = NULL;
    return new_board;
}

A_MOVE* a_move_init( Position* new_point_to_move_from, Position* new_point_to_move_to, int new_id){
    A_MOVE* new_movement = malloc( sizeof( A_MOVE));
    new_movement->from = new_point_to_move_from;
    new_movement->to = new_point_to_move_to;
    new_movement->id = new_id;
    return new_movement;
}

LL_PIECES* ll_pieces_init( Piece* new_piece){
    LL_PIECES* new_ll_node = malloc( sizeof( LL_PIECES));
    new_ll_node->current_figure = new_piece;
    new_ll_node->next_node = NULL;
    return new_ll_node;
}

LOTS_MOVES* lots_moves_init( A_MOVE* new_movement){
    LOTS_MOVES* new_moves_node = malloc( sizeof( LOTS_MOVES));
    new_moves_node->curr_move = new_movement;
    new_moves_node->next_move = NULL;
    return new_moves_node;
}

void Piece_free( Piece* fig){
    if( fig == NULL) return;
    free( fig->pp);
    free( fig);
}

void LL_PIECES_free( LL_PIECES* ll){
    LL_PIECES* clean_tail;
    while( ll != NULL){
        Piece_free( ll->current_figure);
        clean_tail = ll;
        ll = ll->next_node;
        free( clean_tail);
    }
}

void Board_free( Board* boar){
    for( int i = 0; i < boar->boardL; i++){
        for( int j = 0; j < boar->boardW; j++){
            Piece_free( boar->board[i][j]);
            LL_PIECES_free( boar->dangerous[ i][ j]);
        }
    }
    LL_PIECES_free( boar->White_team);
    LL_PIECES_free( boar->Black_team);
    free( boar);
}

void MOVE_free( A_MOVE* movement){
    free( movement->from);
    free( movement->to);
    free( movement);
}

void LOTS_MOVES_free( LOTS_MOVES* a_trip){
    LOTS_MOVES* cleaning_pointer;
    while( a_trip != NULL){
        MOVE_free( a_trip->curr_move);
        cleaning_pointer = a_trip;
        a_trip = a_trip->next_move;
        free( cleaning_pointer);
    }
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

int evaluate_game_status( Board* Field){ // figures eaten up, number of safe points, proximity to rooks;
    Piece* Black_king = Field->Black_team->current_figure;
    int total_score = Field->boardL * Field->boardW;
    if( is_checkmate( Field, Black_king)) return total_score;
    if( is_stalemate( Field, Black_king)) return 0; // stalemate is considered a draw
    if( is_draw( Field)) return 0; // draw is the best you can hope for in BK case

    int current_score = 0;

    int safe_points = 0;
    Position* temp = position_init( 0, 0);
    for( int i_x = -1; i_x < 2; i_x++){
        for( int i_y = -1; i_y < 2; i_y++){
            temp->X = Black_king->pp->X + i_x;
            temp->Y = Black_king->pp->Y + i_y;
            if( Point_safe( Field, Black_king, temp) == 1) safe_points++;
        }
    }
    int score_for_point = -total_score / 9; // cuz more safe points is better for king and so it represents a lower score

    current_score = (safe_points * score_for_point) / 3; // num of safe points is 1/3 of equation;
    free( temp);

    LL_PIECES* ll_iter = Field->White_team;
    int board_d = sqrt( Field->boardL*Field->boardL + Field->boardW*Field->boardW); // with it we compare distance, 1 should be the most
    int distance;
    int distance_x, distance_y;
    int distance_points;

    while( ll_iter != NULL){
        if( ll_iter->current_figure != NULL){
            if( strcmp( ll_iter->current_figure->equation, Rook_Movement) == 0){
                current_score += ( total_score / 3);
                distance_x = ll_iter->current_figure->pp->X - Black_king->pp->X;
                distance_y = ll_iter->current_figure->pp->Y - Black_king->pp->Y;
                distance = sqrt( distance_x*distance_x + distance_y*distance_y);
                if( Point_safe( Field, Black_king, ll_iter->current_figure->pp))  distance_points = (distance / board_d) * (total_score / 6);
                else distance_points = total_score / 6;
                //printf("\n X = %d, Y = %d, C = %d -> %d", distance_x, distance_y, distance, distance_points);
                current_score += distance_points;
            }
        }
        ll_iter = ll_iter->next_node;
    }

    return current_score;
}

LOTS_MOVES* add_move_to_lots( LOTS_MOVES* lots, A_MOVE* lil_movement){
    LOTS_MOVES* iterator = lots;

    if( iterator == NULL){
        lots = lots_moves_init( lil_movement);
        return lots;
    }

    while( iterator->next_move != NULL){
        iterator = iterator->next_move;
    }
    iterator->next_move = lots_moves_init( lil_movement);
    return lots;
}

LOTS_MOVES* all_posible_moves( Board* Field, Piece* figure){
    LOTS_MOVES* all_the_moves = NULL;

    A_MOVE* temp_movement;
    Position* temp_origin;
    Position* temp_point;
    if( strcmp( figure->equation, King_Movement) == 0){
        for( int i_x = -1;  i_x < 2;  i_x++){
            for( int i_y = -1; i_y < 2; i_y++){
                if( i_x == 0 && i_y == 0) i_y++;
                temp_point = position_init(  figure->pp->X + i_x,  figure->pp->Y + i_y);
                temp_origin = position_init( figure->pp->X, figure->pp->Y);
                if( Point_safe( Field, figure, temp_point)){
                    temp_movement = a_move_init( temp_origin, temp_point, 0);
                    all_the_moves = add_move_to_lots( all_the_moves, temp_movement);
                }
            }
        }
    }else{
        for( int i = 1; figure->pp->X + i < Field->boardL; i++){
            if( Field->board[ figure->pp->X + i][ figure->pp->Y] != NULL) break;
                temp_point = position_init(  temp_point->X = figure->pp->X + i,  figure->pp->Y);
                temp_origin = position_init( figure->pp->X, figure->pp->Y);
                temp_movement = a_move_init( temp_origin, temp_point, 0);
                all_the_moves = add_move_to_lots( all_the_moves, temp_movement);
        }
        for( int i = 1; figure->pp->X - i >= 0; i++){
            if( Field->board[ figure->pp->X - i][ figure->pp->Y] != NULL) break;
                temp_point = position_init(  figure->pp->X - i,  figure->pp->Y);
                temp_origin = position_init( figure->pp->X, figure->pp->Y);
                temp_movement = a_move_init( temp_origin, temp_point, 0);
                all_the_moves = add_move_to_lots( all_the_moves, temp_movement);
        }
        for( int i = 1; figure->pp->Y + i < Field->boardW; i++){
            if( Field->board[ figure->pp->X][ figure->pp->Y + i] != NULL) break;
                temp_point = position_init(  figure->pp->X,  figure->pp->Y + i);
                temp_origin = position_init( figure->pp->X, figure->pp->Y);
                temp_movement = a_move_init( temp_origin, temp_point, 0);
                all_the_moves = add_move_to_lots( all_the_moves, temp_movement);
        }
        for( int i = 1; figure->pp->Y - i >= 0; i++){
            if( Field->board[ figure->pp->X][ figure->pp->Y - i] != NULL) break;
                temp_point = position_init(  figure->pp->X,  figure->pp->Y - i);
                temp_origin = position_init( figure->pp->X, figure->pp->Y);
                temp_movement = a_move_init( temp_origin, temp_point, 0);
                all_the_moves = add_move_to_lots( all_the_moves, temp_movement);
        }

    }

    return all_the_moves;
}

void jump( Board* Board, Piece* figure, Position* jump_to){
    Board->board[ jump_to->X][ jump_to->Y] = figure;
    Board->board[ figure->pp->X][ figure->pp->Y] = NULL;
    figure->pp->X = jump_to->X;
    figure->pp->Y = jump_to->Y;
}

A_MOVE* mini( Board* Field, LL_PIECES* our_team, LL_PIECES* their_team, int deaph);
A_MOVE* maxi( Board* Field, LL_PIECES* our_team, LL_PIECES* their_team ,int deaph);

A_MOVE* mini( Board* Field, LL_PIECES* our_team, LL_PIECES* their_team, int deaph){
    if( deaph == 0) {
        A_MOVE* score = a_move_init( NULL, NULL, evaluate_game_status( Field)); // no looking forwards so no position moving given
        return score;
    }

    Position* best_to_move; // point of piece we should move
    Position* best_move_to; // where we should move that piece

    int min = infinity; // number representing how good is a move
    A_MOVE* score;

    while( our_team != NULL){
        LOTS_MOVES* possibilities = all_posible_moves( Field, our_team->current_figure);
        LOTS_MOVES* cleanin = possibilities;
        while( possibilities != NULL){
            Piece* place_holder = Field->board[ possibilities->curr_move->to->X][ possibilities->curr_move->to->Y];
            jump( Field, our_team->current_figure, possibilities->curr_move->to);
            score = maxi( Field, their_team, our_team, deaph - 1);
            if( score->id < min){
                min = score->id;
                free( best_move_to);
                free( best_to_move);
                best_to_move = position_init( possibilities->curr_move->from->X, possibilities->curr_move->from->Y);
                best_move_to = position_init( possibilities->curr_move->to->X, possibilities->curr_move->to->Y);
            }
            free( score);
            jump( Field, our_team->current_figure, possibilities->curr_move->from);
            Field->board[ possibilities->curr_move->to->X][ possibilities->curr_move->to->Y] = place_holder;
            possibilities = possibilities->next_move;
        }
        LOTS_MOVES_free( cleanin);
        our_team = our_team->next_node;
    }

    A_MOVE* best_move = a_move_init( best_to_move, best_move_to, min);
    return best_move;
}

void remove_piece_from_ll( Board* Field, Piece* figure);

A_MOVE* maxi( Board* Field, LL_PIECES* our_team, LL_PIECES* their_team ,int deaph){
    if( deaph == 0){
        A_MOVE* score = a_move_init( NULL, NULL, evaluate_game_status( Field)); // no looking forwards so no position moving given
        return score;
    }

    Position* moving_to = position_init( 0, 0);
    Position* original_position = position_init( 0, 0);

    Position* best_to_move  = NULL; // point of piece we should move
    Position* best_move_to  = NULL; // where we should move that piece

    int max = -infinity; // number representing how good is a move
    A_MOVE* score;

    while( our_team != NULL){
        LOTS_MOVES* possibilities = all_posible_moves( Field, our_team->current_figure);
        LOTS_MOVES* cleanin = possibilities;
        original_position->X = our_team->current_figure->pp->X;
        original_position->Y = our_team->current_figure->pp->Y;

        while( possibilities != NULL){
            score = maxi( Field, their_team, our_team, deaph - 1);
            if( score->id > max){
                max = score->id;
                free( best_move_to);
                free( best_to_move);
                best_to_move = position_init( original_position->X, original_position->Y);
                best_move_to = position_init( moving_to->X, moving_to->Y);
            }
            free( score);
            possibilities = possibilities->next_move;
        }
        LOTS_MOVES_free( cleanin);
        our_team = our_team->next_node;
    }
    free( moving_to);
    free( original_position);

    A_MOVE* best_move = a_move_init( best_to_move, best_move_to, max);
    return best_move;
}


void main(void){
    srand( time(NULL));

    char pieces[4][2] = {"WR","WR","WK","BK"};
    Piece* kings[2];
    Piece* iter = NULL;
    int i_x, i_y;
    char color;
    int times = 0;
    LL_PIECES* new_ll;

    int bl = BoardL, bw = BoardW;
//    printf(" Lenght of chessboard table = ");
//    scanf("%d", &bl);
//    printf(" Width of chessboard table = ");
//    scanf("%d", &bw);

    Board* Chess_board = board_init( bl, bw);
//    getch();
//    getch();
//    system("cls");

    for( int i = 0; i < 4; i++){
        do{
            times++;
            if( iter != NULL) Chess_board->board[ iter->pp->X][ iter->pp->Y] = NULL;

            do i_x = rand()%Chess_board->boardL, i_y = rand()%Chess_board->boardW;
            while( Chess_board->board[i_x][i_y] != NULL);

            if( pieces[i][0] == 'B') color = ':';
            else color = '@';

            if( pieces[i][1] == 'K') iter = Piece_init( King_Movement, 'K', i_x, i_y, pieces[i][0], color);
            else                     iter = Piece_init( Rook_Movement, 'R', i_x, i_y, pieces[i][0], color);
        }while( Point_safe( Chess_board, iter, iter->pp) == 0);
        Chess_board->board[ iter->pp->X][ iter->pp->Y] = iter;
        new_ll = ll_pieces_init( iter);
        if( pieces[i][0] == 'B' && pieces[i][1] == 'K'){
            Chess_board->Black_team = new_ll;
        }else{
            new_ll->next_node = Chess_board->White_team;
            Chess_board->White_team = new_ll;
        }
        if( pieces[i][1] == 'K') kings[i/2] = iter;
        iter = NULL;
    }
    Piece_free( iter);
    int i = 0;
    Piece_free( iter);

    do{
        if( is_checkmate( Chess_board, kings[1])){
            printf("\n Black is checkmate!");
            break;
        }
        if( is_stalemate( Chess_board, kings[1])){
            printf("\n Black is stalemate!");
            break;
        }
        if( is_draw( Chess_board)){
            printf("\n Game is draw!");
            break;
        }

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
        if( Chess_board->board[ from->X][ from->Y] != NULL){
            if( Chess_board->board[ from->X][ from->Y]->Team != 'W'){
                // we chose a black piece
            }else{
                if( Move_piece( Chess_board, Chess_board->board[ from->X][ from->Y], to) == 0){ // we should repeat the move
                    for(int i = 0; i < Chess_board->boardW/2; i++);
                        for( int j = 0; j < SquareW; j++)
                            printf(" ");
                    printf("move not viable");
                }else{
                    // move successful time for blacks turn
                    A_MOVE* blacks_move = mini( Chess_board, Chess_board->Black_team, Chess_board->White_team, 1);
                    Move_piece( Chess_board, Chess_board->Black_team->current_figure, blacks_move->to);
                    free( blacks_move);
                }
            }
        }
        i++;
        getchar();
        system("cls");


    //}while( i < 10);
    }while( 1);
    drawBoard( Chess_board);
    Board_free( Chess_board);
    Piece_free( kings[ 0]);
    Piece_free( kings[ 1]);
}

void remove_piece_from_ll( Board* Field, Piece* figure){
    LL_PIECES* Team_removing = NULL;
    if( Field->board[ figure->pp->X][ figure->pp->Y]->Team == 'W') Team_removing = Field->White_team;
    if( Field->board[ figure->pp->X][ figure->pp->Y]->Team == 'B') Team_removing = Field->Black_team;

    if( Team_removing->current_figure == Field->board[ figure->pp->X][ figure->pp->Y]){
        if( Team_removing == Field->Black_team) Field->Black_team = Team_removing->next_node;
        if( Team_removing == Field->White_team) Field->White_team = Team_removing->next_node;
        LL_PIECES_free( Team_removing);
        return;
    }

    LL_PIECES* Team_removing_i = Team_removing;
    while( Team_removing_i->next_node != NULL){
        if( Team_removing_i->next_node->current_figure == Field->board[ figure->pp->X][ figure->pp->Y]){
            LL_PIECES* next = Team_removing_i->next_node;
            Team_removing_i->next_node = Team_removing_i->next_node->next_node;
            LL_PIECES_free( next);
            break;
        }
        Team_removing_i = Team_removing_i->next_node;
    }
}

bool Move_piece( Board* Board, Piece* figure, Position* move_to){
    if( Board == NULL) return 0;
    if( figure == NULL) return 0;
    if( move_to == NULL) return 0;
    if( figure->pp->X < 0 || figure->pp->X > Board->boardL) return 0;
    if( figure->pp->Y < 0 || figure->pp->Y > Board->boardW) return 0;
    if( move_to->X < 0 || Board->boardL < move_to->X) return 0;
    if( move_to->Y < 0 || Board->boardW < move_to->Y) return 0;
    if( Board->board[ move_to->X][ move_to->Y] != NULL)
        if( Board->board[ move_to->X][ move_to->Y]->Team == figure->Team) return 0; // cannot take from your team
    if( Board->board[ move_to->X][ move_to->Y] != NULL)
        if( strcmp( Board->board[ move_to->X][ move_to->Y]->equation, King_Movement) == 0) return 0; // cannot take a king
    if( strcmp( figure->equation, King_Movement) == 0){
        if( move_to->X - figure->pp->X > 1 || move_to->X - figure->pp->X < -1) return 0;
        if( move_to->Y - figure->pp->Y > 1 || move_to->Y - figure->pp->Y < -1) return 0;
        if( Point_safe( Board, figure, move_to) == 0) return 0;
        if( Board->board[ move_to->X][ move_to->Y] != NULL){
            remove_piece_from_ll( Board, Board->board[ move_to->X][ move_to->Y]);
            Piece_free( Board->board[ move_to->X][ move_to->Y]);
        }
        Board->board[ move_to->X][ move_to->Y] = figure;
        Board->board[ figure->pp->X][ figure->pp->Y] = NULL;
        figure->pp->X = move_to->X;
        figure->pp->Y = move_to->Y;

    }else if( strcmp( figure->equation, Rook_Movement) == 0){
        if( move_to->X != figure->pp->X && move_to->Y != figure->pp->Y) return 0;
        int m_x = move_to->X - figure->pp->X; // m_x = movement_x
        int m_y = move_to->Y - figure->pp->Y; // m_y = movement_y
        if( m_x > 0) m_x = 1;
        else if( m_x < 0) m_x = -1;
        if( m_y > 0) m_y = 1;
        else if( m_y < 0) m_y = -1;

        for( int i_x = figure->pp->X + m_x, i_y = figure->pp->Y + m_y;    i_x != move_to->X || i_y != move_to->Y;    i_x += m_x, i_y += m_y)
            if( Board->board[i_x][i_y] != NULL) return 0;

        if( Board->board[ move_to->X][ move_to->Y] != NULL){
            remove_piece_from_ll( Board, Board->board[ move_to->X][ move_to->Y]);
            Piece_free( Board->board[ move_to->X][ move_to->Y]);
        }
        Board->board[ move_to->X][ move_to->Y] = figure;
        Board->board[ figure->pp->X][ figure->pp->Y] = NULL;
        figure->pp->X = move_to->X;
        figure->pp->Y = move_to->Y;
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

bool is_draw( Board* Board){
    for( int i_x = 0; i_x < Board->boardL; i_x++){
        for( int i_y = 0; i_y < Board->boardW; i_y++){
            if( Board->board[ i_x][ i_y] != NULL){
                if( strcmp( Board->board[ i_x][ i_y]->equation, King_Movement) != 0) return 0;
            }
        }
    }
    return 1;
}

/*
        LL_PIECES* W_iter = Chess_board->White_team;
        while( W_iter != NULL){
            printf("\n %c %c - %d %d", W_iter->current_figure->Team, W_iter->current_figure->shape, W_iter->current_figure->pp->X, W_iter->current_figure->pp->Y);
            W_iter = W_iter->next_node;
        }
        LL_PIECES* B_iter = Chess_board->Black_team;
        while( B_iter != NULL){
            printf("\n %c %c - %d %d", B_iter->current_figure->Team, B_iter->current_figure->shape, B_iter->current_figure->pp->X, B_iter->current_figure->pp->Y);
            B_iter = B_iter->next_node;
        }

        remove_piece_from_ll( Chess_board, Chess_board->White_team->next_node->next_node->current_figure);
        printf("\n");

        W_iter = Chess_board->White_team;
        while( W_iter != NULL){
            printf("\n %c %c - %d %d", W_iter->current_figure->Team, W_iter->current_figure->shape, W_iter->current_figure->pp->X, W_iter->current_figure->pp->Y);
            W_iter = W_iter->next_node;
        }
         B_iter = Chess_board->Black_team;
        while( B_iter != NULL){
            printf("\n %c %c - %d %d", B_iter->current_figure->Team, B_iter->current_figure->shape, B_iter->current_figure->pp->X, B_iter->current_figure->pp->Y);
            B_iter = B_iter->next_node;
        }
    Board_free( Chess_board);
    return;

*/
