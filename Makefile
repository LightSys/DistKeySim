ADAK_ROOT = $(shell pwd)
BUILD     = $(ADAK_ROOT)/build
SRC       = $(ADAK_ROOT)/src
INCLUDE   = $(ADAK_ROOT)/include
BIN       = $(ADAK_ROOT)/bin
BUILD_SRC = $(BUILD)/src
SOURCES   = $(INCLUDE)/*.h $(INCLUDE)/*.hpp $(SRC)/*.cc $(SRC)/*.cpp
OUTPUTS   = $(BUILD_SRC)/outputs
USE_CORES = 1
CMP       = cmp

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
all: $(BUILD_SRC)/adak

# Update message protobuf sources when message.proto changes
$(SRC)/message.pb.cc $(INCLUDE)/message.pb.h : $(SRC)/message.proto 
	cd $(SRC) && \
		protoc message.proto --cpp_out=. && \
		mv  message.pb.h $(INCLUDE)

# Build ADAK
$(BUILD_SRC)/adak : $(SOURCES)
	mkdir -p $(BUILD)
	cd $(BUILD) && \
		cmake .. -DBUILD_TESTING=0 && \
		$(MAKE) -j$(USE_CORES)

.PHONY: src
src :
	@echo $(SOURCES)

# ----------------------------------------------
# This is the most comprehensive automated
# test. It is the test we run in GitHub actions.
# ----------------------------------------------
.PHONY: build-and-test
build-and-test :
	$(MAKE) all
	$(MAKE) run-test1-repeatability
	$(MAKE) run-test2-oscillation
	$(MAKE) run-test3-non-repeatability
	$(MAKE) run-test4-scenario-1 SCEN_1_DAYS=$(SCEN_1_DAYS)

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
# Sanitize replaces UUIDs in logOutput.txt and statsLog.csv with node index
# thus making the files comparable.

NON =

.PHONY: run-repeatable
run-repeatable :
	cp -p config/$(NON)repeatable-config.json $(BUILD_SRC)/config.json
	cd $(BUILD_SRC) && ./adak
	cd $(ADAK_ROOT)

NEXT_RUN = $(shell cat $(OUTPUTS)/num.txt)
LAST_RUN = $(shell echo $(NEXT_RUN) - 1 | bc)
.PHONY: sanitize
sanitize :
	$(MAKE) sanitize-logOutput RUN=$(LAST_RUN)
	$(MAKE) sanitize-statsLog  RUN=$(LAST_RUN)

.PHONY: sanitize-statsLog
sanitize-statsLog :
	$(ADAK_ROOT)/bin/sanitizeStatsLog.py $(OUTPUTS)/statsLog$(RUN).csv > \
		$(OUTPUTS)/statsLog$(RUN).clean.txt

.PHONY: sanitize-logOutput
sanitize-logOutput :
	$(ADAK_ROOT)/bin/sanitizeLogOutput.py $(OUTPUTS)/logOutput$(RUN).txt > \
		$(OUTPUTS)/logOutput$(RUN).clean.txt

.PHONY: compare
compare :
	$(MAKE) compare-logOutput
	$(MAKE) compare-statsLog

PREV_RUN = $(shell echo $(LAST_RUN) - 1 | bc)
.PHONY: compare-statsLog
compare-statsLog :
	$(CMP) $(OUTPUTS)/statsLog$(PREV_RUN).clean.txt \
		       $(OUTPUTS)/statsLog$(LAST_RUN).clean.txt

.PHONY: compare-logOutput
compare-logOutput :
	$(CMP) $(OUTPUTS)/logOutput$(PREV_RUN).clean.txt \
		       $(OUTPUTS)/logOutput$(LAST_RUN).clean.txt

.PHONY: run-test1-repeatability
run-test1-repeatability :
	$(MAKE) run-repeatable sanitize 
	$(MAKE) run-repeatable sanitize 
	$(MAKE) compare
	@echo "Test 1 Passed: Repeatability"

# -----------------------------
# Assert absence of oscillation
# -----------------------------
.PHONY: run-test2-oscillation
run-test2-oscillation :
	$(BIN)/testOscillation.py
	@echo "Test 2 Passed: Oscillation"

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

.PHONY: run-non-repeatable
run-non-repeatable :
	$(MAKE) run-repeatable NON=non

.PHONY: run-test3-non-repeatability
run-test3-non-repeatability :
	$(MAKE) run-non-repeatable sanitize 
	$(MAKE) run-non-repeatable sanitize 
	@$(MAKE) compare; \
	if [ $$? -eq 0 ]; then \
        echo "Test 3 Failed: Non Repeatability"; \
        exit 1; \
    else \
        echo "Test 3 Passed: Non Repeatability"; \
    fi

# --------------------------------------------
# Test Scenario 1 (see "ADAK Scenarios 1.pdf")
# --------------------------------------------
#
# Objective: Ensure the functionality of sharing keyspace blocks and
# the stability of subblock sharing (there should be minimal,
# if any, subblock sharing in this scenario)
#
# Note: We make 40 milliseconds the length of a simulation "tick".
# Therefore, there are 25 simulation ticks per second.
#
# To run a test shorter than 1 day, do something like this
#
#     make run-test4-scenario-1 SCEN_1_DAYS=0.1

SIM_UNITS_PER_SECOND = 25
SECONDS      = 60
MINUTES      = 60
HOURS        = 24
SCEN_1_DAYS  = 1
ITERATIONS   = $(shell echo "$(SIM_UNITS_PER_SECOND)*$(SECONDS)*$(MINUTES)*$(HOURS)*$(SCEN_1_DAYS)" | bc)
.PHONY: run-scenario1
run-scenario1 :
	jq ".simLength |= $(ITERATIONS)" < config/scenario1-config.json > $(BUILD_SRC)/config.json
	cd $(BUILD_SRC) && ./adak
	cd $(ADAK_ROOT)

.PHONY: run-test4-scenario-1
run-test4-scenario-1 :
	$(BIN)/testScenario1.py --days $(SCEN_1_DAYS)
	@echo "Test 4 Passed: Scenario 1"

# --------------------------------------------
# Test Scenario 2 (see "ADAK Scenarios 1.pdf")
# --------------------------------------------
#
# Objective: There initially should be a lot of subblock sharing to fill in “holes”,
# but eventually block sharing should happen, reducing the volume of subblock sharing.
# Block sharing should eventually result in the higher-rate node having 7x the keyspace
# as the lower rate node. (these values were chosen so that the keyspace blocks will
# end up being divided up into eighths, with one node having 1/8 of the keyspace and
# the other having 7/8 of the keyspace)
#
# Note: We make 40 milliseconds the length of a simulation "tick".
# Therefore, there are 25 simulation ticks per second.
#
# To run a test shorter than 7 days, do something like this
#
#     make run-test5-scenario-2 SCEN_2_DAYS=0.1

SIM_UNITS_PER_SECOND = 25
SECONDS      = 60
MINUTES      = 60
HOURS        = 24
SCEN_2_DAYS  = 7
SCEN_2_ITERATIONS = $(shell echo "$(SIM_UNITS_PER_SECOND)*$(SECONDS)*$(MINUTES)*$(HOURS)*$(SCEN_2_DAYS)" | bc)
SCEN_2_CONFIG  = "config/scenario2-config.json"

.PHONY: run-scenario2
run-scenario2 :
	jq ".simLength |= $(SCEN_2_ITERATIONS)" < $(SCEN_2_CONFIG) > $(BUILD_SRC)/config.json
	cd $(BUILD_SRC) && ./adak
	cd $(ADAK_ROOT)

.PHONY: run-test5-scenario-2
run-test5-scenario-2 :
	time $(BIN)/testScenario2.py --days $(SCEN_2_DAYS) --config $(SCEN_2_CONFIG)
	@echo "Test 5 Passed: Scenario 2"

.PHONY: run-test5-scenario-2a
run-test5-scenario-2a :
	$(MAKE) run-test5-scenario-2 SCEN_2_DAYS=0.1 SCEN_2_CONFIG="config/scenario2a-config.json"
	$(MAKE) sanitize

.PHONY: run-test5-scenario-2b
run-test5-scenario-2b :
	$(MAKE) run-test5-scenario-2 SCEN_2_DAYS=0.1 SCEN_2_CONFIG="config/scenario2b-config.json"
	$(MAKE) sanitize

# --------------------------------------------
# Test Scenario 3 (see "ADAK Scenarios 1.pdf")
# --------------------------------------------
#
# Objective: Ensure block sharing eventually stabilizes, and determine what resolution
# the keyspace is broken up into (eighths, 16ths, 32nds) by ADAK in an attempt to distribute
# the keyspace according to object creation rate.
#
# Note: We make 40 milliseconds the length of a simulation "tick".
# Therefore, there are 25 simulation ticks per second.
#
# To run a test shorter than 7 days, do something like this
#
#     make run-test6-scenario-3 SCEN_2_DAYS=0.1

SCEN_3_DAYS  = 7
SCEN_3_ITERATIONS = $(shell echo "$(SIM_UNITS_PER_SECOND)*$(SECONDS)*$(MINUTES)*$(HOURS)*$(SCEN_2_DAYS)" | bc)
SCEN_3_CONFIG  = "config/scenario3-config.json"

.PHONY: run-scenario3
run-scenario3 :
	jq ".simLength |= $(SCEN_3_ITERATIONS)" < $(SCEN_3_CONFIG) > $(BUILD_SRC)/config.json
	cd $(BUILD_SRC) && ./adak
	cd $(ADAK_ROOT)

.PHONY: run-test6-scenario-3
run-test6-scenario-3 :
	time $(BIN)/testScenario3.py --days $(SCEN_3_DAYS) --config $(SCEN_3_CONFIG)
	@echo "Test 6 Passed: Scenario 3"

# ----------------------------------------------------
# Display stuff from log output files
# ----------------------------------------------------
LOGFILE = $(shell ls -tr $(OUTPUTS)/logOutput*.txt | tail -1)

.PHONY: count-consuming-keys
count-consuming-keys :
	awk '/consuming/ {counts[$$1] = counts[$$1] + 1} END {for (i in counts) {print i, counts[i]}}' $(LOGFILE)

short-allocation-ratio :
	@awk '/sourceNodeID/ {sourceNodeID = $$3} /shortAllocationRatio/ {print sourceNodeID, $$2}' $(LOGFILE)

alloc-csv :
	@tr -s ' ' < $(LOGFILE) | sed -e 's/ /=/g' | \
		awk -F= 'BEGIN {print "shortAlloc,shortAllocIsOne,longAlloc,longAllocIsOne"} /shortAlloc/ {printf "%g,%s,%g,%s\n", $$3, $$19, $$5, $$23}'

# ----------------------------------------------------
# Find what config files are actually used
# ----------------------------------------------------
.PHONY: find-config-files
find-config-files :
	@for file in config/*.json ; do \
    	basename=`basename $$file` ; \
    	echo --------------------------- $$basename ---------------------- ; \
    	grep -RI $$basename --exclude src/build/output . ; \
    	find . -name $$basename | grep -v config/ ; \
	done

# -------------------------
# Test eventGen config file
# -------------------------
CONNECTION_MODE = single
SIM_LENGTH = 50
NUM_NODES = 10
.PHONY: run-eventGen
run-eventGen :
	jq ".connectionMode |= \"$(CONNECTION_MODE)\"" < config/eventGen-config.json | \
		jq ".simLength |= $(SIM_LENGTH)" | \
		jq ".numNodes |= $(NUM_NODES)" > $(BUILD_SRC)/config.json
	cd $(BUILD_SRC) && ./adak
	cd $(ADAK_ROOT)

# ------------------
# Show Visualization
# ------------------
STATS_LOG    = $(OUTPUTS)/statsLog$(RUN).csv
GRAPH_IS_LOG = True
.PHONY: show-vis1
show-vis1 :
	$(BIN)/showVis.py 1 $(GRAPH_IS_LOG) $(STATS_LOG)

.PHONY: show-vis2
show-vis2 :
	$(BIN)/showVis.py 2 $(GRAPH_IS_LOG) $(STATS_LOG)

# -----------------------
# Generate class diagrams
# -----------------------
# Get hpp2plantuml with homebrew or pip install
# ---------------------------------------------
images/%.puml : include/%.h
	hpp2plantuml -i $<  -o $@

images/Config.puml : include/Config.hpp
	hpp2plantuml -i $<  -o $@

images/%.png : images/%.puml
	plantuml $<

images/Strategy.puml : include/ADAKStrategy.h include/ControlStrategy.h
	hpp2plantuml -i include/ADAKStrategy.h -i include/ControlStrategy.h -o $@

images/GeometricDisconnect.puml : include/EventGen.h include/GeometricDisconnect.h include/Random.h
	hpp2plantuml -i include/EventGen.h -i include/GeometricDisconnect.h -i include/Random.h -o $@

images/Network.puml : include/Network.h include/Simulation.h
	hpp2plantuml -i include/Network.h -i include/Simulation.h -o $@

images/Node.puml : include/Node.h include/NodeData.h
	hpp2plantuml -i include/Node.h -i include/NodeData.h -o $@

all-images : images/Logger.png \
	images/Strategy.png \
	images/picoSHA2.png \
	images/Channel.png \
	images/Config.png \
	images/EventGen.png \
	images/GeometricDisconnect.png \
	images/Keyspace.png \
	images/Network.png \
	images/Node.png \
	images/main-seq.png \
	images/Simulation-seq.png

# -----
# Clean
# -----
.PHONY: clean
clean :
	touch $(SRC)/message.proto
	rm -rf build
	cp /dev/null make.out

.PHONY: clean-outputs
clean-outputs :
	rm -rf $(OUTPUTS)
	rm -f  $(BUILD_SRC)/*.{csv,txt,json}

.PHONY: clean-last-output
clean-last-output :
	rm -f $(OUTPUTS)/statsLog$(LAST_RUN).* \
		$(OUTPUTS)/logOutput$(LAST_RUN).* > \
		$(OUTPUTS)/copy_of_config$(LAST_RUN).json \
		$(OUTPUTS)/full_config$(LAST_RUN).json
	echo $(LAST_RUN) > 	$(OUTPUTS)/num.txt

# --------------------
# Show number of cores
# --------------------
.PHONY: cores
cores :
	@echo NUM_CORES=$(NUM_CORES)
	@echo USE_CORES=$(USE_CORES)
