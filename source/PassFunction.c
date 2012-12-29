////////////////////////////////////////////////////////////////////
/*
	File Name:		PassFunction.c
	Instructor:		Prof. Mohamed Zahran
	Grader:			Libin Lu
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This PassFunction.c file includes
					First Pass Function and Second Pass Function.
*/
////////////////////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Linker.h"

///////////////VARIABLE DENIFITION///////////////
FILE	*in;						//input file pointer
FILE	*out;						//output file pointer
int		offset = 0;					//offset of Moudle
struct	symbol_link table_head;		//symbol link structure
struct	module_link module_head;	//module link structure

///////////////FUNCTIONS///////////////
/*First Pass function
  --Calculate base address for each module
  --Generate symbol table with all variables from all modules and their absolute address
*/
void firstPass(){
	char	name[VARIABLE_NAME_LENTH];	//variable name, or type of address in code
	int		tmp_count;					//number of variables, or number of variables used, or module size
	int		module_count;				//number of module
	int		tmp_add;					//relative address of variables, or address of code 
	int		i;							//count number
	struct	symbol_link *node;			//Symbol Link
	struct	module_link *module_p;		//Module Link
	struct	map_link *map_p;			//Map Link

	//Initialize symbol_link and module_link structure
	table_head.next = NULL;
	module_head.next = NULL;

	//At the beginning, the base address is 0 and the module number is 1
	offset = 0;
	module_count = 1;

	//Read the first number in Definition List of each Module -- it represents the number of variables in this module
	while(fscanf(in, "%d", &tmp_count) != EOF){
		//Insert each variables into the Symbol Link
		for(i = 0; i < tmp_count; ++i){
			//Read the variable's name and address(relative address from the start of this module)
			fscanf(in, "%s%d", name, &tmp_add);
			if(strlen(name) > SYMBOL_LENTH){
				printf("Variable name is too long! Tale the first %d char.\n", SYMBOL_LENTH);
				name[SYMBOL_LENTH] = '\0';
			}
			//Check same name
			if(checkSymbolName(name) == 1){
				node = &table_head;			//Initialize Symbol Link
				while(node->next != NULL)
					node = node->next;

				node->next = (struct symbol_link *)malloc(sizeof(struct symbol_link));
				node = node->next;
				//Assign the variable
				strcpy(node->symbol_name, name);
				node->module = module_count;
				node->multiply_defined = 0;
				node->symbol_add = offset + tmp_add;
				node->used = 0;
				node->next = NULL;
			}
		}

		//Insert Module link
		module_p = &module_head;		//Initialize Module Link
		while(module_p->next != NULL)
			module_p = module_p->next;
		
		module_p->next = (struct module_link *)malloc(sizeof(struct module_link));
		module_p = module_p->next;
		//Assign the Module
		module_p->begin_add = offset;
		module_p->module_number = module_count;
		module_p->next = NULL;
		module_p->map_head.next = NULL;

		//Read the first number in Use List of each Module -- it represents the number of variables used in this module
		fscanf(in, "%d", &tmp_count);
		//Get the name of used variables, but it is not useful in the First Pass
		for(i = 0; i < tmp_count; ++i){
			fscanf(in, "%s", name);
		}

		//Read the first number in Code of each Module -- it represents the Module's size
		fscanf(in, "%d", &tmp_count);
		//Assign the Module
		module_p->module_size = tmp_count;

		map_p = &(module_p->map_head);		//Initialize Map Link
		//Insert each Code's style and address into the Map Link
		for(i = 0; i < tmp_count; ++i){
			//Read the Code's type and address
			fscanf(in, "%s%d", name, &tmp_add);

			map_p->next = (struct map_link*)malloc(sizeof(struct map_link));
			map_p = map_p->next;
			//Assign the Map
			map_p->add = tmp_add;
			map_p->next = NULL;
			map_p->word_type = name[0];
			map_p->real_type = 'I';
			map_p->number = i;
			map_p->outflag = 0;
			map_p->nodefine = 0;
			map_p->name = NULL;
			map_p->module_add = offset;
		}

		//Finish a Module and then operate the next Module
		++module_count;
		//The next Module's base addres is the Module's base address plus the Module's size
		offset = offset + tmp_count;
	}

	//Output the Symbol Table in the output files
	fprintf(out, "Symbol Table\n");
	//Let the node points the first Symbol
	node = table_head.next;

	while(node != NULL){
		//Find the Module's number in order to decide whether the variable is outside the Module, which is a kind of Error
		module_p = findModuleNumber(node->module);
		//If the variable is outside Module
		if(node->symbol_add >= module_p->begin_add + module_p->module_size){
			node->symbol_add = module_p->begin_add;
			fprintf(out, "%2s = %2d	Error: The value of %s is outside Module %d! Zero (relative) used.", node->symbol_name, node->symbol_add, node->symbol_name, node->module);
		}
		else{
			fprintf(out, "%2s = %2d", node->symbol_name, node->symbol_add);
		}
		//If multiply defined the variable
		if(node->multiply_defined == 1){
			fprintf(out, "	Error: This variable is multiply defined! First value used.");
		}
		fprintf(out, "\n");
		//Point to the next variable
		node = node->next;
	}
	fprintf(out, "\n");
}

