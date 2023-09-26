#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int wind_spd = 0;
enum wind_dir {SW=1,S,SE,W,STILL,E,NW,N,NE,VAR} direction=STILL;
enum clouds {CLEAR=1,FEW,SCATTERED,BROKEN,OVERCAST} sky=CLEAR;
int temp = 0;

int main(void)
{
	void update_weather(void);

	puts("Current weather conditions:");
	printf("%d\t%d\t%d\t%d\n",wind_spd,direction,sky,temp);
   update_weather();
	puts("The updated weather:");
	printf("%d\t%d\t%d\t%d\n",wind_spd,direction,sky,temp);

	return 0;
}

void update_weather(void)
{
	FILE *weather;
	char wind_d[5],cloud_a[20],cloud_b[20];
	int wind_s,t,check,count=0;
	char *windarray[10]={"SW","S","SE","W","STILL","E","NW","N","NE","VAR"};
	char *cloudarray[5]={"CLEAR","FEW","SCATTERED","BROKEN","OVERCAST"};

	weather=fopen("weather.txt","r");
	if(weather==NULL){
		puts("Error opening first file.");
		exit(0);
	}
	fscanf(weather,"time: %*d:%*d\n");
	fscanf(weather,"wind_dir: %s\n",&wind_d);
	fscanf(weather,"wind_spd: %d knots\n",&wind_s);
	check=fscanf(weather,"clouds: %s %*d %s %*d\n",&cloud_a,&cloud_b);
	printf("check = %d\n",check);
	fscanf(weather,"temp: %d C",&t);
	fclose(weather);

	while(stricmp(wind_d,windarray[count])!=0)
		count++;
	direction=count+1;
	wind_spd=wind_s;
	count=0;
	while(stricmp(cloud_a,cloudarray[count])!=0)
		count++;
	sky=count+1;
	temp=t;
}
