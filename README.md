# camera and keyboard capture program

This repo holds the programs that are needed to periodically capture the image from webcam and which key is pressed. This need rises for my small face-tracking robot arm project.


the two key program in this directory is the `keyserver` and a test client program.

## `keyserver` program

The `keyserver` executable can be built from `keyserver.c` file. The below command works in linux environment.
```
$ gcc -pthread -o keyserver keyserver.c
```

### Usage
Once the executable has been built, it is executed with the following cmd format
```
$ ./keyserver <portnum> <inputeventnumber>
```
- make sure that the portnum used here is a fresh one
- the `inputeventnumber` is the event number of the keyboard device that the user will use.

## how to check the device input event number for the input

```
$ cat /proc/bus/input/devices
```

From the list of outputs, locate the keyboard device that you are using.


## `mainlogging.py` program
This python code is the main logging program, which gathers both image data(from webcam) and keyboard input(from `keyserver` program) and pairs them and then stores it.
The image data will be saved in `images` directory and the captured key data will be saved in `keycapture` directory.

However, it is still in development and currently only the captured image and key will only be printed.
