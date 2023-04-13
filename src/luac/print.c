/*
** $Id: print.c,v 1.32 2000/11/06 20:04:36 lhf Exp $
** print bytecodes
** See Copyright Notice in lua.h
*/

#include <stdio.h>
#include <stdlib.h>

#include "luac.h"

/* macros used in print.h, included in PrintCode */
#define P_OP(x) printf("%-11s\t", x)
#define P_NONE
#define P_AB printf("%d %d", GETARG_A(i), GETARG_B(i))
#define P_F                                                                    \
  printf("%d %d\t; %p", GETARG_A(i), GETARG_B(i), tf->kproto[GETARG_A(i)])
#define P_J printf("%d\t; to %d", GETARG_S(i), GETARG_S(i) + at + 1)
#define P_Q PrintString(tf, GETARG_U(i))
#define P_K printf("%d\t; %s", GETARG_U(i), tf->kstr[GETARG_U(i)]->str)
#define P_L PrintLocal(tf, GETARG_U(i), at - 1)
#define P_N printf("%d\t; " NUMBER_FMT, GETARG_U(i), tf->knum[GETARG_U(i)])
#define P_S printf("%d", GETARG_S(i))
#define P_U printf("%u", GETARG_U(i))

static void PrintString(const Proto *tf, int n) {
  const char *s = tf->kstr[n]->str;
  printf("%d\t; ", n);
  putchar('"');
  for (; *s; s++) {
    switch (*s) {
    case '"':
      printf("\\\"");
      break;
    case '\a':
      printf("\\a");
      break;
    case '\b':
      printf("\\b");
      break;
    case '\f':
      printf("\\f");
      break;
    case '\n':
      printf("\\n");
      break;
    case '\r':
      printf("\\r");
      break;
    case '\t':
      printf("\\t");
      break;
    case '\v':
      printf("\\v");
      break;
    default:
      putchar(*s);
      break;
    }
  }
  putchar('"');
}

static void PrintLocal(const Proto *tf, int n, int pc) {
  const char *s = luaF_getlocalname(tf, n + 1, pc);
  printf("%u", n);
  if (s != NULL)
    printf("\t; %s", s);
}

static void PrintCode(const Proto *tf) {
  const Instruction *code = tf->code;
  const Instruction *p = code;
  for (;;) {
    int at = p - code + 1;
    Instruction i = *p;
    int line = luaG_getline(tf->lineinfo, at - 1, 1, NULL);
    printf("%6d\t", at);
    if (line >= 0)
      printf("[%d]\t", line);
    else
      printf("[-]\t");
    switch (GET_OPCODE(i)) {
        case OP_END: P_OP("END"); P_NONE; break;
        case OP_RETURN: P_OP("RETURN"); P_U; break;
        case OP_CALL: P_OP("CALL"); P_AB; break;
        case OP_TAILCALL: P_OP("TAILCALL"); P_AB; break;
        case OP_PUSHNIL: P_OP("PUSHNIL"); P_U; break;
        case OP_POP: P_OP("POP"); P_U; break;
        case OP_PUSHINT: P_OP("PUSHINT"); P_S; break;
        case OP_PUSHSTRING: P_OP("PUSHSTRING"); P_Q; break;
        case OP_PUSHNUM: P_OP("PUSHNUM"); P_N; break;
        case OP_PUSHNEGNUM: P_OP("PUSHNEGNUM"); P_N; break;
        case OP_PUSHUPVALUE: P_OP("PUSHUPVALUE"); P_U; break;
        case OP_GETLOCAL: P_OP("GETLOCAL"); P_L; break;
        case OP_GETGLOBAL: P_OP("GETGLOBAL"); P_K; break;
        case OP_GETTABLE: P_OP("GETTABLE"); P_NONE; break;
        case OP_GETDOTTED: P_OP("GETDOTTED"); P_K; break;
        case OP_GETINDEXED: P_OP("GETINDEXED"); P_L; break;
        case OP_PUSHSELF: P_OP("PUSHSELF"); P_K; break;
        case OP_CREATETABLE: P_OP("CREATETABLE"); P_U; break;
        case OP_SETLOCAL: P_OP("SETLOCAL"); P_L; break;
        case OP_SETGLOBAL: P_OP("SETGLOBAL"); P_K; break;
        case OP_SETTABLE: P_OP("SETTABLE"); P_AB; break;
        case OP_SETLIST: P_OP("SETLIST"); P_AB; break;
        case OP_SETMAP: P_OP("SETMAP"); P_U; break;
        case OP_ADD: P_OP("ADD"); P_NONE; break;
        case OP_ADDI: P_OP("ADDI"); P_S; break;
        case OP_SUB: P_OP("SUB"); P_NONE; break;
        case OP_MULT: P_OP("MULT"); P_NONE; break;
        case OP_DIV: P_OP("DIV"); P_NONE; break;
        case OP_POW: P_OP("POW"); P_NONE; break;
        case OP_CONCAT: P_OP("CONCAT"); P_U; break;
        case OP_MINUS: P_OP("MINUS"); P_NONE; break;
        case OP_NOT: P_OP("NOT"); P_NONE; break;
        case OP_JMPNE: P_OP("JMPNE"); P_J; break;
        case OP_JMPEQ: P_OP("JMPEQ"); P_J; break;
        case OP_JMPLT: P_OP("JMPLT"); P_J; break;
        case OP_JMPLE: P_OP("JMPLE"); P_J; break;
        case OP_JMPGT: P_OP("JMPGT"); P_J; break;
        case OP_JMPGE: P_OP("JMPGE"); P_J; break;
        case OP_JMPT: P_OP("JMPT"); P_J; break;
        case OP_JMPF: P_OP("JMPF"); P_J; break;
        case OP_JMPONT: P_OP("JMPONT"); P_J; break;
        case OP_JMPONF: P_OP("JMPONF"); P_J; break;
        case OP_JMP: P_OP("JMP"); P_J; break;
        case OP_PUSHNILJMP: P_OP("PUSHNILJMP"); P_NONE; break;
        case OP_FORPREP: P_OP("FORPREP"); P_J; break;
        case OP_FORLOOP: P_OP("FORLOOP"); P_J; break;
        case OP_LFORPREP: P_OP("LFORPREP"); P_J; break;
        case OP_LFORLOOP: P_OP("LFORLOOP"); P_J; break;
        case OP_CLOSURE: P_OP("CLOSURE"); P_F; break;
    }
    printf("\n");
    if (i == OP_END)
      break;
    p++;
  }
}

#define IsMain(tf) (tf->lineDefined == 0)

#define SS(x) (x == 1) ? "" : "s"
#define S(x) x, SS(x)

static void PrintHeader(const Proto *tf) {
  printf("\n%s " SOURCE_FMT " (%d instruction%s/%d bytes at %p)\n",
         IsMain(tf) ? "main" : "function", SOURCE, S(tf->ncode),
         tf->ncode * Sizeof(Instruction), tf);
  printf("%d%s param%s, %d stack%s, ", tf->numparams, tf->is_vararg ? "+" : "",
         SS(tf->numparams), S(tf->maxstacksize));
  printf("%d local%s, %d string%s, %d number%s, %d function%s, %d line%s\n",
         S(tf->nlocvars), S(tf->nkstr), S(tf->nknum), S(tf->nkproto),
         S(tf->nlineinfo));
}

void luaU_printchunk(const Proto *tf) {
  int i, n = tf->nkproto;
  PrintHeader(tf);
  printf("--------\n");
  PrintCode(tf);
  for (i = 0; i < n; i++)
    luaU_printchunk(tf->kproto[i]);
}
