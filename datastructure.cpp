#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstdlib>
#include <atomic>
using namespace std;
class PacketQueue {
private:
    queue<int> q;
    mutex mtx;
    condition_variable cv;
    const int MAX_SIZE;
    atomic<bool> running{true};
public:
    PacketQueue(int maxSize) : MAX_SIZE(maxSize) {}
    void addPacket(int packetId) {
        unique_lock<mutex> lock(mtx);
        if (!running) return;
        if (q.size() >= MAX_SIZE) {
            cout << "[CONGESTION] Queue full! Dropping packet " << packetId << endl;
        } else {
            q.push(packetId);
            cout << "[ARRIVAL]   Packet " << packetId << " arrived. Queue size = " << q.size() << endl;
            cv.notify_one(); // Wake up consumer
        }
    }
    int getPacket() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]() { return !q.empty() || !running; });
        if (!q.empty()) {
            int id = q.front();
            q.pop();
            return id;
        }
        return -1; 
    }
    void stop() {
        unique_lock<mutex> lock(mtx);
        running = false;
        cv.notify_all(); 
    }
    size_t remainingPackets() {
        lock_guard<mutex> lock(mtx);
        return q.size();
    }
    bool isRunning() const { return running; }
};
void packetProducer(PacketQueue& pq) {
    int packetId = 1;
    while (pq.isRunning()) {
        this_thread::sleep_for(chrono::milliseconds(rand() % 800 + 200)); 
        pq.addPacket(packetId++);
    }
}
void packetConsumer(PacketQueue& pq) {
    while (pq.isRunning()) {
        int packetId = pq.getPacket();
        if (packetId == -1 && !pq.isRunning()) break;
        if (packetId != -1) {
            cout << "    [PROCESS] Packet " << packetId << " processed. Queue size = "
                 << pq.remainingPackets() << endl;
            this_thread::sleep_for(chrono::milliseconds(700));
        }
    }
}
int main() {
    srand(time(0));
    const int MAX_QUEUE_SIZE = 5;
    const int SIMULATION_TIME = 10; 
    PacketQueue pq(MAX_QUEUE_SIZE);
    thread producer(packetProducer, ref(pq));
    thread consumer(packetConsumer, ref(pq));this_thread::sleep_for(chrono::seconds(SIMULATION_TIME));
    pq.stop();
    if (producer.joinable()) producer.join();
    if (consumer.joinable()) consumer.join();
    cout << "\nSimulation ended. Remaining packets in queue: " << pq.remainingPackets() << endl;
    return 0;}
