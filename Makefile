BUILD     = build
SRC       = src
INCLUDE   = include
CLIENT    = client
MESSAGE   = message
SOURCES   = $(INCLUDE)/*.h $(INCLUDE)/*.hpp $(SRC)/*.cc $(SRC)/*.cpp
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

.PHONY: all
all: $(BUILD)/$(SRC)/adak

# Update message protobuf sources when message.proto changes
$(SRC)/$(MESSAGE).pb.cc $(INCLUDE)/$(MESSAGE).pb.h : $(SRC)/$(MESSAGE).proto 
	cd $(SRC) && \
		protoc message.proto --cpp_out=. && \
		mv  message.pb.h ../$(INCLUDE)

# Build ADAK
$(BUILD)/$(SRC)/adak : $(SOURCES)
	mkdir -p $(BUILD)
	cd $(BUILD) && \
		cmake .. -DBUILD_TESTING=0 && \
		make -j$(USE_CORES)

.PHONY: src
src :
	@echo $(SOURCES)

.PHONY: run-scenario
run-scenario :
	cp -p scenario$(SCENARIO)_repeatable_config.json $(BUILD)/$(SRC)/config.json
	cd $(BUILD)/$(SRC) && ./adak

.PHONY: run-scenario1
run-scenario1 :
	$(MAKE) run-scenario SCENARIO=1

STATS_LOG    = ./statslog.csv
VIS_NUM      = 2
GRAPH_IS_LOG = True
.PHONY: show-vis
show-vis :
	$(CLIENT)/showVis.py $(VIS_NUM) $(GRAPH_IS_LOG) $(STATS_LOG)

.PHONY: clean
clean :
	touch $(SRC)/$(MESSAGE).proto
	rm -rf build

.PHONY: clean-outputs
clean-outputs :
	rm -rf $(BUILD)/$(SRC)/outputs

.PHONY: cores
cores :
	@echo NUM_CORES=$(NUM_CORES)
	@echo USE_CORES=$(USE_CORES)
