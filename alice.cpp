#include "common.h"
#include <gperftools/profiler.h>
#include "lock/sem.h"
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
      // 间隔1s发送
      {1 * SECOND_TO_NANO, MESSAGE_SIZES[0]},
      {2 * SECOND_TO_NANO, MESSAGE_SIZES[1]},
      {3 * SECOND_TO_NANO, MESSAGE_SIZES[2]},
      {4 * SECOND_TO_NANO, MESSAGE_SIZES[3]},
      {5 * SECOND_TO_NANO, MESSAGE_SIZES[4]},
      // 间隔100mss发送
      {5 * SECOND_TO_NANO + 100 * MILLI_TO_NANO, MESSAGE_SIZES[4]},
      {5 * SECOND_TO_NANO + 200 * MILLI_TO_NANO, MESSAGE_SIZES[3]},
      {5 * SECOND_TO_NANO + 300 * MILLI_TO_NANO, MESSAGE_SIZES[2]},
      {5 * SECOND_TO_NANO + 400 * MILLI_TO_NANO, MESSAGE_SIZES[1]},
      {5 * SECOND_TO_NANO + 500 * MILLI_TO_NANO, MESSAGE_SIZES[0]},
      // 间隔10ms发送
      {5 * SECOND_TO_NANO + 510 * MILLI_TO_NANO, MESSAGE_SIZES[0]},
      {5 * SECOND_TO_NANO + 520 * MILLI_TO_NANO, MESSAGE_SIZES[1]},
      {5 * SECOND_TO_NANO + 530 * MILLI_TO_NANO, MESSAGE_SIZES[2]},
      {5 * SECOND_TO_NANO + 540 * MILLI_TO_NANO, MESSAGE_SIZES[3]},
      {5 * SECOND_TO_NANO + 550 * MILLI_TO_NANO, MESSAGE_SIZES[4]},
      // 间隔1ms发送
      {5 * SECOND_TO_NANO + 551 * MILLI_TO_NANO, MESSAGE_SIZES[4]},
      {5 * SECOND_TO_NANO + 552 * MILLI_TO_NANO, MESSAGE_SIZES[3]},
      {5 * SECOND_TO_NANO + 553 * MILLI_TO_NANO, MESSAGE_SIZES[2]},
      {5 * SECOND_TO_NANO + 554 * MILLI_TO_NANO, MESSAGE_SIZES[1]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO, MESSAGE_SIZES[0]},
      // 间隔100us发送
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 100 * MICRO_TO_NANO,       MESSAGE_SIZES[0]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 200 * MICRO_TO_NANO,       MESSAGE_SIZES[1]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 300 * MICRO_TO_NANO,       MESSAGE_SIZES[2]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 400 * MICRO_TO_NANO,       MESSAGE_SIZES[3]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 500 * MICRO_TO_NANO,       MESSAGE_SIZES[4]},
      // 间隔10us发送
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 510 * MICRO_TO_NANO,       MESSAGE_SIZES[4]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 520 * MICRO_TO_NANO,       MESSAGE_SIZES[3]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 530 * MICRO_TO_NANO,       MESSAGE_SIZES[2]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 540 * MICRO_TO_NANO,       MESSAGE_SIZES[1]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 550 * MICRO_TO_NANO,       MESSAGE_SIZES[0]},
      // 间隔1us发送
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 551 * MICRO_TO_NANO,       MESSAGE_SIZES[0]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 552 * MICRO_TO_NANO,       MESSAGE_SIZES[1]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 553 * MICRO_TO_NANO,       MESSAGE_SIZES[2]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 554 * MICRO_TO_NANO,       MESSAGE_SIZES[3]},
      {5 * SECOND_TO_NANO + 555 * MILLI_TO_NANO + 555 * MICRO_TO_NANO,       MESSAGE_SIZES[4]},
  };
  int init_size = c.size();
  const int repeat = 10;  // 重复若干次
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

// 获取下一条现在需要发送的消息并将其从test_cases中移除，当test_cases中没有满足时间要求的消息时返回NULL
const Message *next_message() {
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
  if (c.first > now()) return NULL; // 还没到发送时间

  // 在堆上申请Message的临时空间，并在所有next_message的调用中复用
  static Message *m = (Message *)malloc(MESSAGE_SIZES[4]); // 8192

  // 构建消息头
  test_cases.pop_front();
  m->t = c.first;
  m->size = c.second;

  // 随机生成消息体
  static unsigned int seed = 1;
  int *p = (int *)(m->payload);
  for (auto i = m->payload_size() / 4; i; --i) { // 32, 128, 512, 2048, 8192
    seed = (seed * 1103515245U + 12345U) & 0x7fffffffU;
    *p++ = (int)seed;
  }

  m->checksum = crc32(m);
  return m;
}

// 记录延迟
void record(const Message *m) {
  assert(m->checksum == crc32(m));
  time_t t = now();
  delays.push_back((t - m->t) / 2);  // 来回的时间差除2近似地取作单程延迟，注意每轮通信会有四次校验和的计算，这些耗时也必须包含进延迟中
//   printf("%ld %ld %ld\n", m->t, t, (t - m->t) / 2);
  
  static set<time_t> ts;


  assert(ts.find(m->t) == ts.end());  // 每个时间戳对应的消息只应被记录一次
  ts.insert(m->t);
  for (auto c : test_cases) assert(c.first != m->t);  // 被记录的消息必须是已经被取出的测试数据
}

/* --------------------------------------不得修改两条分割线之间的内容--------------------------------------
 */
const Message *m1 = (Message *)malloc(MESSAGE_SIZES[4]); // 8192
// const Message *m1 = (Message *)memalign(4096, 8192); // 8192

void coreActionAlice(const Message * m1) {
      while(spinLock.get() != 0) {}
      send(m1);
      spinLock.set(1);
      while(spinLock.get() != 2) {}
      const Message *m2 = recv();
      spinLock.set(0);
      record(m2);
}


int main() {
  spinLock.set(0);
  while (true) {
    m1 = next_message(); 
    if (m1) {
      coreActionAlice(m1);
    } else {
      time_t dt = now() - test_cases.front().first;
      timespec req = {dt / SECOND_TO_NANO, dt % SECOND_TO_NANO}, rem;
      nanosleep(&req, &rem);  // 等待到下一条消息的发送时间
    }
  }
  return 0;
}


// int main() {

//   spinLock.set(0);
//   cout << sysconf(_SC_PAGESIZE) << endl;
//   void *c = memalign(4096, 8192);
//   while (true) {
//     m1 = next_message(); 

//     if (m1) {
//       ShareMem shm(1111, (void*)m1, sizeof(ShareMem));
//       // coreActionAlice(m1);
//       cout << "test" << endl;
//     } else {
//       time_t dt = now() - test_cases.front().first;
//       timespec req = {dt / SECOND_TO_NANO, dt % SECOND_TO_NANO}, rem;
//       nanosleep(&req, &rem);  // 等待到下一条消息的发送时间
//     }
//   }
//   return 0;
// }