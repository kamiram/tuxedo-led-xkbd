all:
	gcc -O2 -std=c11 -o tuxedo-led-xkbd tuxedo-led-xkbd.c  -l X11
install:
	sudo cp tuxedo-led-xkbd /usr/local/bin

