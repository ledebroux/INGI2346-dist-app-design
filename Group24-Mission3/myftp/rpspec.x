

program PROG {
  version VERS {
    string RPWD(void) = 1;
    int32_t RCD(string) = 2;
    string RLS(void) = 3;
  } = 1;
} = 0x12345688;