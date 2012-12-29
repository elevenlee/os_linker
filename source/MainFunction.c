///////////////////////////////////////////////
/*
	File Name:		MainFunction.c
	Instructor:		Prof. Mohamed Zahran
	Grader:			Libin Lu
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This MainFunction.c file
					includes Main Function.
*/
///////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Linker.h"

///////////////FUNCTIONS///////////////
/*
  Main Function
*/
int main(int argc, char *argv[]){
	char inputfile[FILE_NAME_LENTH];	//input file name
	char outputfile[FILE_NAME_LENTH];	//output file name

	//Initialize the char Array
	memset(inputfile, 0, FILE_NAME_LENTH);
	memset(outputfile, 0, FILE_NAME_LENTH);

	//Get the input file name
	printf("Input the MODULE file name:");
	scanf("%s", inputfile);
	//Open input file
	if((in = fopen(inputfile, "r")) == NULL){
		fprintf(stderr, "Can not OPEN file for READ!\n");
		exit(-1);
	}
	//Get the output file name
	printf("Input the RESULT file name:");
	scanf("%s", outputfile);
	//Open output file
	if((out = fopen(outputfile, "w")) == NULL){
		fprintf(stderr, "Can not OPEN file for WRITE!\n");
		exit(-1);
	}

	//Relocating relative addresses
	firstPass();
	//Resolving absolute addresses
	secondPass();

	//Close the file pointer
	fclose(in);
	fclose(out);

	return 0;
}
