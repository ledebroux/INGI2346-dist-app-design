/* hi.c */
#include "pvm3.h"
main()
{
        int ptid;
        char greetings[100];

        ptid=pvm_parent(); /*get tid of task that started me*/
        strcpy(greetings,"Hello world from ");
        gethostname(greetings + strlen(greetings));

        pvm_initsend(PvmDataDefault); /*initialize send buffer*/
        pvm_pkstr(greetings); /*XDR encode string into send buffer*/
        pvm_send(ptid,1);

        pvm_exit(); /*disconnect from the PVM*/
        exit(0);
}