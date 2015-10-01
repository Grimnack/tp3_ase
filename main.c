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
struct ctx_s *anneau =NULL;

void switch_to_ctx (struct ctx_s* ctx){
	/*Premiere etape, sort-on d'un contexte 
   si oui on sauvegarde le contexte qu'on a quitté*/
	if(ctx_courant!=NULL){
		asm("movl %%ebp, %0 \n\t movl %%esp, %1"
			:"=r" (ctx_courant->ctx_ebp),
			"=r" (ctx_courant->ctx_esp));
	}
	ctx_courant = ctx;
	/*Deuxieme etape, on change de contexte*/
	asm("movl %0, %%ebp \n\t movl %1, %%esp"
		::"r" (ctx->ctx_ebp),
		"r" (ctx->ctx_esp));
	if(ctx_courant->ctx_state==INITIALIZED){
    	ctx_courant->ctx_state=ACTIVABLE;
      	(*(ctx_courant->f))(ctx_courant->ctx_arg);
      	ctx_courant->ctx_state=TERMINATED;
		return ;
	};
	return ;

}


int create_ctx(int stack_size, func_t f, void* args){
	struct ctx_s *ctx ;
	ctx = malloc(sizeof(struct ctx_s));
	init_ctx(ctx,stack_size,f,args);
	if(anneau==NULL){
		ctx->suivant = ctx;
		anneau = ctx ;
	} else {
		ctx->suivant = anneau -> suivant ;
		anneau->suivant = ctx;
	}
	return 0;
}

void yield(void){
	struct ctx_s *tmp;
	if(ctx_courant==NULL)
		switch_to_ctx(anneau);
	else
		while (ctx_courant->suivant->ctx_state==TERMINATED && ctx_courant->suivant != ctx_courant){
			free(ctx_courant->suivant->stack);
			tmp = ctx_courant->suivant->suivant;
			free(ctx_courant->suivant);
			ctx_courant->suivant = tmp;
		}
	if (ctx_courant->suivant->ctx_state==TERMINATED)
		exit(0);
	switch_to_ctx(ctx_courant->suivant);
	return ;
}

void f_pong(void*);
void f_ping(void*);


int main (void){
	create_ctx(16384, f_ping, NULL);
	create_ctx(16384, f_pong, NULL);
	yield() ;

	exit(EXIT_SUCCESS);
}

/*fonction qui va changer de context vers le contexte de pong après avoir imprimé un char*/
void f_ping(void* args){
	while(1){
		printf("A");
		yield();
		printf("B");
		yield();
		printf("C");
		yield();
	}
}

/*fonction qui va changer de context vers le contexte de ping après avoir imprimé un char*/
void f_pong(void* args){
	while(1){
		printf("1");
		yield();
		printf("2");
		yield();
	}
}
