#ifndef _TEST_SCHEDULE_H_
#define _TEST_SCHEDULE_H_

class TestSchedule : public Schedule {
  // tests that require access
  friend void ScheduleMergingIntoEmptyTest();
  friend void ScheduleMergingIntoExistingTest();
};

#endif
