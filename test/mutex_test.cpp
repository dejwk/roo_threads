#include "roo_threads/mutex.h"

#include "gtest/gtest.h"

TEST(Mutex, SimpleTake) {
  roo::mutex mutex;
  mutex.lock();
  mutex.unlock();
}
TEST(Mutex, TryLockSuccess) {
  roo::mutex mutex;
  EXPECT_TRUE(mutex.try_lock());
  mutex.unlock();
}

TEST(Mutex, TryLockFailWhenLocked) {
  roo::mutex mutex;
  mutex.lock();
  EXPECT_FALSE(mutex.try_lock());
  mutex.unlock();
}

TEST(Mutex, LockUnlockMultipleTimes) {
  roo::mutex mutex;
  for (int i = 0; i < 5; ++i) {
    mutex.lock();
    mutex.unlock();
  }
}

TEST(Mutex, LockGuardLocksAndUnlocks) {
  roo::mutex mutex;
  {
    roo::lock_guard<roo::mutex> guard(mutex);
    EXPECT_FALSE(mutex.try_lock());  // Should be locked by guard.
  }
  EXPECT_TRUE(mutex.try_lock());  // Should be unlocked after guard destruction.
  mutex.unlock();
}
TEST(Mutex, UniqueLockLocksAndUnlocks) {
  roo::mutex mutex;
  {
    roo::unique_lock<roo::mutex> lock(mutex);
    EXPECT_FALSE(mutex.try_lock());  // Should be locked by unique_lock.
  }
  EXPECT_TRUE(
      mutex.try_lock());  // Should be unlocked after unique_lock destruction.
  mutex.unlock();
}

TEST(Mutex, UniqueLockTryLockSuccess) {
  roo::mutex mutex;
  roo::unique_lock<roo::mutex> lock(mutex, roo::try_to_lock);
  EXPECT_TRUE(lock.owns_lock());
  EXPECT_FALSE(mutex.try_lock());
  lock.unlock();
  EXPECT_TRUE(mutex.try_lock());
  mutex.unlock();
}

TEST(Mutex, UniqueLockTryLockFail) {
  roo::mutex mutex;
  mutex.lock();
  roo::unique_lock<roo::mutex> lock(mutex, roo::try_to_lock);
  EXPECT_FALSE(lock.owns_lock());
  mutex.unlock();
}

TEST(Mutex, UniqueLockDeferLock) {
  roo::mutex mutex;
  roo::unique_lock<roo::mutex> lock(mutex, roo::defer_lock);
  EXPECT_FALSE(lock.owns_lock());
  EXPECT_TRUE(mutex.try_lock());
  mutex.unlock();
  lock.lock();
  EXPECT_FALSE(mutex.try_lock());
  lock.unlock();
}

TEST(Mutex, UniqueLockRelease) {
  roo::mutex mutex;
  roo::unique_lock<roo::mutex> lock(mutex);
  EXPECT_TRUE(lock.owns_lock());
  roo::mutex* released = lock.release();
  EXPECT_FALSE(lock.owns_lock());
  // The mutex is still locked, manually unlock.
  released->unlock();
}
