all:
	cd seggetd && make
	cd request && make
	cd tuiclient && make

install:
	cd seggetd && make install
	cd request && make install
	cd tuiclient && make install

clean:
	cd seggetd && make clean
	cd request && make clean
	cd tuiclient && make clean
