/*

Nalin Gabriel Prabindh
nalin.gaby@gmail.com

2015
 
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

static int perform_get_state(char* statename, int* val)
{
 *val = -1;
 if(!strcmp(statename, "touch"))
 {
   FILE* fp;
   fp = popen("cat /sys/class/msensor/sensor0/value0", "r"); 
   if(fp != NULL)
   {
	char result[10] = {0};
	fgets(result, 10-1, fp); 
	printf("result = %s\n", result);
	*val = atoi(result);
	fclose(fp);
   }
   printf("touch sensor returns %d\n", *val);
 }
 else if(!strcmp(statename, "color-red"))
 {
   FILE* fp;

   system("echo COL-COLOR > /sys/class/msensor/sensor2/mode");
   fp=popen("cat /sys/class/msensor/sensor2/value0", "r");
   if(fp != NULL)
   {
	char result[10]={0};
	fgets(result, 10-1, fp);
	printf("result = %s\n", result);
	*val = atoi(result);
	fclose(fp);
   }
   printf("color-red sensor returns %d\n", *val);
 }
 else if(!strcmp(statename, "distance"))
 {
   FILE* fp;
   system("echo IR-PROX > /sys/class/msensor/sensor0/mode");
   fp=popen("cat /sys/class/msensor/sensor0/value0", "r");
   if(fp != NULL)
   {
	char result[10]={0};
	fgets(result, 10-1, fp);
	printf("result = %s\n", result);
	*val = atoi(result);
	fclose(fp);
   }
   printf("IR sensor proximity returns %d\n", *val);
 }
 else if(!strcmp(statename, "remote"))
 {
   FILE* fp;
   system("echo IR-REMOTE > /sys/class/msensor/sensor0/mode");
   fp=popen("cat /sys/class/msensor/sensor0/value0", "r");
   if(fp != NULL)
   {
	char result[10]={0};
	fgets(result, 10-1, fp);
	printf("result = %s\n", result);
	*val = atoi(result);
	fclose(fp);
   }
   printf("IR sensor returns %d from remote-control\n", *val);
 }
  
 return 0;
}

void set_motor_state_default()
{
	system("echo time > /sys/class/tacho-motor/motor0/run_mode");
	system("echo off > /sys/class/tacho-motor/motor0/regulation_mode");
	system("echo 80 > /sys/class/tacho-motor/motor0/duty_cycle_sp");
	system("echo normal > /sys/class/tacho-motor/motor0/polarity_mode");
	system("echo 500 > /sys/class/tacho-motor/motor0/time_sp");

	system("echo time > /sys/class/tacho-motor/motor1/run_mode");
	system("echo off > /sys/class/tacho-motor/motor1/regulation_mode");
	system("echo 80 > /sys/class/tacho-motor/motor1/duty_cycle_sp");
	system("echo normal > /sys/class/tacho-motor/motor1/polarity_mode");
	system("echo 500 > /sys/class/tacho-motor/motor1/time_sp");
}

void set_motor_state_reverse_default()
{
	system("echo time > /sys/class/tacho-motor/motor0/run_mode");
	system("echo off > /sys/class/tacho-motor/motor0/regulation_mode");
	system("echo 80 > /sys/class/tacho-motor/motor0/duty_cycle_sp");
	system("echo inverted > /sys/class/tacho-motor/motor0/polarity_mode");
	system("echo 500 > /sys/class/tacho-motor/motor0/time_sp");

	system("echo time > /sys/class/tacho-motor/motor1/run_mode");
	system("echo off > /sys/class/tacho-motor/motor1/regulation_mode");
	system("echo 80 > /sys/class/tacho-motor/motor1/duty_cycle_sp");
	system("echo inverted > /sys/class/tacho-motor/motor1/polarity_mode");
	system("echo 500 > /sys/class/tacho-motor/motor1/time_sp");
}

void set_led_state_default()
{
 system("echo 0 > /sys/class/leds/ev3:red:left/brightness");
 system("echo 0 > /sys/class/leds/ev3:red:right/brightness");    
 system("echo 0 > /sys/class/leds/ev3:green:left/brightness");  
 system("echo 0 > /sys/class/leds/ev3:green:right/brightness");
}

#define MAX_STRAIGHT_IR_DIST (20)
#define MAX_MISSING_COUNT (10)
#define MIN_STRAIGHT_IR_DIST (5)

void main()
{
	int val;
	set_motor_state_default();
	set_led_state_default();
	int missing_count = 0;
	int run_count = 0;
	while(1)
	{
		run_count ++;
		if(run_count > 60) break;
		val = -1;
		sleep(1);
		perform_get_state("distance", &val);
		//if not present nearby
		if((val > MAX_STRAIGHT_IR_DIST) && (missing_count < MAX_MISSING_COUNT))
		{
			missing_count ++;
			//do a sweep - left
			system("echo 1 > /sys/class/tacho-motor/motor0/run");
			sleep(1);
			perform_get_state("distance", &val);
			if(val > MAX_STRAIGHT_IR_DIST)
			{
				//do a sweep - right
				system("echo 1 > /sys/class/tacho-motor/motor1/run");
				sleep(1);
				system("echo 1 > /sys/class/tacho-motor/motor1/run");
			}
		}
		else if(val < MIN_STRAIGHT_IR_DIST)
		{
			//do a - reverse+++
			set_motor_state_reverse_default();
			system("echo 1 > /sys/class/tacho-motor/motor0/run");
			system("echo 1 > /sys/class/tacho-motor/motor1/run");
			sleep(2);
			set_motor_state_default();
		}	
		else if(val <= MAX_STRAIGHT_IR_DIST) //follow
		{
			missing_count = 0;
			system("echo 1 > /sys/class/tacho-motor/motor0/run");
			system("echo 1 > /sys/class/tacho-motor/motor1/run");
		}
	}
cleanup_exit:
	system("echo 0 > /sys/class/leds/ev3:red:left/brightness");
	system("echo 0 > /sys/class/leds/ev3:red:right/brightness");    
	system("echo 0 > /sys/class/leds/ev3:green:left/brightness");  
	system("echo 0 > /sys/class/leds/ev3:green:right/brightness");
}

