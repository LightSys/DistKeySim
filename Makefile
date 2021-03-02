BUILD     = build
SRC       = src
INCLUDE   = include
CLIENT    = client
MESSAGE   = message
USE_CORES = 1

# Figure out how many cores we can safely use
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
NUM_CORES = $(shell nproc)
USE_CORES = $(shell echo $(NUM_CORES) - 2 | bc)
endif
ifeq ($(UNAME), Darwin)
NUM_CORES = $(shell sysctl -n hw.ncpu)
USE_CORES = $(shell echo $(NUM_CORES) - 2 | bc)
endif

all : $(BUILD)/$(SRC)/adak

# Update message protobuf sources when message.proto changes
$(SRC)/$(MESSAGE).pb.cc $(INCLUDE)/$(MESSAGE).pb.h : $(SRC)/$(MESSAGE).proto 
	cd $(SRC) && \
		protoc message.proto --cpp_out=. && \
		mv  message.pb.h ../$(INCLUDE)

# Build ADAK
$(BUILD)/$(SRC)/adak : $(INCLUDE)/*.h $(INCLUDE)/*.hpp $(SRC)/*.cc $(SRC)/*.cpp  
	mkdir -p $(BUILD)
	cd $(BUILD) && \
		cmake -j$(USE_CORES) .. -DBUILD_TESTING=0 && \
		make -j$(USE_CORES)

run-scenario :
	cp -p scenario$(SCENARIO)_config.json $(BUILD)/$(SRC)/config.json
	cd $(BUILD)/$(SRC) && ./adak

run-scenario1 :
	$(MAKE) run-scenario SCENARIO=1

STATS_LOG    = ./statslog.csv
VIS_NUM      = 2
GRAPH_IS_LOG = True
show-vis :
	$(CLIENT)/showVis.py $(VIS_NUM) $(GRAPH_IS_LOG) $(STATS_LOG)

clean :
	rm -rf build

cores :
	echo $(NUM_CORES)
	echo $(USE_CORES)
