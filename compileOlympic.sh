gcc olympic.c phone.c -I/opt/vc/include/ -o olympic -lm -L/opt/vc/lib /usr/local/lib/libwiringPi.so /usr/local/lib/libsimple2d.a -lsimple2d -I/usr/local/include/SDL2 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lSDL2 -lm -I/opt/vc/include/ -L/opt/vc/lib -lbrcmGLESv2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
