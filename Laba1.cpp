#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

using namespace std;

const int BUFFER_SIZE = 10;//размер буфера 

mutex mtx;
condition_variable bufferNotFull, bufferNotEmpty;

vector<int> buffer(BUFFER_SIZE, 0);
int readIndex = 0;
int writeIndex = 0;

void writer(int id, int numMessages) {
    for (int i = 0; i < numMessages; ++i) {
        unique_lock<mutex> lock(mtx);
        bufferNotFull.wait(lock, [] { return (writeIndex + 1) % BUFFER_SIZE != readIndex; });
        buffer[writeIndex] = id;
        writeIndex = (writeIndex + 1) % BUFFER_SIZE;
        cout << "Писатель " << id << " написал сообщение." <<endl;
        bufferNotEmpty.notify_all();
    }
}

void reader(int id, int numMessages) {
    for (int i = 0; i < numMessages; ++i) {
        unique_lock<std::mutex> lock(mtx);
        bufferNotEmpty.wait(lock, [] { return readIndex != writeIndex; });
        int data = buffer[readIndex];
        readIndex = (readIndex + 1) % BUFFER_SIZE;
        cout << "Читатель " << id << " прочитал сообщение от Писателя " << data << "." << endl;
        bufferNotFull.notify_all();
    }
}

int main() {

    setlocale(LC_ALL, "Russian");

    const int numWriters = 2;
    const int numReaders = 3;
    const int messagesPerWriter = 5;
    const int messagesPerReader = 5;

    vector<thread> writers;
    vector<thread> readers;

    for (int i = 0; i < numWriters; ++i) {
        writers.emplace_back(writer, i, messagesPerWriter);
    }

    for (int i = 0; i < numReaders; ++i) {
        readers.emplace_back(reader, i, messagesPerReader);
    }

    for (auto& writerThread : writers) {
        writerThread.join();
    }

    for (auto& readerThread : readers) {
        readerThread.join();
    }

    return 0;
}
