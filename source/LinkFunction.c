///////////////////////////////////////////////////////////////
/*
	File Name:		LinkFunction.c
	Instructor:		Prof. Mohamed Zahran
	Grader:			Libin Lu
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This LinkFunction.c file includes
					Several Functions that operate the Link.
*/
///////////////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Linker.h"

///////////////FUNCTIONS///////////////
/*Find the Variable's name
  Variable Definition:
  -- name: the name of Variable
*/
struct symbol_link *findSymbolName(char *name){
	struct symbol_link *p;

	p = table_head.next;
	while(p != NULL){
		if(strcmp(p->symbol_name, name) == 0){
			p->used = 1;
			return p;
		}
		p = p->next;
	}
	return NULL;
}

/*Find the number of Module
  Variable Definition:
  -- num: the number of Module
*/
struct module_link *findModuleNumber(int num){
	struct module_link *p;

	p = module_head.next;
	while(1){
		if(p->module_number == num)
			return p;
		p = p->next;
	}
	printf("Find module error!\n");
	return NULL;
}

/*Find the number of Map
  Variable Definition:
  -- p: pointer to the map
  -- num: the number of Code
*/
struct map_link *findMapNumber(struct map_link *p, int num){
	while(1){
		if(p->number == num)
			return p;
		p = p->next;
	}
	printf("Find map error!\n");
	return NULL;
}

/*Check whether the variable is multiply defined
  Variable Definition:
  -- name: the name of Variable
*/
int checkSymbolName(char *name){
	struct symbol_link *p;

	p = table_head.next;
	while(p != NULL){
		if(strcmp(name, p->symbol_name) == 0){
			p->multiply_defined = 1;
			return 0;
		}
		p = p->next;
	}
	return 1;
}

/*Find Module's Max address
  Variable Definition:
  -- num: the number of Module
*/
int findModuleMaxAddress(int num){
	struct module_link *p;

	p = module_head.next;
	while(1){
		if(p->module_number == num){
			return p->begin_add + p->module_size;
		}
		p = p->next;
	}
	printf("Find module max address error!\n");
	return 0;
}

/*Free the Symbol Link
  Variable Definition:
  -- p: symbol_link pointer
*/
void freeSymbolLink(struct symbol_link *p){
	if(p->next != NULL)
		freeSymbolLink(p->next);
	free(p);
}

/*Free the Module Link
  Variable Definition:
  -- p: module_link pointer
*/
void freeModuleLink(struct module_link *p){
	if(p->next != NULL)
		freeModuleLink(p->next);
	if(p->map_head.next != NULL)
		freeMapLink(p->map_head.next);
	free(p);
}

/*Free the Map Link
  Variable Definition:
  -- p: map_link pointer
*/
void freeMapLink(struct map_link *p){
	if(p->next != NULL)
		freeMapLink(p->next);
	if(p->name != NULL)
		free(p->name);
	free(p);
}
