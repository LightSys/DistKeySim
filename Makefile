BUILD     = build
BUILD_SRC = build/src
NUM_CORES = $(shell sysctl -n hw.ncpu)
USE_CORES = $(shell echo $(NUM_CORES) - 2 | bc)

all :
	mkdir -p $(BUILD)
	cd $(BUILD) && \
		cmake .. -DBUILD_TESTING=0 && \
		make -j$(USE_CORES)

run-scenario : all
	cp -p scenario$(SCENARIO)_config.json $(BUILD_SRC)/config.json
	cd $(BUILD_SRC) && ./adak

run-scenario1 :
	$(MAKE) run-scenario SCENARIO=1

clean :
	rm -rf build

cores :
	echo $(NUM_CORES)
	echo $(USE_CORES)

