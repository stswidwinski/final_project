OBJDIRS += schedule/batchschedule

# Create template specific variables
UPPERC_DIR := SCHEDULE/BATCHSCHEDULE
LOWERC_DIR := schedule/batchschedule

$(UPPERC_DIR)_SRCS := schedule/batchschedule/batchschedule.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/txns/txn.o obj/containers/array_cont.o obj/schedule/lockstage/lockstage.o obj/schedule/locktable/locktable.o obj/schedule/lockqueue/lockqueue.o obj/schedule/locktable/readytxnqueue.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-schedule/batchschedule: $(SCHEDULE/BATCHSCHEDULE_TESTS)
	@for a in $(SCHEDULE/BATCHSCHEDULE_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
