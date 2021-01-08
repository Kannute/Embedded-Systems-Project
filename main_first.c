#include <stdio.h>

long int det(int A00, int A01, int A02, int A10, int A11, int A12, int A20, int A21, int A22){
    return A00*(A11*A22 - A21*A12) - A01 *(A10*A22 -A20*A12) + A02*(A10*A21 - A20*A11);
}

int main(){


    double dziel = -5502300;

    long int tab[6] = {
        9600,
        -371790,
        211714200,
        -488400,
        660,
        73534200
    };
    int x1 = 50;
    int x2 = 200;
    int x3 = 120;
    int y1 = 30;
    int y2 =30;
    int y3 = 250;

    int x1_p = 490;
    int x2_p = 493;
    int x3_p = 2970;

    int y1_p = 10500;
    int y2_p = 33000;
    int y3_p = 2180;

    long int delta =  det(x1_p, y1_p, 1, x2_p, y2_p, 1, x3_p, y3_p, 1);
    long int dx1 =  det(x1, y1_p, 1, x2, y2_p, 1, x3, y3_p, 1);
    long int dx2 = det(x1_p, x1, 1, x2_p, x2, 1, x3_p, x3, 1);
    long int dx3 = det(x1_p, y1_p, x1, x2_p, y2_p, x2, x3_p, y3_p, x3);
    long int dy1 = det(y1, y1_p, 1, y2, y2_p, 1, y3, y3_p, 1);
    long int dy2 = det(x1_p, y1, 1, x2_p, y2, 1, x3_p, y3, 1);
    long int dy3 = det(det(x1_p, y1_p, y1, x2_p, y2_p, y2, x3_p, y3_p, y3);

    printf("\n\n");
    
    double alfax = (double)dx1/delta;
    double betax =  (double)dx2/delta;
    double deltax = (double)dx3/delta;

    double alfay = (double)dy1 / delta;
    double betay = (double)dy2/delta;
    double deltay = (double)dy3/delta;
    
    
    
    
    for(int i=0;i<6;i++){

        printf("%g\n", tab[i]/dziel);
    }
    return 0;
}
