## Build in WSL

1. Init submodule
```
git submodule update --init --recursive
```

2. Change directory to appllication folder:
```
mattpro@Laptop:/mnt/d/LocalRepo/imbue_light/imbuelight-stair-controller-firmware$ cd application/
mattpro@Laptop:/mnt/d/LocalRepo/imbue_light/imbuelight-stair-controller-firmware/application$ 
```

3. Make sure that env PICO_SDK_PATH is set

```
export PICO_SDK_PATH=../../pico-sdk
```

4. Make directory build 

```
mkdir build
```

5. Run cmake from build directory

```
cd build
cmake ..
```

Example from WSL terminal:
```
/mnt/d/LocalRepo/ImbueLight/imbuelight-stair-controller/firmware/imbuelight_stair_controller_fw/build$ cmake ..
```

6. Build project

```
make
```
