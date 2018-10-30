#include "Core.h"
#include "opcodes.h"
#include "string.h"

int level = 0; //increases in multiple of twos to make the spacing better

/*Would have done this in C++ instead of using shitty printf's but for some reason Visual Studio is retarded and this breaks if I use anything 
from the C++ standard library.*/

//Might plan to scrap all this and do it with string library later when it decides to work

void parse_function(Proto *p)
{
	int i;
	
	if(p->source)
		printf("-------Function: %s - Parameters: %d , Line defined: %d, Is Var Arg: %s ------------\n", p->source, p->numparams, p->linedefined, p->is_vararg == 1 ? "True" : "False");
	else
		printf("-------Function: %s - Parameters: %d , Line defined: %d, Is Var Arg: %s ------------\n", "(Unknown name)", p->numparams, p->linedefined, p->is_vararg == 1 ? "True" : "False");

	printf("%.*sUpvalues: %d\n", level + 2, space_filler, p->numupvalues);
	for(i = 0; i < p->numupvalues; i++)
		printf("%.*sValue Name: %d: %d\n", level + 6, space_filler, i, p->upvalues[i].name);

	printf("%.*sConstants: %d\n", level+2, space_filler, p->numconstants);
	for (i = 0; i < p->numconstants; i++)
	{
		switch (p->constants[i].tt)
		{
			case LUA_TNIL:
				printf("%.*sValue %d: %s\n", level + 6, space_filler, i, "(nil)");
				break;
			case LUA_TNUMBER:
				printf("%.*sValue %d: %f\n", level + 6, space_filler, i, p->constants[i].val.doub);
				break;
			case LUA_TNUMINT:
				printf("%.*sValue %d: %lld\n", level + 6, space_filler, i, p->constants[i].val.intval);
				break;
			case LUA_TSTRING:
				printf("%.*sValue %d: %s\n", level + 6, space_filler, i, p->constants[i].val.string);
				break;
			case LUA_TBOOLEAN:
				printf("%.*sValue %d: %s\n", level + 6, space_filler, i, p->constants[i].val.boolean == 1 ? "True" : "False");
				break;
		}
	}

	//decompile instructions
	printf("%.*sLocal Variables: %d\n\n", level + 2, space_filler, p->numvars);
	decompiler_main(p);

	//redo for other proto's too (functions inside this one)
	printf("%.*sFunctions Inside this: %d\n", level + 2, space_filler, p->numprotos);

	//print out each of them on a higher level
	for (i = 0; i < p->numprotos; i++)
	{
		level += 5;
		parse_function(p->protos[i]);
		level -= 5;
	}

}