/*Second Pass function
  --Relocate Relative addresses (instructions with R in front of them)
  --Resolve external reference (instructions with E in front of them)
*/
void secondPass(){
	char	name[VARIABLE_NAME_LENTH];	//variable name, or type of address in code
	int		tmp_count;					//number of variables, or number of variables used, or module size
	int		module_count;				//number of module
	int		map_count;					//number of map
	int		tmp_add;					//relative address of variables, or address of code
	int		i;							//count number
	struct	symbol_link *node;			//Symbol Link
	struct	module_link *module_p;		//Module Link
	struct	map_link *map_p;			//Map Link

	//Initialize the point in the input files, SEEK_SET is defined as 0 in stdio.h
	fseek(in, 0, SEEK_SET);

	//At the beginning, the module number is 1 and the map number is 0
	module_count = 1;
	map_count = 0;

	fprintf(out, "Memory Map\n");

	//Read the first number in Definition List of each Module, but it is not useful in the Second Pass
	while(fscanf(in, "%d", &tmp_count) != EOF){
		//Find the Module's number
		module_p = findModuleNumber(module_count);

		//Read the variable's name and address(relative address from the start of this module), but it is also not useful in the Second Pass
		for(i = 0; i < tmp_count; ++i){
			fscanf(in, "%s%d", name, &tmp_add);
		}

		//Read the first number in Use List of each Module -- it represents the number of variables used in this module
		fscanf(in, "%d", &tmp_count);
		//Resolve external addresses
		for(i = 0; i < tmp_count; ++i){
			//Get the name of used variables
			fscanf(in, "%s", name);
			//Find the Symbol name
			node = findSymbolName(name);
			//Refind the Module's Map
			map_p = module_p->map_head.next;
			
			while(map_p != NULL){
				//Find variable
				if(map_p->word_type == 'E' && map_p->add % 1000 == i && node != NULL){
					//Change external addresses
					map_p->add = 1000 * (map_p->add / 1000) + node->symbol_add;
					//Change the address's type
					map_p->real_type = 'E';
				}
				else if(node == NULL){	//If the variable is not defined
					//Assign the map
					map_p->add = 1000 * (map_p->add / 1000);
					map_p->nodefine = 1;
					map_p->name = (char *)malloc(SYMBOL_LENTH);
					strcpy(map_p->name, name);
				}
				//Point to the next map
				map_p = map_p->next;
			}
		}

		//Refind the Module's Map
		map_p = module_p->map_head.next;
		//Output the Memory Map in the output files
		while(map_p != NULL){
			//Find Relative addresses and relocate it to absolute addresses
			if(map_p->word_type == 'R' && map_p->real_type == 'I'){
				//Absolute address equals relative address plus module base address
				fprintf(out, "%2d: %5d", map_count++, map_p->add + map_p->module_add);
			}
			else{
				fprintf(out, "%2d: %5d", map_count++, map_p->add);
			}

			//If addresses out of Module size
			if(map_p->outflag == 1){
				fprintf(out, "	Error: Pointer in use link exceeds module size! Link terminated.");
			}
			//If the variable is not defined
			if(map_p->nodefine == 1){
				fprintf(out, "	Error: %s is not defined! Zero used.", map_p->name);
			}
			//Error information output
			if(map_p->word_type == 'E' && map_p->real_type == 'I'){
				fprintf(out, "	Error: E type address not on use link! Treated as I type.");
			}
			if(map_p->word_type == 'I' && map_p->real_type == 'E'){
				fprintf(out, "	Error: I type address on use link! Treated as E type.");
			}
			if(map_p->word_type == 'R' && map_p->real_type == 'E'){
				fprintf(out, "	Error: R type address on use link! Treated as E type.");
			}
			if(map_p->word_type == 'A' && map_p->real_type == 'E'){
				fprintf(out, "	Error: A type address on use link! Treated as E type.");
			}
			fprintf(out, "\n");
			//Point to the next map
			map_p = map_p->next;
		}

		//Read the first number in Code of each Module -- it represents the Module's size
		fscanf(in, "%d", &tmp_count);
		//Read the Code's type and address, but it is not useful
		for(i = 0; i < tmp_count; ++i){
			fscanf(in, "%s%d", name, &tmp_add);
		}
		//Finish a Module and then operate the next Module
		++module_count;
	}

	//warning information output
	fprintf(out, "\n");
	//Find the Symbol Link
	node = table_head.next;

	//output the variables that not used
	while(node != NULL){
		if(node->used == 0)
			fprintf(out, "Warning: %s was defined in Module %d but never used!\n", node->symbol_name, node->module);
		//Point to the next variable
		node = node->next;
	}

	//Free Symbol Link, Module Link and Map Link
	if(table_head.next != NULL){
		freeSymbolLink(table_head.next);
	}
	if(module_head.next != NULL){
		freeModuleLink(module_head.next);
	}
}
