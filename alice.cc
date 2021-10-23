#include <gperftools/profiler.h>
#include "lock/sem.h"
#include "threadPool.h"
#include "lock/mtx.h"
#include <malloc.h>



/* --------------------------------------不得修改两条分割线之间的内容--------------------------------------
 */

/*
示例的测试数据（最终测试时会更换），对于每一组测试数据
第一个数字表示要求发送消息的时间，保证严格递增
第二个数字表示所发送消息的长度，保证出现在MESSAGE_SIZES中
*/




deque<pair<time_t, int>> generate() {
  deque<pair<time_t, int>> c = {
      // // 间隔1s发送
      // {1 * SECOND_TO_NANO, MESSAGE_SIZES[0]},
      // {2 * SECOND_TO_NANO, MESSAGE_SIZES[1]},
      // {3 * SECOND_TO_NANO, MESSAGE_SIZES[2]},
      // {4 * SECOND_TO_NANO, MESSAGE_SIZES[3]},
      // {5 * SECOND_TO_NANO, MESSAGE_SIZES[4]},
      // // 间隔100mss发送
      // {5 * SECOND_TO_NANO + 100 * MILLI_TO_NANO, MESSAGE_SIZES[4]}, // 5.1s
      // {5 * SECOND_TO_NANO + 200 * MILLI_TO_NANO, MESSAGE_SIZES[3]},
      // {5 * SECOND_TO_NANO + 300 * MILLI_TO_NANO, MESSAGE_SIZES[2]},
      // {5 * SECOND_TO_NANO + 400 * MILLI_TO_NANO, MESSAGE_SIZES[1]},
      // {5 * SECOND_TO_NANO + 500 * MILLI_TO_NANO, MESSAGE_SIZES[0]}, // 5.5
      // // 间隔10ms发送
      // {5 * SECOND_TO_NANO + 510 * MILLI_TO_NANO, MESSAGE_SIZES[0]}, // 5.51
      // {5 * SECOND_TO_NANO + 520 * MILLI_TO_NANO, MESSAGE_SIZES[1]},
      // {5 * SECOND_TO_NANO + 530 * MILLI_TO_NANO, MESSAGE_SIZES[2]},
      // {5 * SECOND_TO_NANO + 540 * MILLI_TO_NANO, MESSAGE_SIZES[3]},
      // {5 * SECOND_TO_NANO + 550 * MILLI_TO_NANO, MESSAGE_SIZES[4]}, // 5.55
      // // 间隔1ms发送
      // {5 * SECOND_TO_NANO + 551 * MILLI_TO_NANO, MESSAGE_SIZES[4]}, // 5.551
      // {5 * SECOND_TO_NANO + 552 * MILLI_TO_NANO, MESSAGE_SIZES[3]},
      // {5 * SECOND_TO_NANO + 553 * MILLI_TO_NANO, MESSAGE_SIZES[2]},
      // {5 * SECOND_TO_NANO + 554 * MILLI_TO_NANO, MESSAGE_SIZES[1]},
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO, MESSAGE_SIZES[0]}, // 5.555
      // // 间隔100us发送
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 100 * MICRO_TO_NANO,       MESSAGE_SIZES[0]}, // 5.5551
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 200 * MICRO_TO_NANO,       MESSAGE_SIZES[1]},
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 300 * MICRO_TO_NANO,       MESSAGE_SIZES[2]},
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 400 * MICRO_TO_NANO,       MESSAGE_SIZES[3]},
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 500 * MICRO_TO_NANO,       MESSAGE_SIZES[4]}, // 5.5555
      // // 间隔10us发送
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 510 * MICRO_TO_NANO,       MESSAGE_SIZES[4]},//  5.55551
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 520 * MICRO_TO_NANO,       MESSAGE_SIZES[3]},
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 530 * MICRO_TO_NANO,       MESSAGE_SIZES[2]},
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 540 * MICRO_TO_NANO,       MESSAGE_SIZES[1]},
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 550 * MICRO_TO_NANO,       MESSAGE_SIZES[0]},//  5.55555
      // // 间隔1us发送
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 551 * MICRO_TO_NANO,       MESSAGE_SIZES[0]},//  5.555 551
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 552 * MICRO_TO_NANO,       MESSAGE_SIZES[1]},//  5.555 552
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 553 * MICRO_TO_NANO,       MESSAGE_SIZES[2]},//  5.555 553
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 554 * MICRO_TO_NANO,       MESSAGE_SIZES[3]},//  5.555 554
      // {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 555 * MICRO_TO_NANO,       MESSAGE_SIZES[4]},//  5.555 555




      {1 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 551 * MICRO_TO_NANO,       MESSAGE_SIZES[0]},//  5.555 551
      {1 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 552 * MICRO_TO_NANO,       MESSAGE_SIZES[1]},//  5.555 552
      {1 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 553 * MICRO_TO_NANO,       MESSAGE_SIZES[2]},//  5.555 553
      {1 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 554 * MICRO_TO_NANO,       MESSAGE_SIZES[3]},//  5.555 554
      {1 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 555 * MICRO_TO_NANO,       MESSAGE_SIZES[4]},//  5.555 555
  };
  int init_size = c.size();
  const int repeat = 0;  // 重复若干次
  for (int i = 1; i <= repeat; ++i)
    for (int j = 0; j < init_size; ++j)
      c.push_back(make_pair(c[j].first + 6 * SECOND_TO_NANO * i, c[j].second));
  // 检查时间递增
  for (int i = 1; i < c.size(); ++i) assert(c[i].first > c[i - 1].first);
  return c;
}

