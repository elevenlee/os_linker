////////////////////////////////////////////////////////////////
/*
	File Name:		Linker.h
	Instructor:		Prof. Mohamed Zahran
	Grader:			Libin Lu
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This Linker.h file includes
					MACRO, STRUCT DEFINITION,
					GLOBAL VARIABLE AND FUNCTION DECLARATION.
*/
////////////////////////////////////////////////////////////////

///////////////PRECOMPILER///////////////
#ifndef	LINKER_H_
#define LINKER_H_

///////////////DEBUG///////////////
//#define DEBUG 1
#ifdef DEBUG
	#define DEBUG_PRINT printf("%s-%s:%d:", __FILE__, __FUNCTION__, __LINE__)
#else
	#define DEBUG_PRINT
#endif

///////////////HEADER FILES///////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////MACRO///////////////
#define SYMBOL_LENTH		20
#define VARIABLE_NAME_LENTH	64
#define FILE_NAME_LENTH		25

///////////////STRUCT DEFINITION///////////////
struct map_link{					//Map Link
	int		add;					//code address
	int		number;					//code number
	int		module_add;				//module base address
	int		outflag;				//whether the address is outside the module 
	int		nodefine;				//whether the variable is defined
	char	word_type;				//word type
	char	real_type;				//real type
	char	*name;					//map name
	struct	map_link *next;			//link point
};

struct module_link{					//Module Link
	int		begin_add;				//Module's base address
	int		module_size;			//Module's size
	int		module_number;			//the number of Module
	struct	module_link *next;		//link point
	struct	map_link map_head;		//link with Map
};

struct symbol_link{							//Symbol Link
	int		symbol_add;						//variable address(absolute)
	int		used;							//whether the variable is used
	int		module;							//the Module number which variable is defined
	int		multiply_defined;				//whether the variable is mutiply defined
	char	symbol_name[SYMBOL_LENTH + 1];	//variable name
	struct	symbol_link *next;				//link point
};

///////////////GLOBAL VARIABLE///////////////
extern FILE		*in;						//input file pointer
extern FILE		*out;						//output file pointer
extern int		offset;						//offset of Moudle
extern struct	symbol_link table_head;		//symbol link structure
extern struct	module_link module_head;	//module link structure

///////////////FUNCTION DECLARATION///////////////
struct	symbol_link *findSymbolName(char *name);					//Find the Variable's name
struct	module_link *findModuleNumber(int num);						//Find the number of Module
struct	map_link	*findMapNumber(struct map_link *p,int num);		//Find the number of Map
int		checkSymbolName(char *name);								//Check whether the variable is multiply defined
int		findModuleMaxAddress(int num);								//Find Module's Max address
void	freeSymbolLink(struct symbol_link *p);						//Free the Symbol Link
void	freeModuleLink(struct module_link *p);						//Free the Module Link
void	freeMapLink(struct map_link *p);							//Free the Map Link
void	firstPass();												//First Pass function
void	secondPass();												//Second Pass function

#endif	//LINKER_H_
