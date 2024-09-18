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
	which protoc
	protoc --version
	cd $(SRC) && protoc message.proto --cpp_out=.
	cd $(SRC) && mv message.pb.h $(INCLUDE)

# Build ADAK
$(BUILD_SRC)/adak : CMakeLists.txt src/CMakeLists.txt $(SOURCES)
	mkdir -p $(BUILD)
	cd $(BUILD) && cmake .. -DBUILD_TESTING=0
	cd $(BUILD) && make -j$(USE_CORES)

.PHONY: src
src :
	@echo $(SOURCES)

protobuf : FORCE
	git clone https://github.com/protocolbuffers/protobuf.git
	cd protobuf && \
    git submodule update --init --recursive && \
	cmake . -DCMAKE_CXX_STANDARD=14 && \
	cmake --build .

install-protobuf :
	cd protobuf && make install

clean-protobuf :
	rm -rf protobuf

# ---------------------------------------------------------
# Abseil is not already installed on some versions of Linux
# See https://abseil.io/docs/cpp/quickstart-cmake
# ---------------------------------------------------------
abseil : FORCE
	git clone https://github.com/abseil/abseil-cpp.git
	cd abseil-cpp && \
	mkdir build && \
	cd build && \
	cmake -DABSL_BUILD_TESTING=ON -DABSL_USE_GOOGLETEST_HEAD=ON -DCMAKE_CXX_STANDARD=14 .. && \
	cmake --build . --target all

clean-abseil : 
	rm -rf abseil-cpp

# ----------------------------------------------
# This is the most comprehensive automated
# test. It is the test we run in GitHub actions.
# ----------------------------------------------
#

set-verbose-false :
	sed -i '' 's/Verbose *= .*/Verbose = false;/g' include/Logger.h
	grep Verbose include/Logger.h

set-verbose-true :
	sed -i '' 's/Verbose *= .*/Verbose = true;/g' include/Logger.h
	grep Verbose include/Logger.h

.PHONY: build-and-test
build-and-test : all
	make run-test1-repeatability
	make run-test2-oscillation
	make run-test3-non-repeatability
	make run-test4-scenario-1
	make run-test5-doNothing

test6 :
	# Test 6 fails as of 8/18
	make run-test6-scenario-2

test7 :
	# Test 7 fails as of 8/18
	make run-test7-scenario-3

test8 :
	# Probably fails too
	make run-test8-scenario-4

next-short : all
	-make run-test6-scenario-2 SCEN_2_DAYS=0.01
	make sanitize jsonify

NEXT_RUN = $(shell if [ -e "$(OUTPUTS)/num.txt" ]; then cat "$(OUTPUTS)/num.txt"; else echo 1; fi)
LAST_RUN = $(shell echo $(NEXT_RUN) - 1 | bc)
.PHONY: sanitize
sanitize :
	make sanitize-logOutput RUN=$(LAST_RUN)
	make sanitize-statsLog  RUN=$(LAST_RUN)

.PHONY: sanitize-statsLog
sanitize-statsLog :
	$(ADAK_ROOT)/bin/sanitizeStatsLog.py $(OUTPUTS)/statsLog$(RUN).csv > \
		$(OUTPUTS)/statsLog$(RUN).clean.txt

.PHONY: sanitize-logOutput
sanitize-logOutput :
	$(ADAK_ROOT)/bin/sanitizeLogOutput.py $(OUTPUTS)/logOutput$(RUN).txt > \
		$(OUTPUTS)/logOutput$(RUN).clean.txt
$(OUTPUTS)/logOutput$(RUN).clean.txt : sanitize-logOutput

.PHONY: compare
compare :
	make compare-logOutput
	make compare-statsLog

PREV_RUN = $(shell echo $(LAST_RUN) - 1 | bc)
.PHONY: compare-statsLog
compare-statsLog :
	$(CMP) $(OUTPUTS)/statsLog$(PREV_RUN).clean.txt \
		       $(OUTPUTS)/statsLog$(LAST_RUN).clean.txt

