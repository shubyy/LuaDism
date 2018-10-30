
#include "Core.h"

void error(const char *message)
{
	printf("%s\n", message);
	exit(0);
}


void read_header(LoadS *loader)
{
	const char *mismatch = "Format Mismatch: Chunk compile on non standard type try using 64-bit version";

	check_str(loader, LUA_SIGNATURE, sizeof(LUA_SIGNATURE) - 1, "Invalid Chunk: Missing LuaS.");
	if (!check_elm(loader, 0))
		error("Format Mismatch");
	loader->n--;
	check_str(loader, LUAC_DATA, sizeof(LUAC_DATA) - 1, "Format Mismatch: Corrupted.");
	check_size(loader, int);
	check_size(loader, size_t);
	check_size(loader, unsigned int);
	check_size(loader, long long);
	check_size(loader, double);
	//dbg
	//printf("%d\n", *((long long *)loader->pos));
	check_val(loader, LUAC_INT, long long);
	//dbg
	//printf("%f\n", *((double *)loader->pos));
	check_val(loader, LUAC_NUM, double);
}

void check_str(LoadS *loader, const char *buf2, size_t n, const char *err)
{
	if (memcmp(loader->pos, buf2, n) != 0)
		error(err);

	loader->pos += n;
	loader->n -= n;
	//debug
	//printf("0x%x\n", *(loader->pos));
}

void check_sizef(LoadS *loader, unsigned char size)
{
	if (((unsigned char)*(loader->pos++)) != size)
		error("Format Mismatch: Chunk compiled on different type (try using 64-bit version)");
	loader->n--;
}