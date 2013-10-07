int getLs(char* str, char* path){
  DIR *dir;
  struct dirent *dent;
  // char buffer[50];
  // strcpy(buffer, args[1]);
  dir = opendir(path);   //this part
  if(dir!=NULL)
  {
      while((dent=readdir(dir))!=NULL)
          printf(dent->d_name);
  }
  close(dir);
}