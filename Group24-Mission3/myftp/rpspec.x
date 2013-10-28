
struct file_part {
  opaque chunck<>;
  int32_t last;
};

struct file_desc {
  string filename<>;
  uint32_t offset;
};


program PROG {
  version VERS {
    string RPWD(void) = 1;
    int32_t RCD(string) = 2;
    string RLS(void) = 3;
    file_part RGET(file_desc) = 4;
  } = 1;
} = 0x12345688;