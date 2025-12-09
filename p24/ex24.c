#include <stdio.h>
#include "dbg.h"
#include <stdlib.h>
#define MAX_DATA 100

typedef enum EyeColor {
    BLUE_EYES, GREEN_EYES, BROWN_EYES,
    BLACK_EYES, OTHER_EYES
} EyeColor;

const char *EYE_COLOR_NAMES[] = {
    "Blue", "Green", "Brown", "Black", "Other"
};

typedef struct Person {
    int age;
    char first_name[MAX_DATA];
    char last_name[MAX_DATA];
    EyeColor eyes;
    float income;
} Person;

void getFile(char *file_name,char *buffer,int buffer_size,int length){
	FILE* file = fopen(file_name,"r");
	printf("Opening the file:%s\n",file_name);
	check(file!=NULL,"Failed to open the file!");
	int i=0;
	for(i=0;i<buffer_size && i<length;i++){
		char c=fgetc(file);
		if(c==EOF) {buffer[i]='\0';break;}
		else{buffer[i]=c;}
	}
	buffer[i-1]='\0';
	puts(buffer);
	fclose(file);
error:
	//return -1;
}

int main(int argc, char *argv[])
{
    Person you = {.age = 0};
    int i = 0;
    char *in = NULL;
	char file_name[MAX_DATA];
	int buffer_size=50;
	char buffer[buffer_size];
	printf("File Name:");
 	fgets(file_name,MAX_DATA-1,stdin);	
	for(int i=0;i<MAX_DATA;i++) if(file_name[i]=='\n'){file_name[i]='\0';break;} 
	printf("length:");
	int length=-1;
	scanf("%d",&length);
	getFile(file_name,buffer,buffer_size,length);
	getchar();
    printf("What's your First Name? ");
    in = fgets(you.first_name, MAX_DATA-1, stdin);
	for(int i=0;i<MAX_DATA;i++){
		if(you.first_name[i]==' '||you.first_name[i]=='\n') {
			for(int j=i;j<MAX_DATA-1;j++){you.first_name[j]=you.first_name[j+1];}
		}
	}
	you.first_name[MAX_DATA-1]='\0';
    check(in != NULL, "Failed to read first name.");

    printf("What's your Last Name? ");
    in = fgets(you.last_name, MAX_DATA-1, stdin);
    check(in != NULL, "Failed to read last name.");

    printf("How old are you? ");
//    int rc = fscanf(stdin, "%d", &you.age);
//    int rc = scanf("%d", &you.age);
	char tempRC[MAX_DATA];
	fgets(tempRC,8,stdin);
    int rc = atoi(tempRC);
    check(rc > 0, "You have to enter a number.");
	you.age=rc;
    printf("What color are your eyes:\n");
    for(i = 0; i <= OTHER_EYES; i++) {
        printf("%d) %s\n", i+1, EYE_COLOR_NAMES[i]);
    }
    printf("> ");

    int eyes = -1;
    rc = fscanf(stdin, "%d", &eyes);
    check(rc > 0, "You have to enter a number.");

    you.eyes = eyes - 1;
    check(you.eyes <= OTHER_EYES && you.eyes >= 0, "Do it right, that's not an option.");

    printf("How much do you make an hour? ");
    rc = fscanf(stdin, "%f", &you.income);
    check(rc > 0, "Enter a floating point number.");

    printf("----- RESULTS -----\n");

    printf("First Name: %s\n", you.first_name);
    printf("Last Name: %s", you.last_name);
    printf("Age: %d\n", you.age);
    printf("Eyes: %s\n", EYE_COLOR_NAMES[you.eyes]);
    printf("Income: %f\n", you.income);

    return 0;
error:

    return -1;
}
