#include <stdio.h>

long int det(int A00, int A01, int A02, int A10, int A11, int A12, int A20, int A21, int A22){
    return A00*(A11*A22 - A21*A12) - A01 *(A10*A22 -A20*A12) + A02*(A10*A21 - A20*A11);
}

    double alfax;
    double betax;
    double deltax;

    double alfay;
    double betay;
    double deltay;



void calibrate()
{
    int x1 = 64;//50;
    int y1 = 384;//30;

    int x2 = 192;//200;
    int y2 =192;//30;

    int x3 = 192;//120;
    int y3 = 576;//250;

    int x1_p = 678;//490;
    int y1_p = 2169;//10500;

    int x2_p = 2807;//493;
    int y2_p = 1327;//33000;

    int x3_p = 2629;//2970;
    int y3_p = 3367;//2180;

    long int delta =  det(x1_p, y1_p, 1, x2_p, y2_p, 1, x3_p, y3_p, 1);
    long int dx1 =  det(x1, y1_p, 1, x2, y2_p, 1, x3, y3_p, 1);
    long int dx2 = det(x1_p, x1, 1, x2_p, x2, 1, x3_p, x3, 1);
    long int dx3 = det(x1_p, y1_p, x1, x2_p, y2_p, x2, x3_p, y3_p, x3);
    long int dy1 = det(y1, y1_p, 1, y2, y2_p, 1, y3, y3_p, 1);
    long int dy2 = det(x1_p, y1, 1, x2_p, y2, 1, x3_p, y3, 1);
    long int dy3 = det(x1_p, y1_p, y1, x2_p, y2_p, y2, x3_p, y3_p, y3);

    printf("\n\n");

    alfax = (double)dx1/delta;
    betax =  (double)dx2/delta;
    deltax = (double)dx3/delta;

    alfay = (double)dy1 / delta;
    betay = (double)dy2/delta;
    deltay = (double)dy3/delta;







}

int getX(int x,int y)
{
    return (alfax* x + betax* y + deltax)/1;
}


int getY(int x, int y)
{
    return (alfay* x + betay* y + deltay)/1;
}

int main(){

    calibrate();
    printf("%04d\n",34);
    printf("-----------\n");
    printf("%g\n",alfax);
    printf("%g\n",betax);
    printf("%g\n",deltax);

    printf("-----------\n");

    printf("%g\n",alfay);
    printf("%g\n",betay);
    printf("%g\n",deltay);


    return 0;
}
