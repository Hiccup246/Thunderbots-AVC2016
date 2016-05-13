# include <stdio.h>
# include <time.h>

extern "C" int init_hardware ();
extern "C" int Sleep ( int sec , int usec );
extern "C" int set_motor ( int motor, int speed );
extern "C" char get_pixel(int x, int y, int num);
extern "C" int take_picture();

int main (){
    init_hardware();
    int width = 320;
    int height = 480;
    int brightnessArray[width];
    int threshold = 128;
    int generatedArray[width];
    int errorArray[width];
    int leftSpeed=50;
    int rightSpeed=50;
    int baseSpeed = 50;
    double k=0.1; // constant

    // an array starting from -160 to 160
    int number = -160;
    for(int y=0; y<width; y++){
        generatedArray[y] = number;
        number++;
    }

    take_picture();
    int counter = 0;
    while(counter<10){
        int sumError=0;
        take_picture();
        for(int i=0; i<width; i++){
            char colour = get_pixel(height/2, i, 3);
            if(colour > threshold){
                brightnessArray[i] = 1;
            } else {
                brightnessArray[i] = 0;
            }
            // brightness * genArray
            errorArray[i] = brightnessArray[i] * generatedArray[i];
            sumError+=errorArray[i];
        }
        // calculate speed for left and right motors k has to be small like 0.1
        // leftmotor = baseSpeed - k*sumError
        // rightmotor = baseSpeed + k*sumError
        leftSpeed = baseSpeed - k*sumError; // sumError should be 0 for the robot to go in a straight line
        rightSpeed = baseSpeed + k*sumError;

        set_motor(1, leftSpeed);
        set_motor(2, rightSpeed);
        Sleep(0, 100000);
        counter++;
    }
    return 0;
}
