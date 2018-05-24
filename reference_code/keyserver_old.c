#include <stdio.h>
#include <fcntl.h>   // open
#include <stdlib.h>
#include <string.h>  // strerror
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>  // daemon, close
#include <linux/input.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>

#define UNKNOWN_KEY "\0"
#define UK UNKNOWN_KEY

typedef struct input_event input_event;

#define KEY_RELEASE 0
#define KEY_PRESS 1


#define ASSERT_ON_COMPILE(expn) typedef char __C_ASSERT__[(expn) ? 1 : -1]

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

#define LOG(msg, ...) printf(msg "\n", ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) printf(msg "\n", ##__VA_ARGS__)


static char *shift_key_names[] = {
   UK, "<ESC>",
   "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+",
   "<Backspace>", "<Tab>",
   "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
   "{", "}", "<Enter>", "<LCtrl>",
   "A", "S", "D", "F", "G", "H", "J", "K", "L", ":",
   "\"", "~", "<LShift>",
   "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?",
   "<RShift>",
   "<KP*>",
   "<LAlt>", " ", "<CapsLock>",
   "<F1>", "<F2>", "<F3>", "<F4>", "<F5>", "<F6>", "<F7>", "<F8>", "<F9>", "<F10>",
   "<NumLock>", "<ScrollLock>",
   "<KP7>", "<KP8>", "<KP9>",
   "<KP->",
   "<KP4>", "<KP5>", "<KP6>",
   "<KP+>",
   "<KP1>", "<KP2>", "<KP3>", "<KP0>",
   "<KP.>",
   UK, UK, UK,
   "<F11>", "<F12>",
   UK, UK, UK, UK, UK, UK, UK,
   "<KPEnter>", "<RCtrl>", "<KP/>", "<SysRq>", "<RAlt>", UK,
   "<Home>", "<Up>", "<PageUp>", "<Left>", "<Right>", "<End>", "<Down>",
   "<PageDown>", "<Insert>", "<Delete>"
};

static char *key_names[] = {
   UK, "<ESC>",
   "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=",
   "<Backspace>", "<Tab>",
   "q", "w", "e", "r", "t", "y", "u", "i", "o", "p",
   "[", "]", "<Enter>", "<LCtrl>",
   "a", "s", "d", "f", "g", "h", "j", "k", "l", ";",
   "'", "`", "<LShift>",
   "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/",
   "<RShift>",
   "<KP*>",
   "<LAlt>", " ", "<CapsLock>",
   "<F1>", "<F2>", "<F3>", "<F4>", "<F5>", "<F6>", "<F7>", "<F8>", "<F9>", "<F10>",
   "<NumLock>", "<ScrollLock>",
   "<KP7>", "<KP8>", "<KP9>",
   "<KP->",
   "<KP4>", "<KP5>", "<KP6>",
   "<KP+>",
   "<KP1>", "<KP2>", "<KP3>", "<KP0>",
   "<KP.>",
   UK, UK, UK,
   "<F11>", "<F12>",
   UK, UK, UK, UK, UK, UK, UK,
   "<KPEnter>", "<RCtrl>", "<KP/>", "<SysRq>", "<RAlt>", UK,
   "<Home>", "<Up>", "<PageUp>", "<Left>", "<Right>", "<End>", "<Down>",
   "<PageDown>", "<Insert>", "<Delete>"
};

// bool isShift(uint16_t code) {
//    return code == KEY_LEFTSHIFT || code == KEY_RIGHTSHIFT;
// }

int isShift(uint16_t code) {
   if (code == KEY_LEFTSHIFT || code == KEY_RIGHTSHIFT) {
      return 1;
   }
   else {
      return 0;
   }
}

static int openKeyboardDeviceFile(char *deviceFile) {
   int kbd_fd = open(deviceFile, O_RDONLY);
   if (kbd_fd == -1) {
      // LOG_ERROR("%s", strerror(errno));
      printf("%s", strerror(errno));
      exit(-1);
   }

   return kbd_fd;
}

char *getKeyText(uint16_t code, uint8_t shift_pressed) {

   ASSERT_ON_COMPILE(ARRAY_SIZE(key_names) == ARRAY_SIZE(shift_key_names));

   // LOG("%s", shift_pressed ? "true" : "false");
   char **arr;
   if (shift_pressed != 0) {
      arr = shift_key_names;
   } else {
      arr = key_names;
   }

   if (code < ARRAY_SIZE(key_names)) {
      return arr[code];
   } else {
      // LOG("Unknown key: %u", code);
      printf("Unknown key: %u", code);
      return UNKNOWN_KEY;
   }
}

int kbd_fd;
uint8_t shift_pressed = 0;
input_event event;
int isPressed = 0; // if 0, it means release

int savedcode = 0;
int state = 0; // 0: no press, 1: left, 2: right
int portnum;
char* portstrptr;

void *server_code(void *);
void * key_code(void *);
void preparenumstr(char *savestr);