void decompiler_main(Proto *p)
{
	int spaces = 10;
	Instruction *start = p->code;
	printf("----------------------------\n");
	printf("%s%.*s%s %s %s\n","Opcode", level + spaces - 6, space_filler, "A", "B", "C");
	for (int i = 0; i < p->codesize; i++)
	{
		int kB, kC;
		Instruction current = *start++;
		int opcode = GET_OP(current);
		//I really had no clue of a better way of doing this. C++ string classes refused to work and just broke everything, and the printf 
		//string.h lib is awful. So yeah I just typed everything out. Fun.
		switch (opcode)
		{
		case OP_MOVE:
			printf("%s%.*s%d %d\n", "MOVE", spaces - 4, space_filler, GETARG_A(current), GETARG_B(current));
			break;
		case OP_LOADK:
			printf("%s%.*s%d %d\n", "LOADK", spaces - 5, space_filler, GETARG_A(current), GETARG_Bx(current));
			break;
		case OP_LOADKX:
			printf("%s%.*s%d\n", "LOADKX", spaces - 6, space_filler, GETARG_A(current));
			break;
		case OP_LOADBOOL:
			printf("%s%.*s%d %d %d\n", "LOADBOOL", spaces - 8, space_filler, GETARG_A(current), GETARG_B(current), GETARG_C(current));
			break;
		case OP_LOADNIL:
			printf("%s%.*s%d %d\n", "LOADNIL", spaces - 7, space_filler, GETARG_A(current), GETARG_B(current));
			break;
		case OP_GETUPVAL:
			printf("%s%.*s%d %d\n", "GETUPVAL", spaces - 8, space_filler, GETARG_A(current), GETARG_B(current));
			break;
		case OP_GETTABLE:
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "GETTABLE", spaces - 8, space_filler, GETARG_A(current), GETARG_B(current), kC);
			break;
		case OP_GETTABUP:
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "GETTABUP", spaces - 8, space_filler, GETARG_A(current), GETARG_B(current), kC);
			break;
		case OP_SETTABUP:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "GETTABUP", spaces - 8, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_SETUPVAL:
			printf("%s%.*s%d %d\n", "SETUPVAL", spaces - 8, space_filler, GETARG_A(current), GETARG_B(current));
			break;
		case OP_ADD:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "ADD", spaces - 3, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_RETURN:
			printf("%s%.*s%d %d\n", "RETURN", spaces - 6, space_filler, GETARG_A(current), GETARG_B(current));
			break;
		case OP_SETTABLE:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "SETTABLE", spaces - 8, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_NEWTABLE:
			printf("%s%.*s%d %d %d\n", "NEWTABLE", spaces - 8, space_filler, GETARG_A(current), GETARG_B(current), GETARG_C(current));
			break;
		case OP_SELF:
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "SELF", spaces - 4, space_filler, GETARG_A(current), GETARG_B(current), kC);
			break;
		case OP_SUB:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "SUB", spaces - 3, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_MUL:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "MUL", spaces - 3, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_MOD:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "MOD", spaces - 3, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_POW:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "POW", spaces - 3, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_DIV:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "DIV", spaces - 3, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_IDIV:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "IDIV", spaces - 4, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_BAND:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "BAND", spaces - 4, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_BOR:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "BOR", spaces - 3, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_BXOR:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "BXOR", spaces - 4, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_SHL:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "SHL", spaces - 3, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_SHR:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "SHR", spaces - 3, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_UNM:
			printf("%s%.*s%d %d\n", "UNM", spaces - 3, space_filler, GETARG_A(current), GETARG_B(current));
			break;
		case OP_BNOT:
			printf("%s%.*s%d %d\n", "BNOT", spaces - 4, space_filler, GETARG_A(current), GETARG_B(current));
			break;
		case OP_NOT:
			printf("%s%.*s%d %d\n", "NOT", spaces - 3, space_filler, GETARG_A(current), GETARG_B(current));
			break;
		case OP_LEN:
			printf("%s%.*s%d %d\n", "LEN", spaces - 3, space_filler, GETARG_A(current), GETARG_B(current));
			break;
		case OP_CONCAT:
			printf("%s%.*s%d %d %d\n", "CONCAT", spaces - 6, space_filler, GETARG_A(current), GETARG_B(current), GETARG_C(current));
			break;
		case OP_JMP:
			printf("%s%.*s%d %d\n", "JMP", spaces - 3, space_filler, GETARG_A(current), GETARG_sBx(current));
			break;
		case OP_EQ:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "EQ", spaces - 2, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_LE:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "LE", spaces - 2, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_LT:
			kB = RKB(current);
			kC = RKC(current);
			printf("%s%.*s%d %d %d\n", "LT", spaces - 2, space_filler, GETARG_A(current), kB, kC);
			break;
		case OP_TEST:
			kC = RKC(current);
			printf("%s%.*s%d    %d\n", "TEST", spaces - 4, space_filler, GETARG_A(current), kC);
			break;
		case OP_TESTSET:
			printf("%s%.*s%d %d %d\n", "TESTSET", spaces - 7, space_filler, GETARG_A(current), GETARG_B(current), GETARG_C(current));
			break;
		case OP_CALL:
			printf("%s%.*s%d %d %d\n", "CALL", spaces - 4, space_filler, GETARG_A(current), GETARG_B(current), GETARG_C(current));
			break;
		case OP_TAILCALL:
			printf("%s%.*s%d %d %d\n", "TAILCALL", spaces - 8, space_filler, GETARG_A(current), GETARG_B(current), GETARG_C(current));
			break;
		case OP_FORLOOP:
			printf("%s%.*s%d %d\n", "FORLOOP", spaces - 7, space_filler, GETARG_A(current), GETARG_sBx(current));
			break;
		case OP_FORPREP:
			printf("%s%.*s%d %d\n", "FORPREP", spaces - 7, space_filler, GETARG_A(current), GETARG_sBx(current));
			break;
		case OP_TFORCALL:
			printf("%s%.*s%d    %d\n", "TFORCALL", spaces - 8, space_filler, GETARG_A(current), GETARG_C(current));
			break;
		case OP_TFORLOOP:
			printf("%s%.*s%d %d\n", "TFORLOOP", spaces - 8, space_filler, GETARG_A(current), GETARG_sBx(current));
			break;
		case OP_SETLIST:
			printf("%s%.*s%d %d %d\n", "SETLIST", spaces - 7, space_filler, GETARG_A(current), GETARG_B(current), GETARG_C(current));
			break;
		case OP_CLOSURE:
			printf("%s%.*s%d %d\n", "CLOSURE", spaces - 7, space_filler, GETARG_A(current), GETARG_Bx(current));
			break;
		case OP_VARARG:
			printf("%s%.*s%d %d\n", "VARARG", spaces - 6, space_filler, GETARG_A(current), GETARG_B(current));
			break;
		case OP_EXTRAARG:
			printf("%s%.*s%d\n", "EXTRAARG", spaces - 8, space_filler, GETARG_Ax(current));
			break;
		}
	}
	printf("----------------------------\n\n");
}

int RKB(int i)
{
	if (GETARG_B(i) & BITRK)
		return INDEXK(GETARG_B(i));
	else
		return GETARG_B(i);
}

int RKC(int i)
{
	if (GETARG_C(i) & BITRK)
		return INDEXK(GETARG_C(i));
	else
		return GETARG_C(i);
}

void print_spaces(int num)
{
	for (int i = 0; i < num; i++)
		printf(" ");
}