auto test_cases = generate();
size_t test_case_count = test_cases.size();
vector<time_t> delays;

// 记录延迟
void record(const Message *m) {
  assert(m->checksum == crc32(m));
  time_t t = now();
  delays.push_back((t - m->t) / 2);  // 来回的时间差除2近似地取作单程延迟，注意每轮通信会有四次校验和的计算，这些耗时也必须包含进延迟中
  cout << "delay.size()= " << delays.size() << endl;

//   printf("%ld %ld %ld\n", m->t, t, (t - m->t) / 2);
  
  static set<time_t> ts;

  assert(ts.find(m->t) == ts.end());  // 每个时间戳对应的消息只应被记录一次
  ts.insert(m->t);
  for (auto c : test_cases) assert(c.first != m->t);  // 被记录的消息必须是已经被取出的测试数据
}

void* coreActionAlice(void* p) {

    threadPool* thread_pool = (threadPool*)p;
    ShmQue* task_que = thread_pool->que;

    while (1) {
      pthread_mutex_lock(&task_que->mutex);
      pthread_cond_wait(&thread_pool->cond, &task_que->mutex); // wait !
      MessageWithSpinloc* msg = task_que->popFront(); // 
      int idx = calculateShmIdx(msg, task_que->shm_bufs);
      cout << "1= " <<  idx << endl;

      cout << "pop front task and send to bob, idx= " <<  idx << endl;
      
      pthread_mutex_unlock(&task_que->mutex);
      cout << "1= " <<  idx << endl;
      msg->setLockVal(1);
      while(msg->getLockVal() != 2) {}
      cout << "receie from bob, idx= " << idx << endl;

      record(msg->getMsg());
    }
    return nullptr;
}


threadPool ThreadPool(THREAD_POOL_SIZE, coreActionAlice);




// 获取下一条现在需要发送的消息并将其从test_cases中移除，当test_cases中没有满足时间要求的消息时返回NULL
MessageWithSpinloc *next_message() {
  // 所有测试用例均完成，打印统计结果并退出
  if (delays.size() == test_case_count) {
    sort(delays.begin(), delays.end());
    double median = delays[delays.size() / 2];
    double p95 = delays[delays.size() * 95 / 100];
    double p99 = delays[delays.size() * 99 / 100];
    double maximum = delays.back();
    double s = 0;
    for (auto d : delays) s += d;
    double mean = s / delays.size();
    double s2 = 0;
    for (auto d : delays) s2 += (d - mean) * (d - mean);
    double std = sqrt(s2 / delays.size());
    printf(
        "n=%zu median=%.0fns mean=%.0fns std=%.0fns 95%%=%.0fns 99%%=%.0fns "
        "maximum=%.0fns\n",
        delays.size(), median, mean, std, p95, p99, maximum);
    exit(0);
  }

  // 检查下一条消息是否已经到时间
  if (test_cases.empty()) return NULL; // 发送结束
  auto c = test_cases.front();
  if (c.first > now()) return NULL; // 还没到发送时间， 
  
  while(ThreadPool.que->isFull()) {}
  MessageWithSpinloc *msg_with_spinlock = ThreadPool.que->getTailNext(); // 8192
  Message *m = msg_with_spinlock->getMsg();
  // 构建消息头
  test_cases.pop_front();
  m->t = c.first;
  m->size = c.second;
  cout << "m->size= " <<  m->size << endl;

  // 随机生成消息体
  static unsigned int seed = 1;
  int *p = (int *)(m->payload);
  for (auto i = m->payload_size() / 4; i; --i) { // 32, 128, 512, 2048, 8192
    seed = (seed * 1103515245U + 12345U) & 0x7fffffffU;
    *p++ = (int)seed;
  }

  m->checksum = crc32(m);
  return msg_with_spinlock;
}



/* --------------------------------------不得修改两条分割线之间的内容--------------------------------------
 */


int main() {
  MessageWithSpinloc *m1 = nullptr;
  ThreadPool.Start();
  int idx = 0;
  while (true) {
    m1 = next_message(); 
    if (m1) {
      pthread_mutex_lock(&ThreadPool.que->mutex);
      ThreadPool.que->addRear(1); // 队列size++;
      pthread_cond_signal(&ThreadPool.cond); 
      cout << "call thread to  signal！！！！！！" << endl; // 阻塞住
      pthread_mutex_unlock(&ThreadPool.que->mutex);
    } else {
      time_t dt = now() - test_cases.front().first;
      // cout << now() << " " << test_cases.front().first << endl;
      timespec req = {dt / SECOND_TO_NANO, dt % SECOND_TO_NANO}, rem;
      nanosleep(&req, &rem);  // 等到整数秒时发送
    }
  }
  return 0;
}