int main(int argc, char **argv) {


   printf("argc= %d\n", argc);
   if (argc != 2) {
      printf("please input port number and input device number");
      return 0;
   }

   char* portstr = argv[1];
   portstrptr = argv[1];

   portnum = atoi(portstr);
   printf("using portnum=%d\n", portnum);

// for my dell laptop, input
//    I: Bus=0011 Vendor=0001 Product=0001 Version=ab83
// N: Name="AT Translated Set 2 keyboard"
// P: Phys=isa0060/serio0/input0
// S: Sysfs=/devices/platform/i8042/serio0/input/input4
// U: Uniq=
// H: Handlers=sysrq kbd event4 leds

   char devicefile[] = "/dev/input/event16";
   kbd_fd = openKeyboardDeviceFile(devicefile);
   assert(kbd_fd > 0);




   // Daemonize process. Don't change working directory but redirect standard
   // inputs and outputs to /dev/null
   // if (daemon(1, 0) == -1) {
   //    LOG_ERROR("%s", strerror(errno));
   //    exit(-1);
   // }



   // printf("start server");


   // int listenfd = 0, client_sock = 0;
   // struct sockaddr_in serv_addr;

   // char sendBuff[1025];
   // char client_message[2000]={};
   // time_t ticks;

   // listenfd = socket(AF_INET, SOCK_STREAM, 0);
   // memset(&serv_addr, '0', sizeof(serv_addr));
   // memset(sendBuff, '0', sizeof(sendBuff));

   // serv_addr.sin_family = AF_INET;
   // serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   // serv_addr.sin_port = htons(5000);

   // bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

   // listen(listenfd, 10);

   // printf("listen finished");

   // int read_size;
   // client_sock = accept(listenfd, NULL, NULL);
   // if (client_sock < 0)
   // {
   //   perror("accept failed");
   //   return 1;
   // }
   // puts("Connection accepted");

   pthread_t thread1, thread2;
   int iret1, iret2;

   iret1 = pthread_create( &thread1, NULL, server_code, NULL);
   if (iret1) {
      printf("iret1 %d\n", iret1);
      exit(EXIT_FAILURE);
   }

   iret2 = pthread_create( &thread2, NULL, key_code, NULL);
   if (iret2) {
      printf("iret2 %d\n", iret2);
      exit(EXIT_FAILURE);
   }



   pthread_join(thread1, NULL);
   pthread_join(thread2, NULL);



   return 0;
}

void *server_code(void * ptr) {
   printf("start server code\n");


   int listenfd = 0, client_sock = 0;
   struct sockaddr_in serv_addr;

   char sendBuff[1025];
   char client_message[2000] = {};

   char numstr[10] = {};

   time_t ticks;

   listenfd = socket(AF_INET, SOCK_STREAM, 0);
   memset(&serv_addr, '0', sizeof(serv_addr));
   memset(sendBuff, '0', sizeof(sendBuff));

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   serv_addr.sin_port = htons(portnum);
   // serv_addr.sin_port = portnum;

   bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

   while (1) {
      listen(listenfd, 10);

      printf("listening..\n");

      // while(1)
      // {

      //     connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
      //     printf("%d\n",connfd);



      //     printf("accept returned fd >0");
      //     ticks = time(NULL);
      //     snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
      //     write(connfd, sendBuff, strlen(sendBuff));

      //     close(connfd);




      //     sleep(1);
      // }


      //accept connection from an incoming client
      int read_size;
      client_sock = accept(listenfd, NULL, NULL);
      if (client_sock < 0)
      {
         perror("accept failed");
         return;
      }
      puts("Connection accepted\n");
      printf("server going into while loop\n");
      //Receive a message from client
      while ( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
      {
         //Send the message back to client
         printf("sending back %s", client_message);
         preparenumstr(numstr);

         // write(client_sock , client_message , strlen(client_message));
         write(client_sock , numstr , 1);
         // write(client_sock , state , 1);
      }

      if (read_size == 0)
      {
         puts("Client disconnected");
         fflush(stdout);
      }
      else if (read_size == -1)
      {
         perror("recv failed");
      }

   }


   return;
}

// this code will keep reading the device
void * key_code(void *ptr) {
   while (1) {

      // if((read_size = recv(client_sock , client_message , 2000 , 0)) > 0){
      //    //Send the message back to client
      //   printf("sending back %s",client_message);
      //   write(client_sock , client_message , strlen(client_message));
      // }

      if (read(kbd_fd, &event, sizeof(input_event)) > 0) {
         if (event.type == EV_KEY) {

         	printf("some event happened");

            if (event.value == KEY_PRESS) {
               isPressed = 1;

               if (isShift(event.code)) {
                  shift_pressed++;
               }
               char *name = getKeyText(event.code, shift_pressed);
               // if (strcmp(name, UNKNOWN_KEY) != 0) {
               //    // LOG("%s", name);
               //    savedcode=event.code;

               //    // fputs(name, logfile);
               // }

               if ( strcmp(name, "j") == 0) {
                  state = 1;
                  printf("%s , left pressed\n", name);

               }
               else if (strcmp(name, "k") == 0) {
                  state = 2;
                  printf("%s right pressed\n", name);
               }
               else {

               }

            } else if (event.value == KEY_RELEASE) {
               state = 0;
               if (isPressed == 1) {
                  isPressed = 0;

                  char *fetchname = getKeyText(event.code, shift_pressed);


                  if (strcmp(fetchname, "j") == 0) {
                     printf("left released\n");
                  }
                  else if (strcmp(fetchname, "k") == 0) {
                     printf("right released\n");
                  }
                  else {
                     printf("%s key released\n", fetchname);
                  }



               }

               if (isShift(event.code)) {
                  shift_pressed--;
               }
            }
         }
         assert(shift_pressed >= 0 && shift_pressed <= 2);

      }
      else{
      	printf("nothing to read from char dev");
      }


   }

   // Config_cleanup(&config);
   // fclose(logfile);
   close(kbd_fd);
}


void preparenumstr(char *savestr) {
   switch (state) {
   case 0:
      savestr[0] = '0';
      break;
   case 1:
      savestr[0] = '1';
      break;
   case 2:
      savestr[0] = '2';
      break;
   default:
      break;


   }
}