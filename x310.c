/***********************************************
x310.c	Control program for ControlByWeb WebRelay X-310

		Quick hack to check out modbus
		A bit messy, but works OK.

		Ed Cole colege@gmail.com

		22/5/2017

Usage overview - serves as a requirements specification.

USAGE:
	x310 {options}

	option:
		-s status. Returns the status of relays inputs and sensors
		-1|2|3|4 addresses the relays 1-4
		-e enables (switches on) the relay(s) addressed
		-d disables (switches off) the relay(s) addressed
		-v verbose
		-V prints version number

requires libmodbus under Ubuntu in package libmodbus-dev

to compile

gcc x310.c -o x310 `pkg-config --cflags --libs libmodbus`

Which expands to

gcc x310.c -o x310 -I/usr/local/include/modbus  -L/usr/local/lib -lmodbus

*************************************************/
#define VERSION "x310: v1.0.0 22/5/17 Ed Cole colege@gmail.com\n"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <modbus.h>

#define X310_IP "192.168.1.232"	// libmodbus doesn't do host resolution
#define RET_ERR -1


modbus_t *ctx;
uint8_t	 relays[]={0,0,0,0}; // the state of the relays
uint8_t  inputs[]={0,0,0,0}; // the state of the digital inputs
float	sen[4];	   // temperature sensors sen[0] = Sensor1
float	vin;	// Supply voltage - Vin
int	senAdd[]={272,274,276,278}; // modbus addresses for sensors
int	vinAdd = 16;			// modbus address for Vin

void print_status(int verbose);
void usageError(char *prog);

int main(int argc, char *argv[]){

int opt;

int	flag_s,
	flag_1,
	flag_2,
	flag_3,
	flag_4,
	flag_e,
	flag_d,
	flag_v = FALSE;

while ((opt = getopt(argc, argv, "s1234edvV")) != -1){
	switch (opt)
	{
		case 'V':
			printf(VERSION);
			exit(0);

		case 's': 
			  flag_s = TRUE;
		break;

		case '1': 
			flag_1 = TRUE;
		break;

		case '2': 
			flag_2 = TRUE;
		break;

		case '3': 
			flag_3 = TRUE;
		break;

		case '4':
			flag_4 = TRUE;
		break;

		case 'e':
			flag_e = TRUE;
                break;

		case 'd':
			flag_d = TRUE;
                break;
		
		case 'v':
                        flag_v = TRUE;
                break;

		default:
			usageError(argv[0]);


	}//switch

}// while

ctx = modbus_new_tcp(X310_IP, MODBUS_TCP_DEFAULT_PORT );
	if (ctx == NULL) {
		fprintf(stderr, "Unable to allocate libmodbus context\n");
		return RET_ERR;
	}//if
	if (modbus_connect(ctx) == -1) {
		fprintf(stderr, "Connection failed: %s\n",
				modbus_strerror(errno));
		modbus_free(ctx);
		return RET_ERR;
	}//if
	modbus_read_bits(ctx,0,4, relays);
	modbus_read_input_bits(ctx,0,4,inputs);
	// x310 holds sensor as 32bit float, but modbus_read_registers
	// reads 16bit unsigned int, so cast the float address to
	// a pointer to uint16_t and read 2 registers
	modbus_read_registers(ctx,vinAdd,2,(uint16_t *) &vin);
	// Read 8 registers = 4 X 32bit floats
	modbus_read_registers(ctx,senAdd[0],8,(uint16_t *) &sen[0]);
	if (flag_s==TRUE){
		printf("Initial state\n");
		printf("=============\n");
		print_status(flag_v);
	}//if
	if (flag_1==TRUE){
		if (flag_e==TRUE){
			relays[0] = TRUE;
			if (flag_v==TRUE){
                               	printf("Turning Relay 1 on\n");
                       	}
		}
		if (flag_d==TRUE){
			relays[0] = FALSE;
			if (flag_v==TRUE){
				printf("Turning Relay 1 off\n");
			}
		}
	}
	if (flag_2==TRUE){
		if (flag_e==TRUE){
			relays[1] = TRUE;
			if (flag_v==TRUE){
				printf("Turning Relay 2 on\n");
			}
		}
		if (flag_d==TRUE){
			relays[1] = FALSE;
			if (flag_v==TRUE){
				printf("Turning Relay 2 off\n");
			}
		}
	}   

	if (flag_3==TRUE){
		if (flag_e==TRUE){
			relays[2] = TRUE;
			if (flag_v==TRUE){
				printf("Turning Relay 3 on\n");
			}
		}
		if (flag_d==TRUE){
			relays[2] = FALSE;
			if (flag_v==TRUE){
				printf("Turning Relay 3 off\n");
			}
		}
	}

	if (flag_4==TRUE){
		if (flag_e==TRUE){
			relays[3] = TRUE;
			if (flag_v==TRUE){
				printf("Turning Relay 4 on\n");
			}
		}
		if (flag_d==TRUE){
			relays[3] = FALSE;
			if (flag_v==TRUE){
				printf("Turning Relay 4 off\n");
			}
		}
	}


	if (flag_s==TRUE){ // print new status
		printf("\nFinal state\n");
		printf("===========\n");
		print_status(flag_v);
	}

	modbus_write_bits(ctx,0,4,relays);
		
	return(0);

}//main

void print_status(int verbose){
	printf("Inputs = %x %x %x %x\n",
		inputs[0],inputs[1],inputs[2],inputs[3]);

	printf("Relays  = %x %x %x %x\n",
		relays[0],relays[1],relays[2],relays[3]);
	if (verbose==TRUE){
		if (relays[0]==TRUE){
			printf("Ralay 1 ON\n");
		}else{
			printf("Relay 1 OFF\n");
		}
		if (relays[1]==TRUE){
                        printf("Relay 2 ON\n");
                }else{
			printf("Relay 2 OFF\n");
                }
		if (relays[2]==TRUE){
                        printf("Relay 3 ON\n");
                }else{
                        printf("Relay 3 OFF\n");
                }       
		if (relays[3]==TRUE){
			printf("Relay 4 ON\n");
		}else{
			printf("Relay 4 OFF\n");
		}
			
	}//if flag_v
	printf("Sensors = %.2f %.2f %.2f %.2f\n",
		sen[0],sen[1],sen[2],sen[3]);
	printf("Voltage = %.2f\n",
			vin);
}//print_status

	
//************   usageError
void usageError(char *prog)
{
    fprintf(stderr,"\nUsage:\n %s [options]\n",prog);
    fprintf(stderr,"\nOptions:\n");
    fprintf(stderr," -s status\t\tprint status of inputs, relays and sensors\n");
    fprintf(stderr," -v verbose\t\t\n");
    fprintf(stderr," -[1234] relay\t\taddresses one or more relays\n");
    fprintf(stderr," -e enable\t\tenbles (switches on) addressed relay(s)\n");
    fprintf(stderr," -d disable\t\tdisables (switches off) addressed relay(s)\n");
    fprintf(stderr," -V version\t\tprints program version number\n");
    exit(1);
}

