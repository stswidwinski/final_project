OBJDIRS += schedule

# Create template specific variables
UPPERC_DIR := SCHEDULE
LOWERC_DIR := schedule

$(UPPERC_DIR)_SRCS := schedule/lockstage.cc schedule/lockqueue.cc schedule/locktable.cc schedule/schedule.cc schedule/batchschedule.cc schedule/schedule.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS :=

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-schedule: $(SCHEDULE_TESTS)
	@for a in $(SCHEDULE_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
