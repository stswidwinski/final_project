OBJDIRS += simulation

# Create template specific variables
UPPERC_DIR := SIMULATION
LOWERC_DIR := simulation

$(UPPERC_DIR)_SRCS := simulation/txn_generator.cc simulation/txn_map.cc simulation/sim_args_parser.cc simulation/schedule_snooper.cc simulation/simulation_test.cc simulation/model.cc simulation/data.cc simulation/workload.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/txns/txn.o obj/schedule/schedule/schedule.o obj/schedule/batchschedule/batchschedule.o obj/schedule/locktable/locktable.o obj/schedule/lockqueue/lockqueue.o obj/schedule/locktable/readytxnqueue.o obj/schedule/lockstage/lockstage.o obj/containers/array_cont.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-simulation: $(SIMULATION_TESTS)
	@for a in $(SIMULATION_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
 
