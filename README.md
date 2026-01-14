# particle_life
I compile with this
```
gcc -o particleLife main.c `pkg-config --cflags --libs sdl3 sdl3-image` -lm
```
and my bashrc is configured with
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/user/SDL3/lib
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/user/SDL3/lib/pkgconfig
```
