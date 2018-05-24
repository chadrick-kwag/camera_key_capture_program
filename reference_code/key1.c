#include <stdio.h>
#include <fcntl.h>   // open
#include <stdlib.h>
#include <string.h>  // strerror
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>  // daemon, close
#include <linux/input.h>

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

int isShift(uint16_t code){
   if(code==KEY_LEFTSHIFT || code == KEY_RIGHTSHIFT){
      return 1;
   }
   else{
      return 0;
   }
}

static int openKeyboardDeviceFile(char *deviceFile) {
   int kbd_fd = open(deviceFile, O_RDONLY);
   if (kbd_fd == -1) {
      // LOG_ERROR("%s", strerror(errno));
      printf("%s",strerror(errno));
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

int main(int argc, char **argv) {
   
   char devicefile[] = "/dev/input/event14";
   int kbd_fd = openKeyboardDeviceFile(devicefile);
   assert(kbd_fd > 0);

   

   
   // Daemonize process. Don't change working directory but redirect standard
   // inputs and outputs to /dev/null
   // if (daemon(1, 0) == -1) {
   //    LOG_ERROR("%s", strerror(errno));
   //    exit(-1);
   // }

   uint8_t shift_pressed = 0;
   input_event event;
   int isPressed=0; // if 0, it means release

   int savedcode=0;
   int state=0; // 0: no press, 1: left, 2: right
   while (read(kbd_fd, &event, sizeof(input_event)) > 0) {
      if (event.type == EV_KEY) {

         if (event.value == KEY_PRESS) {
            isPressed=1;

            if (isShift(event.code)) {
               shift_pressed++;
            }
            char *name = getKeyText(event.code, shift_pressed);
            // if (strcmp(name, UNKNOWN_KEY) != 0) {
            //    // LOG("%s", name);
            //    savedcode=event.code;
            
            //    // fputs(name, logfile);
            // }
            
            if( strcmp(name,"j")==0){
               state=1;
               printf("%s , left pressed\n",name);

            }
            else if(strcmp(name,"k")==0){
               state=2;
               printf("%s right pressed\n",name);
            }
            else{

            }

         } else if (event.value == KEY_RELEASE) {
            state=0;
            if(isPressed==1){
               isPressed=0;

               char *fetchname= getKeyText(event.code, shift_pressed);
               

               if(strcmp(fetchname,"j")==0){
                  printf("left released\n");
               }
               else if(strcmp(fetchname,"k")==0){
                  printf("right released\n");
               }
               else{
                  printf("%s key released\n",fetchname);
               }


               
            }

            if (isShift(event.code)) {
               shift_pressed--;
            }
         }
      }
      assert(shift_pressed >= 0 && shift_pressed <= 2);
   }

   // Config_cleanup(&config);
   // fclose(logfile);
   close(kbd_fd);
   return 0;
}