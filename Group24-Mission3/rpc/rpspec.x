/*    rpspec.x    */

struct arga {
  int arga1;
  int arga2;
};

program PROG {
  version VERS {
    double RPROCA(arga) = 1;  /* proc #1 */
    arga RPROCB(void) = 2;  /* proc #2 */
    string RPROCC(void) = 3;  /* proc #3 */
  } = 1;        /* vers #1 */
} = 0x12345678;       /* prog num*/