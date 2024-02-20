#include "trug.h"
#include <stdint.h>

/*-----------Global Variable Definitions ------------ */
static uint32_t v1=0;
static uint32_t v2=0;

/*-----------End Global Variable Definitions ------------ */

int rand_pick(){
    /*
     * randomly pick AD or BC to be the key
     * This function uses random number generator
     * For Full security, this should not be used but Full check instead
     */

    uint32_t random=gen_rand();
    uint32_t mask=0x00000001;
    return random & mask;

}



/*----------- Benchmark Functions Declaration ------------ */

void stub2()
{
    int x=3;
    x=x+4;
}


void stub_func(){
    uint32_t membuf[7];
    __asm(" str.w r14, [r13,#0xc]\n");              //manipulate the counter to get the Return Addr on membuf[3]
    membuf [7] = &membuf;
    membuf [4] =gen_rand();                         //this generates "C"
    membuf [5] =membuf[3] ^ membuf [4];             //this generates "D"
    membuf[0]=v1^membuf[5];                         //store "AD"
    membuf[1]=v2^membuf[4];                         //store "BC"
    membuf[2]=v1 ^ v2;                              //store "AB"
    v1=v1^ membuf [4];                              //update global register AC
    v2=v2^ membuf [5];                              //update global register BD




    /*
     * This parts deal with return clean up
     */
//    v1= membuf[1] ^ membuf[2];                      // this gives me AC
//    v2= membuf[0] ^ membuf[2];                      // this gives me BD




    if(rand_pick()==1)      //we use AD
    {
        if(!((membuf[0] ^ v1 ^ membuf[0] ^ v2) == (v1 ^ v2))){ //check if abcd == abcd validation
            while(1);
        }
        membuf[6]=membuf[0] ^ v1;
    }
    else                    //we use BC
    {
        if(!((membuf[1] ^ v1 ^ membuf[1] ^ v2) == (v1 ^ v2))){ //check if abcd == abcd validation
            while(1);
        }
        membuf[6]=membuf[1] ^ v2;
    }



    //clean the stack first
    __asm(" ldr r5, [r13, #0x18]");
    __asm(" str.w r5, [r13, #0x24]\n");

    //this return was not expected to hit
    return;
}


/*----------- Main Function ------------ */
int main(void)
{
    v1=gen_rand();
    v2=gen_rand();



    while(1){
        stub_func();
    }
	return 0;
}
