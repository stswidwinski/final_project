#ifndef _TEST_BATCH_SCHEDULE_H_
#define _TEST_BATCH_SCHEDULE_H_

#include "schedule/locktable/testlocktable.h"
#include "schedule/batchschedule/batchschedule.h"

class TestBatchSchedule : public BatchSchedule {
public:
  TestBatchSchedule(): BatchSchedule() {}
  TestBatchSchedule(
      std::unique_ptr<std::vector<std::unique_ptr<Txn>>> batch)
  : BatchSchedule(std::move(batch)) {}

  static std::unique_ptr<TestBatchSchedule> build_batch_schedule(
      std::unique_ptr<std::vector<std::unique_ptr<Txn>>> batch) {
    return std::make_unique<TestBatchSchedule>(std::move(batch));
  }

  // tests that need access
  friend void ScheduleMergingIntoEmptyTest();
  friend void BatchScheduleInsert();
  friend void BatchScheduleCreationTest1();
  friend void BatchScheduleCreationTest2();
  friend void ScheduleMergingIntoExistingTest();
};

#endif // _TEST_BATCH_SCHEDULE_H_
