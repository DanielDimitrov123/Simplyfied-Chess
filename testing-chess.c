#include<stdio.h>
#include<stdlib.h>

typedef struct{
    int x;
    int y;
} point;

point* position_init( int new_x, int new_y){
    point* new_point = malloc( sizeof( point));
    new_point->x = new_x;
    new_point->y = new_y;
    return new_point;
}

int main(){
    point* p1 = position_init( 0, 0);
    point* p2 = position_init( 0, 0);

    printf("Welcome To testing-chess\n");
    printf("Give us x and y fror p1 ");
    scanf("%d %d", &p1->x, &p1->y);
    printf("Give us x and y fror p2 ");
    scanf("%d %d", &p2->x, &p2->y);
    getchar();
    system("cls");

        int movement_x = p2->x - p1->x;
        int movement_y = p2->y - p1->y;
        if( movement_x > 0) movement_x = 1;
        else if( movement_x < 0) movement_x = -1;
        if( movement_y > 0) movement_y = 1;
        else if( movement_y < 0) movement_y = -1;
        printf(" ok1");
        printf(" %d %d\n", movement_x, movement_y);
        for( int i_x = p1->x, i_y = p1->y;    i_x != p2->x || i_y != p2->y;    i_x += movement_x, i_y += movement_y)
            printf("\n x = %d, y =%d", i_x, i_y);

    movement_x = movement_x * movement_x / movement_x;
    movement_y = movement_y * movement_y / movement_y;

    printf("%d %d", movement_x, movement_y);
    printf("\nA computer science portal ");

    free( p1);
    free( p2);
}
