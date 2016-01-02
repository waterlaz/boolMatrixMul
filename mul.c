#include <stdint.h>
#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "constants.h"


#define SHIFT4(A) A = ((A & Top4) | (A<<4 & LB4) | (A>>4 & RB4))
#define SHIFT2(A) A = ((A & Top2) | (A<<6 & LB2) | (A>>2 & RB2))
#define SHIFT1(A) A = ((A & Top1) | (A<<7 & LB1) | (A>>1 & RB1))

#define SHIFT(A) do { SHIFT4(A); SHIFT2(A); SHIFT1(A); } while(0)

#define MIX1(R1, R2) ( (R1| R1<<1)&ODD_CLM | (R2| R2>>1)&EVEN_CLM ) 
#define MIX2(R1, R2) ( (R1| R1<<2)&ODD_CLM2 | (R2| R2>>2)&EVEN_CLM2 ) 
#define MIX4(R1, R2) ( (R1| R1<<4)&ODD_CLM4 | (R2| R2>>4)&EVEN_CLM4 )

#define CS8(X) (X<<8)|(X>>56)

#define __MUL(A, B, C) do { \
	uint64_t b = B; \
	uint64_t r1 = A&b; \
	b = CS8(b); \
	uint64_t r2 = A&b; \
	b = CS8(b); \
	uint64_t r3 = A&b; \
	b = CS8(b); \
	uint64_t r4 = A&b; \
	b = CS8(b); \
	uint64_t r5 = A&b; \
	b = CS8(b); \
	uint64_t r6 = A&b; \
	b = CS8(b); \
	uint64_t r7 = A&b; \
	b = CS8(b); \
	uint64_t r8 = A&b; \
	r1 = MIX1(r1, r2); \
	r3 = MIX1(r3, r4); \
	r5 = MIX1(r5, r6); \
	r7 = MIX1(r7, r8); \
	r1 = MIX2(r1, r3); \
	r5 = MIX2(r5, r7); \
	r1 = MIX4(r1, r5); \
	SHIFT(r1);  \
	C = r1; } while(0)

#define MUL(A, B, C) do { \
	uint64_t __MUL_a = A; \
	uint64_t __MUL_b = B; \
	uint64_t __MUL_c ; \
	__MUL(__MUL_a, __MUL_b, __MUL_c); \
	C = __MUL_c; } while(0)



void print_matrix(uint64_t a){
	int n = 64;
	while(n--){
		printf("%" PRIu64, a>>63);
		a = a<<1;
		if(n%8==0) printf("\n");
	}
}


uint64_t mul_mat(uint64_t a, uint64_t b){
	uint64_t res;
	MUL(a, b, res);
	return res;
}

uint64_t slow_select(int i, int j, uint64_t a){
	int shift = i*8+j;
	return (a<<shift)>>63;
}

void slow_set_or(int i, int j, uint64_t* a, uint64_t v){
	int shift = i*8+j;
	*a|=v<<(63-shift);
}

uint64_t slow_mul(uint64_t a, uint64_t b){
	uint64_t res = 0;
	int i, j, k;
	for(i=0; i<8; i++)
	for(j=0; j<8; j++)
	for(k=0; k<8; k++){
		slow_set_or(i, j, &res, slow_select(i, k, a)&&slow_select(j, k, b));
	}
	return res;
}

int test_mul(uint64_t a, uint64_t b){
	uint64_t res1, res2;
	//res1 = mul_mat(a, b);
	res1 = 0;
	MUL(a, b, res1);
	res2 = slow_mul(a, b);
	if(res1!=res2){
		printf("\nERROR!\n");
		print_matrix(a);
		printf("-X-X-X-X-X-X-X-\n");
		print_matrix(b);
		printf("-=-=-=-=-=-=-=-\n");
		print_matrix(res2);
		printf("-BUT GOT THIS:-\n");
		print_matrix(res1);
		printf("ERROR!\n\n");
		return 1;
	}
	return 0;
}

uint64_t random_matrix(){
	uint64_t a, b, c, d;
	a = rand();
	b = rand();
	c = rand();
	d = rand();
	b<<=16;
	c<<=32;
	d<<=48;
	return a^b^c^d;
}

/*
int main(){
	srand(time(NULL));
	uint64_t x = random_matrix();
	uint64_t y = random_matrix();
	test_mul(x, y);
	return 0;
}
*/
