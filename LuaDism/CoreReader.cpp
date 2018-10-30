#include "Core.h"

void enter_function(LoadS *loader, Proto *p)
{
	p->source = load_string(loader);
	load_val(loader, &p->linedefined, int);
	load_val(loader, &p->lastlinedefined, int);
	load_val(loader, &p->numparams, lu_byte);
	load_val(loader, &p->is_vararg, lu_byte);
	load_val(loader, &p->maxstacksize, lu_byte);

	//Load the code
	Load_Code(loader, p);
	//load constants used by the function.
	Load_Constants(loader, p);
	//Load all the upvalues
	Load_Upvalues(loader, p);
	//Load any other functions contained within
	Load_Protos(loader, p);
	//load debug information
	Load_DebugInfo(loader, p);
}

char* load_string(LoadS *loader)
{
	int size;
	load_val(loader, &size, lu_byte);
	if (size == 0xFF)
		load_val(loader, &size, lu_byte);
	if (size == 0)
		return NULL;
	else if (--size <= LUAI_MAXSHORTLEN)
	{
		char *source = (char *)malloc(LUAI_MAXSHORTLEN);
		load_bytes(loader, source, size);
		source[size] = '\0';
		return source;
	}
	else
		error("Unsupported string value contained.");
	return NULL; //to remove annoying warning
}

void Load_Code(LoadS *loader, Proto *p)
{
	int codesize, total;
	load_val(loader, &codesize, int);
	total = codesize * sizeof(Instruction);
	p->code = (Instruction *)malloc(total);
	load_bytes(loader, p->code, total);
	p->codesize = codesize;
}

void Load_Constants(LoadS *loader, Proto *p)
{
	int i, n;
	load_val(loader, &n, int);
	int total = n * sizeof(TValue);
	p->constants = (TValue *) malloc(total);
	p->numconstants = n;

	for (i = 0; i < n; i++)
		p->constants[i].tt = LUA_TNIL;
	for (i = 0; i < n; i++)
	{
		lu_byte type;
		TValue *currk = &p->constants[i];
		load_val(loader, &type, lu_byte);

		switch (type)
		{
		case LUA_TNIL:
			currk->tt = LUA_TNIL;
			break;
		case LUA_TBOOLEAN:
			currk->tt = LUA_TBOOLEAN;
			load_val(loader, &currk->val.boolean, lu_byte);
			break;
		case LUA_TNUMFLT:
			currk->tt = LUA_TNUMBER;
			load_val(loader, &currk->val.doub, double);
			break;
		case LUA_TNUMINT:
			currk->tt = LUA_TNUMINT;
			load_val(loader, &currk->val.intval, long long);
			break;
		case LUA_TSHRSTR:
			currk->tt = LUA_TSTRING;
			currk->val.string = load_string(loader);
			break;
		default:
			break;
		}

	}
}

void Load_Upvalues(LoadS *loader, Proto *p)
{
	int i, n;
	load_val(loader, &n, int);
	int total = n * sizeof(Upvaldesc);
	p->upvalues = (Upvaldesc *) malloc(total);
	p->numupvalues = n;

	for (i = 0; i < n; i++)
		p->upvalues[i].name = NULL;

	for (i = 0; i < n; i++)
	{
		load_val(loader, &p->upvalues[i].instack, lu_byte);
		load_val(loader, &p->upvalues[i].idx, lu_byte);
	}
}

void Load_Protos(LoadS *loader, Proto *p)
{
	int i, n;
	load_val(loader, &n, int);
	int total = n * sizeof(Proto *);
	p->protos = (Proto **) malloc(total);
	p->numprotos = n;

	for (i = 0; i < n; i++)
		p->protos[i] = NULL;

	for (i = 0; i < n; i++)
	{
		p->protos[i] = (Proto *) malloc(sizeof(Proto));
		enter_function(loader, (Proto *) p->protos[i]);
	}
}

void Load_DebugInfo(LoadS *loader, Proto *p)
{
	int i, n;
	load_val(loader, &n, int);
	int total = n * sizeof(int);
	p->lineinfo = (int *) malloc(total);
	p->numlineinfo = n;
	load_bytes(loader, p->lineinfo, n * sizeof(int));

	load_val(loader, &n, int);
	total = n * sizeof(LocVar);
	p->vars = (LocVar *) malloc(total);
	p->numvars = n;

	for (i = 0; i < n; i++)
		p->vars[i].varname = NULL;

	for (i = 0; i < n; i++)
	{
		p->vars[i].varname = load_string(loader);
		load_val(loader, &p->vars[i].startpc, int);
		load_val(loader, &p->vars[i].endpc, int);
	}

	load_val(loader, &n, int);
	for (i = 0; i < n; i++)
	{
		p->upvalues[i].name = load_string(loader);
	}

}
