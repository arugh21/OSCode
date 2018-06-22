#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 1024
static char receive[BUFFER_LENGTH];

void print_menu();

int main(){

  char user_entry;
  int r, w;
  char* read_buff[BUFFER_LENGTH] = {0};
  char* write_buff[BUFFER_LENGTH] = {0};

  int fd = open("FILE_PATH", O_RDWR);
  if (fd < 0){
    printf("Failed to open device\n");
    return -1;
  }

  print_menu();
  scanf("%c", &user_entry);
  while(user_entry != "e"){

    //read testing
    if (user_entry == 'r'){
      printf("Number of bytes to be read: ");
      fscanf(stdin,"%d", &r);

      //try to catch byte numbers outside of scope
      if(r<0){
        printf("Error: Cannot read negative number of bytes\n");
        print_menu();
        scanf("%c", &user_entry);
      }
      else if (r > BUFFER_LENGTH){
        printf("User entered number greater than buffer size. Reading entire buffer.\n");
        r = BUFFER_LENGTH;
      }

      read_buff = malloc(r);
      int count = 0;
      int rd = read(fd, read_buff, r);

      for(int i = 0; i<r; i++){
        printf("%c\n", read_buffer[i]);
      }

      free(read_buffer);
      print_menu();
      scanf("%c", &user_entry);

    }

    //write testing
    else if(user_entry == 'w'){
      int wr;
      char* string_entry;
      printf("Enter the string to be written: ");
      scanf("%s", string_entry);
      if(strlen(string_entry)>BUFFER_LENGTH){
        perror("Error: String too long.\n", );
        return errno;
      }
      wr = write(fd, string_entry, strlen(string_entry));
      
      print_menu();
      scanf("%c",&user_entry);
    }
    else if(user_entry=='s'){
      int num_of_bytes = 0;
      printf("Choices: \n0: Set pointer to beginning of kernel buffer.\n1:Move current pointer by user selection.\n2:Set pointer to end of buffer");
      scanf("%d\n", %r);
      if(r==1){
        printf("How many bytes from the current pointer?");
        scanf("%d\n",&num_of_bytes);
      }
      seek(fd, num_of_bytes, r);
      print_menu();
      scanf("%c", &user_entry);
    }
  }
  fd.close();
  return 0;
}

void print_menu(){
  printf("\nPlease choose an option: \n
          r - read from the device \n
          w - write to device_buffer\n
          s - seek into device\n
          e - exit device\n");
}
