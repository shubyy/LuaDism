#pragma once

#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>

#define LUA_SIGNATURE "LuaS"
#define LUA_VERSION_MAJOR	5
#define LUA_VERSION_MINOR	3
#define LUA_VERSION_NUM		503
#define LUA_VERSION_RELEASE	5
#define LUA_VERSION 0x53
#define LUAC_DATA	"\x19\x93\r\n\x1a\n"
#define LUAC_INT 0x5678
#define LUAC_NUM 370.5
#define LUAI_MAXSHORTLEN	40

//Lua Types
#define LUA_TNONE		(-1)
#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8
#define LUA_TNUMFLT	(LUA_TNUMBER | (0 << 4))  /* float numbers */
#define LUA_TNUMINT	(LUA_TNUMBER | (1 << 4))
#define LUA_TSHRSTR	(LUA_TSTRING | (0 << 4))  /* short strings */




#define incr_buf(l,o) l->pos += o; l->n -= o;


//CoreHeader.cpp
#define check_elm(l,o) *(l->pos++) == o
#define check_size(l,o) check_sizef(l,sizeof(o))
#define check_val(l,o,t) if( *( (t *) l->pos) == o ) \
							{ l->pos += sizeof(t); l->n -= sizeof(t); } \
						 else \
							error("Format error: Unknown.");

//CoreVM.cpp

#define load_bytes(l,o,s) if( l->n > s) \
							{ memcpy((void *) o, (void *) l->pos, s); l->pos += s; l->n -= s; } \
						 else \
							error("Error: File ended too early");

#define load_val(l,o,t) if( (l)->n > sizeof( t )) \
							{ *(o) = *( (t *) (l)->pos); (l)->pos += sizeof(t); (l)->n -= sizeof(t); } \
						 else \
							error("Error: File ended too early");

//Lua internal types needed
typedef unsigned char lu_byte;
typedef unsigned int Instruction;

typedef union
{
	long long intval;
	double doub;
	char *string;
	bool boolean;
} Value;

typedef struct
{
	Value val;
	int tt;
} TValue;
							
typedef struct
{
	char *pos; //position in buffer
	int n; //unread
} LoadS;

typedef struct 
{
	char *name;  /* upvalue name (for debug information) */
	lu_byte instack;  /* whether it is in stack (register) */
	lu_byte idx;  /* index of upvalue (in stack or in outer function's list) */
} Upvaldesc;

typedef struct LocVar {
	char *varname;
	int startpc;  /* first point where variable is active */
	int endpc;    /* first point where variable is dead */
} LocVar;

typedef struct Proto
{
	char *source;
	int size;
	int linedefined;
	int lastlinedefined;
	int numparams;
	lu_byte is_vararg;
	lu_byte maxstacksize;
	Instruction *code;
	int codesize;
	TValue *constants;
	int numconstants;
	Upvaldesc *upvalues;
	int numupvalues;
	struct Proto **protos; //array of protos inside this function
	int numprotos;
	int *lineinfo;
	int numlineinfo;
	LocVar *vars;
	int numvars;

} Proto;




//CoreHeader.cpp
void check_str(LoadS *loader, const char *buf2, size_t n, const char *err);
void check_sizef(LoadS *loader, unsigned char size);
void read_header(LoadS *loader);
void error(const char *message);

//CoreVM.cpp
void enter_function(LoadS *loader, Proto *p);
char* load_string(LoadS *loader);
void Load_Code(LoadS *loader, Proto *p);
void Load_Constants(LoadS *loader, Proto *p);
void Load_Upvalues(LoadS *loader, Proto *p);
void Load_DebugInfo(LoadS *loader, Proto *p);
void Load_Protos(LoadS *loader, Proto *p);


//Functions for displaying
void parse_function(Proto *p);
void decompiler_main(Proto *p);