
#include "Core.h"

int main(int argc, char **argv)
{
	char *buffer;
	size_t lSize;
	Proto p;
	LoadS loader;
	int nupvalues;

	if (argc <= 1) //Missed out path to file argument
		error("Usage: luadism(64) *filename*");

	//Newer C++ code
	std::fstream file(argv[1], std::ios::binary | std::ios::in | std::ios::ate);
	if (!file.is_open())
	{
		error("Failed to open file");
	}
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
	//Header is correctly formatted. Now Gather data.
	
	load_val(&loader, &nupvalues, lu_byte);
	enter_function(&loader, &p);

	//Data gathering successful. Now decrypt instructions and print the data neatly formatted.
	//Done with buffer, can free it, also can close the file.
	free(buffer);
	loader.pos = NULL;
	loader.n = 0;
	file.close();
	parse_function(&p);

	return 0;
}

