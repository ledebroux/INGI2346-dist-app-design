/*    rproc.c     */

#include "rpspec.h"

double *rproca_1_svc(data, rqstp)
struct arga *data;
struct svc_req *rqstp;
{
     static double result; long i;
     result = 1;
     for (i = 0; i < (data->arga2); i++)
          result = result * (1.0 + (double) (data->arga1)/100.0);
      /* compute compound interests; arga1=rate, arga2=nyears */
     return(&result);
}

struct arga *rprocb_1_svc(none, rqstp)
void *none;
struct svc_req *rqstp;

{
      static struct arga result;

       result.arga1 = (int) rand();
       result.arga2 = (int) rand();

      return(&result);
}

char **rprocc_1_svc(none, rqstp)
void *none;
struct svc_req *rqstp;

{
      char * result = malloc((strlen("ta mere")+1)*sizeof(char));
      result = "ta mere";

      return(&result);
}

