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

# -----
# Build
# -----
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

# ------------------
# Test repeatability
# ------------------
#
# To test repeatability, run the following once:
#	make run-repeatable sanitize
#
# Then run the following as many times as you feel necessary to test repeatability:
#	make run-repeatable sanitize compare
#
# Sanitize replaces UUIDs in logOutput.txt and statslog.csv with node index
# thus making the files comparable.

OUTPUTS = $(BUILD)/$(SRC)/outputs
NON =

.PHONY: run-repeatable
run-repeatable :
	cp -p scenario1_$(NON)repeatable_config.json $(BUILD)/$(SRC)/config.json
	cd $(BUILD)/$(SRC) && ./adak

NEXT_RUN = $(shell cat $(OUTPUTS)/num.txt)
LAST_RUN = $(shell echo $(NEXT_RUN) - 1 | bc)
.PHONY: sanitize
sanitize :
	$(MAKE) sanitize-logOutput RUN=$(LAST_RUN)
	$(MAKE) sanitize-statsLog  RUN=$(LAST_RUN)

.PHONY: sanitize-statsLog
sanitize-statsLog :
	client/sanitizeStatsLog.py $(BUILD)/$(SRC)/outputs/statslog$(RUN).csv > \
		$(BUILD)/$(SRC)/outputs/statslog$(RUN).clean.txt

.PHONY: sanitize-logOutput
sanitize-logOutput :
	client/sanitizelogOutput.py $(BUILD)/$(SRC)/outputs/logOutput$(RUN).txt > \
		$(BUILD)/$(SRC)/outputs/logOutput$(RUN).clean.txt

.PHONY: compare
compare :
	$(MAKE) compare-logOutput
	$(MAKE) compare-statsLog

PREV_RUN = $(shell echo $(LAST_RUN) - 1 | bc)
.PHONY: compare-statsLog
compare-statsLog :
	cmp $(BUILD)/$(SRC)/outputs/statslog$(PREV_RUN).clean.txt \
		$(BUILD)/$(SRC)/outputs/statslog$(LAST_RUN).clean.txt

.PHONY: compare-logOutput
compare-logOutput :
	cmp $(BUILD)/$(SRC)/outputs/logOutput$(PREV_RUN).clean.txt \
		$(BUILD)/$(SRC)/outputs/logOutput$(LAST_RUN).clean.txt

# ----------------------
# Test non-repeatability
# ----------------------
#
# To test non-repeatability, run the following at least once:
#	make run-non-repeatable sanitize
#
# Then run the following which should fail on the compare:
#	make run-non-repeatable sanitize compare
#
# You can use diff, sdiff, or vimdiff to see the differences.
# When viewing the differences, be sure to diff the *.clean.txt
# files so that you're not seeing just the UUID differences.

.PHONY: run-non
run-non-repeatable run-scenario1 :
	$(MAKE) run-repeatable NON=non

# ------------------
# Show Visualization
# ------------------
STATS_LOG    = ./statslog.csv
VIS_NUM      = 2
GRAPH_IS_LOG = True
.PHONY: show-vis
show-vis :
	$(CLIENT)/showVis.py $(VIS_NUM) $(GRAPH_IS_LOG) $(STATS_LOG)

# -----
# Clean
# -----
.PHONY: clean
clean :
	touch $(SRC)/$(MESSAGE).proto
	rm -rf build

.PHONY: clean-outputs
clean-outputs :
	rm -rf $(BUILD)/$(SRC)/outputs

# --------------------
# Show number of cores
# --------------------
.PHONY: cores
cores :
	@echo NUM_CORES=$(NUM_CORES)
	@echo USE_CORES=$(USE_CORES)
