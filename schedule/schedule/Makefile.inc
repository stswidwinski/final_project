OBJDIRS += schedule/schedule

# Create template specific variables
UPPERC_DIR := SCHEDULE/SCHEDULE
LOWERC_DIR := schedule/schedule

$(UPPERC_DIR)_SRCS := schedule/schedule/schedule.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/txns/txn.o obj/containers/array_cont.o obj/schedule/lockqueue/lockqueue.o obj/schedule/batchschedule/batchschedule.o obj/schedule/locktable/locktable.o obj/schedule/locktable/readytxnqueue.o obj/schedule/lockstage/lockstage.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-schedule/schedule: $(SCHEDULE/SCHEDULE_TESTS)
	@for a in $(SCHEDULE/SCHEDULE_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
