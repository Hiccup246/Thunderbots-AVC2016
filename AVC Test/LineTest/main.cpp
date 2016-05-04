# include <stdio.h>
# include <time.h>

extern "C" int init_hardware ();

extern "C" int Sleep ( int sec , int usec );

extern "C" int set_motor ( int motor, int speed );

extern "C" char get_pixel(int x, int y, int num);

extern "C" int take_picture();

int main (){
    init_hardware();
    int counter = 100;
    int fullspeed = 255;
    int baseSpeed = 50;
    int width = 320;
    int height = 240;
    int brightnessArray[width];
    int threshold = 128;
    int generatedArray[width];
    int errorArray[width];

    int number = -160;
    for(int y=0; y<width; y++){
        generatedArray[y] = number;
        number++;
    }


    // take_picture()
    while(true){
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
        // calculate speed for left and right motors
        // leftmotor = baseSpeed - k*sumError
        // rightmotor = baseSpeed + k*sumError
    }


    // goes in a straight line
    for(int i=0; i<counter;i++){
        set_motor(1, 255);
        set_motor(2, 255);
        Sleep(0, 100000);
    }

    //turns left/right
    for(int i=0; i<counter; i++){
        set_motor(1, fullspeed);
        set_motor(2, 0.5*fullspeed);
        Sleep(0, 100000);
    }
    return 0;
}
