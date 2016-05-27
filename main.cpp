# include <stdio.h>
# include <time.h>

extern "C" int init_hardware ();
extern "C" int init(int d_lev);
extern "C" int Sleep ( int sec , int usec );
extern "C" int set_motor ( int motor, int speed );
extern "C" char get_pixel(int x, int y, int num);
extern "C" int take_picture();
//network
extern "C" int read_analog(int ch_adc);
extern "C" int connect_to_server( char server_addr[14],int port);
extern "C" int send_to_server(char message[7]);
extern "C" int receive_from_server(char message[7]);


#define WIDTH 320
#define HEIGHT 240

int main (){
    init_hardware();
    // init(0);

    //int width = 320;
    //int height = 240;
    int brightnessArray[WIDTH];
    int threshold = 150;
    int generatedArray[WIDTH];
    int errorArray[WIDTH];
    int lagErrorArray[WIDTH];
    int baseSpeed = 35;
    int leftSpeed=baseSpeed;
    int rightSpeed=baseSpeed;
    bool check=true;
    int countrev=0;
    int previousError = 0;
    int derivative = 0;
    int kd = 0.005;
    bool checkintersection=true;
    int intcount=0;
    int sumError=0;
    int incount=0;
    bool right90=true;
    bool left90=true;
    //network
    bool gate = false;
    char ip[14] = "130.195.6.196";
    char message[7];
    int ir_sensor = read_analog(0);

    double k=0.001; // constant

    // an array starting from -160 to 160
    int number = -160;
    for(int y=0; y<WIDTH+1; y++){
        generatedArray[y] = number;
        number++;
    }

    int counter = 0;
    int zeroCount = 0;
    while(counter < 2000){
        sumError=0;
        take_picture();
        for(int i=0; i<WIDTH; i++){
            char colour = get_pixel(i, HEIGHT/2, 3);
            if(colour > threshold){
                brightnessArray[i] = 1;
            } else {
                brightnessArray[i] = 0;
            }
            // brightness * genArray
            errorArray[i] = brightnessArray[i] * generatedArray[i];

	    /*if(counter%2==1){
		lagErrorArray[i] = errorArray[i];
	    }*/
            sumError += errorArray[i];
        }
        derivative = ((sumError-previousError)/0.005);
        printf("Sum error: %d",sumError);


        //90 DEGREE TURNS

        //Right and left 90 Degree turn
        //int nine=0;

        if(sumError>6000){         //sum should be large as the right hand side(0-160) will be white
            right90=true;
        }else{
            right90=false;
        }
        if(sumError<-6000){    //sum should be very small as the left hand side(0 to -160) will be white
            left90=true;
        }else{
            left90=false;
        }

        if(right90){
            while(sumError>0){
                set_motor(0,baseSpeed);
                set_motor(1,-baseSpeed);
            }
        }

        if(left90){
            while(sumError<0){
                set_motor(0,-baseSpeed);
                set_motor(1,baseSpeed);
            }
        }



        // calculate speed for left and right motors k has to be small like 0.1

        //INTERSECTION CODE


        //for(int in=0;in<WIDTh;in++){
        //if((birghtnessArray[h])==1){    //Goes through array and because the camera should read all white the sum should have very few 0's
        //checkintersection=true;                     //if it counts 4 blacks then its not an intersection
        //}
        //else if{intcount<4){
        //checkintersection=true;
        //incount++
        //}
        //else if(intcount==4){
        //checkintersection=false;
        //break;
        //}
        //}

        //if(checkintersection){
     	//set_motor(0,45)
        //set_motor(1,45)
        //}


    //}//~~~~~unsure what this is for/what it closes off
        for(int h=0;h<WIDTH;h++){
            if((brightnessArray[h])==0){
                check=false;
            }else if(countrev<2){
                check=false;//this used to be true, changed it to false in case of the last error vlaue.
                countrev+=1;
            }else if (countrev==2){
                check=true;
                break;
            }
            if ((brightnessArray[h])==1){
            	checkintersection=true;
            }else if(intcount<4){
                checkintersection=true;
                incount++;
            }else if(intcount==4){
                checkintersection=false;
                break;
            }
        //continue;
        }

        if(check==true){
            leftSpeed = baseSpeed + (k*sumError)+(kd*derivative); // sumError should be 0 for the robot to go in a straight line
            rightSpeed = baseSpeed - (k*sumError)-(kd*derivative);
	    /* if(leftSpeed<baseSpeed){
		leftSpeed = 0
	    } else if(rightSpeed<baseSpeed){
		rightSpeed = 0;
	    }
		*/
            if(leftSpeed > 255){
                leftSpeed = 255;
            }
            else if(leftSpeed < -255){
                leftSpeed = -255;
            }
            if(rightSpeed > 255){
                rightSpeed = 255;
            } else if(leftSpeed < -255){
                rightSpeed = -255;
            }

            set_motor(1, leftSpeed); // Motor 1 is left motor//left
            set_motor(2, rightSpeed);//right
            printf("Left Speed = %d, Right Speed =%d\n", leftSpeed, rightSpeed);
            Sleep(0, 50000);
            counter++;
                if(counter==1000){
                    set_motor(1, 0); // Motor 1 is left motor
                    set_motor(2, 0);
                }
        } else {
            //int countback=0;
            //while(countback<4){
            set_motor(1,-baseSpeed);
            set_motor(2,-baseSpeed);
            //Sleep(0,5000000);
            //countback++;
            //}
        }
        if(gate==false){ // && ir_sensor>50?
            printf("%d", ir_sensor);
            set_motor(1, 0);
            set_motor(2, 0);
            if(ir_sensor > 1){
                send_to_server("Please");
                receive_from_server(message);
                send_to_server(message);
                gate = true;
            }
        //might be buggy and need to double check
        printf("%s", message);
        }
    }
    return 0;
}
