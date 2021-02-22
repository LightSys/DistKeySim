all :
	mkdir -p build
	cd build && \
		cmake .. -DBUILD_TESTING=0 && \
		make -j6

clean :
	rm -rf build

