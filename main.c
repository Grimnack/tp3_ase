#include <stdio.h>
#include <stdlib.h>
/* tp3 de Matthieu Caron et Antoine Amara */


typedef void (func_t) (void *) ;
/*type enum nous permettant de connaitre l'etat d'un contexte, dans le tp2 le */
enum ctx_state_e { INITIALIZED, ACTIVABLE, TERMINATED} ;

struct ctx_s {
	void* ctx_ebp ;
	void* ctx_esp ;
	int magic ;
	void* ctx_arg ;
	func_t* f ;
	enum ctx_state_e ctx_state;
	struct ctx_s * suivant;
	char * stack ;
} ;

int init_ctx (struct ctx_s* ctx, int stack_size, func_t f, void* args){
   void* top ;
   ctx->stack = malloc(stack_size) ;
   top = ctx->stack + stack_size - sizeof(void*) ;
   ctx->ctx_ebp = top;
   ctx->ctx_esp = top;
	ctx->magic = 0xB00B5666 ;
	ctx->f = f;
	ctx->ctx_arg = args ;
	ctx-> ctx_state = INITIALIZED;
	return 0;
}

struct ctx_s *ctx_courant =NULL ;