.PHONY: compare-logOutput
compare-logOutput :
	$(CMP) $(OUTPUTS)/logOutput$(PREV_RUN).clean.txt \
		       $(OUTPUTS)/logOutput$(LAST_RUN).clean.txt

.PHONY: jsonify
jsonify : $(OUTPUTS)/logOutput$(LAST_RUN).clean.txt
	$(ADAK_ROOT)/bin/getJsonMsgs.py < $(OUTPUTS)/logOutput$(LAST_RUN).clean.txt > \
		$(OUTPUTS)/logOutput$(LAST_RUN).jsonify.txt

.PHONY: jsonify-all
jsonify-all :
	for file in `ls $(OUTPUTS)/logOutput*.txt | egrep -v '(clean|jsonify).txt'` ; do \
		num=`echo $$file | sed -e s/[^0-9]//g` ; \
		clean="$(OUTPUTS)/logOutput$$num.clean.txt" ; \
		[ ! -f "$$clean" ] && make sanitize-logOutput RUN=$$num ; \
		jsonify="$(OUTPUTS)/logOutput$$num.jsonify.txt" ; \
		[ ! -f "$$jsonify" ] && make jsonify LAST_RUN=$$num ; \
	done

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
	cd $(BUILD_SRC) && time ./adak
	cd $(ADAK_ROOT)

.PHONY: run-test1-repeatability
run-test1-repeatability : all
	@echo "------"
	@echo "Test 1"
	@echo "------"
	make run-repeatable sanitize
	make run-repeatable sanitize
	make compare
	@echo "Test 1 Passed: Repeatability"

# -----------------------------
# Assert absence of oscillation
# -----------------------------
.PHONY: run-test2-oscillation
run-test2-oscillation : all
	@echo "------"
	@echo "Test 2"
	@echo "------"
	time $(BIN)/testOscillation.py
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
	make run-repeatable NON=non

.PHONY: run-test3-non-repeatability
run-test3-non-repeatability : all
	@echo "------"
	@echo "Test 3"
	@echo "------"
	make run-non-repeatable sanitize
	make run-non-repeatable sanitize
	@make compare; \
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
# To run a test shorter than 1 day, do something like this
#
#     make run-test4-scenario-1 SCEN_1_DAYS=0.1

SCEN_1_DAYS   = 1
SCEN_1_CONFIG = "config/scenario1-config.json"

.PHONY: run-test4-scenario-1
run-test4-scenario-1 : all
	@echo "------"
	@echo "Test 4"
	@echo "------"
	time $(BIN)/testScenario.py --scenarioNum 1 --numNodes 2 --days $(SCEN_1_DAYS) --config $(SCEN_1_CONFIG) \
		-a 'assert numKeyspaces == 2, "Test Scenario 1 failed: numKeyspaces=%d" % numKeyspaces'
	@echo "Test 4 Passed: Scenario 1"

# We put this test here because it uses Scenario 1 config

.PHONY: run-test5-doNothing
run-test5-doNothing : all
	@echo "------"
	@echo "Test 5"
	@echo "------"
	time $(BIN)/testScenario.py --scenarioNum 1 --numNodes 2 --days $(SCEN_1_DAYS) \
		--config "config/doNothing-config.json" \
		-a 'assert numKeyspaces == 1, "Test Do Nothing failed: numKeyspaces=%d" % numKeyspaces'
	@echo "Test 5 Passed: Do Nothing Strategy"

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
# To run a test shorter than 7 days, do something like this
#
#     make run-test6-scenario-2 SCEN_2_DAYS=0.1
#
# Note: Full 7 day run of scenario 2 takes 70GB so let's not do that on GitHub
# Actions. We know that this test expands to three keyspaces very early and gets
# no more than that even at the end of 7 day test.
# QED: 7 days is not needed to verify success

SCEN_2_DAYS   = 1
SCEN_2_CONFIG = "config/scenario2-config.json"

