all:
	cd segget && make
	cd request && make
	cd tuiclient && make

install:
	cd segget && make install
	cd request && make install
	cd tuiclient && make install

clean:
	cd segget && make clean
	cd request && make clean
	cd tuiclient && make clean
