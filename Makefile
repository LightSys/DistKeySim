ADAK_ROOT = $(shell pwd)
BUILD     = $(ADAK_ROOT)/build
SRC       = $(ADAK_ROOT)/src
INCLUDE   = $(ADAK_ROOT)/include
CLIENT    = $(ADAK_ROOT)/client
SOURCES   = $(INCLUDE)/*.h $(INCLUDE)/*.hpp $(SRC)/*.cc $(SRC)/*.cpp
OUTPUTS   = $(BUILD)/src/outputs
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
all: $(OUTPUTS)/adak

# Update message protobuf sources when message.proto changes
$(SRC)/message.pb.cc $(INCLUDE)/message.pb.h : $(SRC)/message.proto 
	cd $(SRC) && \
		protoc message.proto --cpp_out=. && \
		mv  message.pb.h $(INCLUDE)

# Build ADAK
$(OUTPUTS)/adak : $(SOURCES)
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

NON =

.PHONY: run-repeatable
run-repeatable :
	cp -p scenario1_$(NON)repeatable_config.json $(BUILD)/src/config.json
	cd $(BUILD)/src && ./adak
	cd $(ADAK_ROOT)

NEXT_RUN = $(shell cat $(OUTPUTS)/num.txt)
LAST_RUN = $(shell echo $(NEXT_RUN) - 1 | bc)
.PHONY: sanitize
sanitize :
	$(MAKE) sanitize-logOutput RUN=$(LAST_RUN)
	$(MAKE) sanitize-statsLog  RUN=$(LAST_RUN)

.PHONY: sanitize-statsLog
sanitize-statsLog :
	$(ADAK_ROOT)/client/sanitizeStatsLog.py $(OUTPUTS)/statslog$(RUN).csv > \
		$(OUTPUTS)/statslog$(RUN).clean.txt

.PHONY: sanitize-logOutput
sanitize-logOutput :
	$(ADAK_ROOT)/client/sanitizeLogOutput.py $(OUTPUTS)/logOutput$(RUN).txt > \
		$(OUTPUTS)/logOutput$(RUN).clean.txt

.PHONY: compare
compare :
	$(MAKE) compare-logOutput
	$(MAKE) compare-statsLog

PREV_RUN = $(shell echo $(LAST_RUN) - 1 | bc)
.PHONY: compare-statsLog
compare-statsLog :
	$(CMP) $(OUTPUTS)/statslog$(PREV_RUN).clean.txt \
		       $(OUTPUTS)/statslog$(LAST_RUN).clean.txt

.PHONY: compare-logOutput
compare-logOutput :
	$(CMP) $(OUTPUTS)/logOutput$(PREV_RUN).clean.txt \
		       $(OUTPUTS)/logOutput$(LAST_RUN).clean.txt

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
run-non-repeatable :
	$(MAKE) run-repeatable NON=non

# ------------------------
# Test short repeatability
# ------------------------
#
# Shorten the repeatability test to see if we can get showVis1 to finish.

.PHONY: run-short-repeatable
run-short-repeatable :
	cp -p scenario1_$(NON)repeatable_config_short.json $(BUILD)/src/config.json
	cd $(BUILD)/src && ./adak
	cd $(ADAK_ROOT)


# --------------------------------------------
# Test Scenario 1 (see "ADAK Scenarios 1.pdf")
# --------------------------------------------
#
# Objective: Ensure the functionality of sharing keyspace blocks and
# the stability of subblock sharing (there should be minimal,
# if any, subblock sharing in this scenario)

.PHONY: run-scenario1
run-scenario1 :
	cp -p scenario1-config.json $(BUILD)/src/config.json
	cd $(BUILD)/src && ./adak
	cd $(ADAK_ROOT)

.PHONY: run-short-scenario1
run-short-scenario1 :
	jq ".simLength |= 10000" < scenario1-config.json > $(BUILD)/src/config.json
	cd $(BUILD)/src && ./adak
	cd $(ADAK_ROOT)

.PHONY: keyspaces
keyspaces :
	awk '/KEYSPACES/,/END KEYSPACES/ {print} /Time Step:/ {print}' \
		$(OUTPUTS)/logOutput$(LAST_RUN).clean.txt > \
		$(OUTPUTS)/logOutput$(LAST_RUN).keyspaces.txt

.PHONY: run-default-config
run-default-config :
	cp -p default-config.json $(BUILD)/src/config.json
	cd $(BUILD)/src && ./adak
	cd $(ADAK_ROOT)

# -------------------------
# Test eventGen config file
# -------------------------
CONNECTION_MODE = single
SIM_LENGTH = 50
NUM_NODES = 10
.PHONY: run-eventGen
run-eventGen :
	jq ".connectionMode |= \"$(CONNECTION_MODE)\"" < eventGen-config.json | \
		jq ".simLength |= $(SIM_LENGTH)" | \
		jq ".numNodes |= $(NUM_NODES)" > $(BUILD)/src/config.json
	cd $(BUILD)/src && ./adak
	cd $(ADAK_ROOT)

# ----------------
# Find oscillation
# ----------------
.PHONY: run-oscillation
run-oscillation :
	$(MAKE) run-eventGen CONNECTION_MODE=full   SIM_LENGTH=50     NUM_NODES=2  sanitize
	$(MAKE) run-eventGen CONNECTION_MODE=full   SIM_LENGTH=500    NUM_NODES=2  sanitize
	$(MAKE) run-eventGen CONNECTION_MODE=full   SIM_LENGTH=5000   NUM_NODES=2  sanitize
	$(MAKE) run-eventGen CONNECTION_MODE=full   SIM_LENGTH=50000  NUM_NODES=2  sanitize
	$(MAKE) run-eventGen CONNECTION_MODE=single SIM_LENGTH=50     NUM_NODES=10 sanitize
	$(MAKE) run-eventGen CONNECTION_MODE=single SIM_LENGTH=500    NUM_NODES=10 sanitize
	$(MAKE) run-eventGen CONNECTION_MODE=single SIM_LENGTH=5000   NUM_NODES=10 sanitize
	$(MAKE) run-eventGen CONNECTION_MODE=single SIM_LENGTH=50000  NUM_NODES=10 sanitize
	$(MAKE) run-eventGen CONNECTION_MODE=full   SIM_LENGTH=50     NUM_NODES=10 sanitize
	$(MAKE) run-eventGen CONNECTION_MODE=full   SIM_LENGTH=500    NUM_NODES=10 sanitize
	$(MAKE) run-eventGen CONNECTION_MODE=full   SIM_LENGTH=5000   NUM_NODES=10 sanitize
	$(MAKE) run-eventGen CONNECTION_MODE=full   SIM_LENGTH=50000  NUM_NODES=10 sanitize

# ------------------
# Show Visualization
# ------------------
STATS_LOG    = $(OUTPUTS)/statslog$(RUN).csv
GRAPH_IS_LOG = True
.PHONY: show-vis1
show-vis1 :
	$(CLIENT)/showVis.py 1 $(GRAPH_IS_LOG) $(STATS_LOG)

.PHONY: show-vis2
show-vis2 :
	$(CLIENT)/showVis.py 2 $(GRAPH_IS_LOG) $(STATS_LOG)

# -----------------------
# Generate class diagrams
# -----------------------
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

.PHONY: clean-outputs
clean-outputs :
	rm -rf $(OUTPUTS)
	rm -f  $(BUILD)/src/*.{csv,txt,json}

.PHONY: clean-last-output
clean-last-output :
	rm -f $(OUTPUTS)/statslog$(LAST_RUN).* \
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