.PHONY: run-test6-scenario-2
run-test6-scenario-2 : all
	@echo "------"
	@echo "Test 6"
	@echo "------"
	time $(BIN)/testScenario.py --scenarioNum 2 --numNodes 2 --days $(SCEN_2_DAYS) --config $(SCEN_2_CONFIG) \
		-a 'assert numKeyspaces > 2, "Test Scenario 2 failed: numKeyspaces=%d" % numKeyspaces'
	@echo "Test 6 Passed: Scenario 2"

.PHONY: run-test6-scenario-2a
run-test6-scenario-2a : all
	make run-test6-scenario-2 SCEN_2_DAYS=0.1 SCEN_2_CONFIG="config/scenario2a-config.json"
	#make jsonify-logOutput

.PHONY: run-test6-scenario-2b
run-test6-scenario-2b : all
	make run-test6-scenario-2 SCEN_2_DAYS=0.1 SCEN_2_CONFIG="config/scenario2b-config.json"

# --------------------------------------------
# Test Scenario 3 (see "ADAK Scenarios 1.pdf")
# --------------------------------------------
#
# Objective: Ensure block sharing eventually stabilizes, and determine what resolution
# the keyspace is broken up into (eighths, 17ths, 32nds) by ADAK in an attempt to distribute
# the keyspace according to object creation rate.
#
# To run a test shorter than 7 days, do something like this
#
#     make run-test7-scenario-3 SCEN_3_DAYS=0.1

SCEN_3_DAYS   = 7
SCEN_3_CONFIG = "config/scenario3-config.json"

.PHONY: run-test7-scenario-3
run-test7-scenario-3 : all
	@echo "------"
	@echo "Test 7"
	@echo "------"
	time $(BIN)/testScenario.py --scenarioNum 3 --numNodes 2 --days $(SCEN_3_DAYS) --config $(SCEN_3_CONFIG) \
		-a 'assert numKeyspaces == 32, "Test Scenario 3 failed: numKeyspaces=%d" % numKeyspaces'
	@echo "Test 7 Passed: Scenario 3"

# ---------------
# Test Scenario 4
# ---------------
#
# Objective: Ensure block sharing eventually stabilizes, and determine what resolution
# the keyspace is broken up into (eighths, 17ths, 32nds) by ADAK in an attempt to distribute
# the keyspace according to object creation rate.
#
# To run a test shorter than 7 days, do something like this
#
#     make run-test7-scenario-4 SCEN_4_DAYS=0.1

SCEN_4_DAYS  = 7
SCEN_4_CONFIG  = "config/scenario4-config.json"

.PHONY: run-test8-scenario-4
run-test8-scenario-4 : all
	@echo "------"
	@echo "Test 7"
	@echo "------"
	time $(BIN)/testScenario.py --scenarioNum 4 --numNodes 2 --days $(SCEN_4_DAYS) --config $(SCEN_4_CONFIG) \
		-a 'assert numKeyspaces == 32, "Test Scenario 4 failed: numKeyspaces=%d" % numKeyspaces'
	@echo "Test 7 Passed: Scenario 4"

# ----------------------------------------------------
# Display stuff from log output files
# ----------------------------------------------------
LOGFILE = $(shell ls -tr $(OUTPUTS)/logOutput$(LAST_RUN).txt | tail -1)
CONSUMING = $(OUTPUTS)/logOutput$(LAST_RUN).consuming.txt

.PHONY: count-consuming-keys
count-consuming-keys :
	awk '/consuming/ {counts[$$1] = counts[$$1] + 1} END {for (i in counts) {print i, counts[i]}}' $(LOGFILE)

compare-consuming-keys :
	egrep '(CS::adak|  keyspace=|  peer=|will not share|  percent of global|consuming a key)' $(LOGFILE) > $(CONSUMING)

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
	cd $(BUILD_SRC) && time ./adak
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

# -------------------
# SSH to LightSys VMs
# -------------------
ssh :
	ssh devel@cos.lightsys.org -p 18522

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

.PHONY: clean-all
clean-all : clean clean-outputs

# --------------------
# Show number of cores
# --------------------
.PHONY: cores
cores :
	@echo NUM_CORES=$(NUM_CORES)
	@echo USE_CORES=$(USE_CORES)

FORCE :