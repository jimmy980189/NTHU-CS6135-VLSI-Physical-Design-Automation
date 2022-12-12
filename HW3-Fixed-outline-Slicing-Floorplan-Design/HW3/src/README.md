# Homework 3: 

## How to compile

In this directory, enter the following command:

```sh
$ make
```

It will generate the executable file "hw3" in "../bin/".

If you want to remove it, please enter the following command:

```sh
$ make clean
```

## How to Run

In this directory, enter the following command:

Usage: ../bin/[exe] [hardblock file] [net file] [position file] [floorplan file] [dead space ratio]

e.g.

```sh
$ ../bin/hw3 ../testcases/n100.hardblock ../testcases/n100.nets ../testcases/n100.pl ../output/n100.floorplan 0.15
```

In "HW2/bin/", enter the following command:

Usage: ./[exe] [hardblock file] [net file] [position file] [floorplan file] [dead space ratio]

e.g.

```sh
$ ./hw3 ../testcases/n100.hardblock ../testcases/n100.nets ../testcases/n100.pl ../output/n100.floorplan 0.15
```
