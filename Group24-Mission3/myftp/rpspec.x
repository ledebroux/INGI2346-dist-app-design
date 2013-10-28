
struct file_part {
  opaque chunck<>;
  int32_t last;
};

struct file_desc {
  string filename<>;
  uint32_t offset;
  string pwd<>;
};

struct file_put {
  opaque chunck<>;
  string filename<>;
  int32_t offset;
  string pwd<>;
};

struct cd_res {
  int32_t code;
  string pwd<>;
};

struct cd_arg {
  string path<>;
  string pwd<>;
};

program PROG {
  version VERS {
    string RPWD(void) = 1;
    cd_res RCD(cd_arg) = 2;
    string RLS(string) = 3;
    file_part RGET(file_desc) = 4;
    int32_t RPUT(file_put) = 5;
  } = 1;
} = 0x12345688;