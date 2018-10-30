
#include "Core.h"

void init_proto(Proto *p);

int main(int argc, char **argv)
{
	char *buffer;
	size_t lSize;
	Proto p;
	LoadS loader;
	int nupvalues;

	if (argc > 1) //Missed out path to file argument
		error("Usage: luadec *filename*");

	//Newer C++ code
	std::fstream file("luac.out", std::ios::binary | std::ios::in | std::ios::ate);
	lSize = file.tellg();
	file.seekg(0);


	buffer = (char *)malloc(sizeof(char)*lSize);
	if (buffer == NULL) //Unable to open the file
		error("Failed to allocate memory for file.");

	file.read(buffer, lSize);
	if (!file)
		error("Failed to read file");

	loader.pos = buffer;
	loader.n = lSize;
	if ((int)buffer[0] != 0x1b)
		error("Invalid Lua Chunk: Missing 0x1b");

	loader.pos++;
	read_header(&loader);
	
	init_proto(&p);
	//Header is correctly formatted. Now Gather data.
	
	load_val(&loader, &nupvalues, lu_byte);
	enter_function(&loader, &p);

	//Data gathering successful. Now decrypt instructions and print the data neatly formatted.
	//Done with buffer, can free it, also can close the file.
	//free(buffer);
	//file.close();
	parse_function(&p);
	
	//Debug setting: Remove after
	getchar();

	return 0;
}

void init_proto( Proto *p)
{
	p->code = NULL;
	p->codesize = 0;
	p->constants = NULL;
	p->is_vararg = 0;
	p->lastlinedefined = 0;
	p->linedefined = 0;
	p->lineinfo = NULL;
	p->maxstacksize = 0;
	p->numconstants = 0;
	p->numlineinfo = 0;
	p->numparams = 0;
	p->numprotos = 0;
	p->numupvalues = 0;
	p->numvars = 0;
	p->protos = NULL;
	p->size = 0;
	p->source = NULL;
	p->upvalues = NULL;
	p->vars = NULL;
}

