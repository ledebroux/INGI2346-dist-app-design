/* hello.c */
#include <stdio.h>
#include "pvm3.h"
int main()
{
        int cc, tid;
        char greetings[100];

        printf("I'm t %x\n",pvm_mytid());
        /* create the second task, executing program "hi" */
        cc = pvm_spawn("hi",(char **)0,0,"",1,&tid);
        if(cc == 1){
                cc = pvm_recv(-1,-1); /*wait for a message*/
                pvm_bufinfo(cc,(int *)0,(int *)0,&tid); /*get tid of sender*/
                pvm_upkstr(greetings); /*extract string from receive buffer 
                                               into greetings*/
                printf("from t%x: %s\n",tid,greetings);
        }else printf("Can't start task hi\n");
        pvm_exit(); /*disconnect from the PVM*/
        exit(0);
        return 0;
}