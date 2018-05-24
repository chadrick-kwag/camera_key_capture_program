# camera and keyboard capture program

This repo holds the programs that are needed to periodically capture the image from webcam and which key is pressed. This need rises for my small face-tracking robot arm project.


the two key program in this directory is the `keyserver` and a test client program.

# `keyserver` program

if you change the input device, then please change the source code and recompile to update it.

## how to check the device file for the input

```
$ cat /proc/bus/input/devices
```

for my dell laptop alone: use 
francium deck keyboard: use `event6`

here is an example command for compiling.
```
$ gcc -pthread -o keyserver keyserver.c
```

when executing, please input which port to use as well. here is an example.
```
$ ./keyserver 9898
```

# `test client` program
this is literally a test client program written in python which serves as an example to anyone who wishes to use it in their own code.


# `mainlog2.py`
this is the latest prototype for data gathering code.

# `keyserver2` code
upgraded version from `keyserver`. Now it can specify the port number and eventnumber from commandline.
```
$ ./keyserver2 <portnumber> <eventnumber>
```

the port number used by the loggin program is 9898.

