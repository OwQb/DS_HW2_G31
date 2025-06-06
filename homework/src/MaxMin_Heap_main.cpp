#include <iostream>
#include <stdexcept>
#include <chrono>
#include <cmath>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace chrono;

//===== 抽象類別 PQ(Max和Min)=====
template <class T>
class PQ {
public:
    virtual ~PQ() {}//建構子解構子
    virtual bool IsEmpty() const = 0;//判斷是否為空
    virtual const T& Top() const = 0;//取得堆頂元素（最小或最大）
    virtual void Push(const T&) = 0; //插入元素
    virtual void Pop() = 0;                //移除堆頂元素
};

//======= 最小堆（MinHeap）實作 =======
template <class T>
class MinHeap : public PQ<T> {
public:
    MinHeap(int theCapacity = 10);//建構子，預設容量 10
    ~MinHeap() { delete[] heap; }//解構子

    bool IsEmpty() const override { return heapSize == 0; }
    const T& Top() const override;//傳回堆頂元素
    void Push(const T&) override;//插入新元素
    void Pop() override;//刪除堆頂元素

private:
    T* heap;//儲存堆的陣列（heap[0] 不使用）
    int capacity;//陣列最大容量
    int heapSize; //當前堆中元素數量
    void ChangeSize1D(T*& a, int oldSize, int newSize); //動態擴充陣列大小
};

template <class T>
MinHeap<T>::MinHeap(int theCapacity) {
    if (theCapacity < 1) throw "Capacity must be >= 1.";
    capacity = theCapacity;
    heapSize = 0;
    heap = new T[capacity + 1];//從 index 1 開始存放元素
}

template <class T>
const T& MinHeap<T>::Top() const {
    if (IsEmpty()) throw "Heap is empty.";
    return heap[1];
}

template <class T>
void MinHeap<T>::ChangeSize1D(T*& a, int oldSize, int newSize) {
    if (newSize < 1) throw "New size must be >= 1.";
    T* temp = new T[newSize + 1];
    for (int i = 1; i <= oldSize; ++i)
        temp[i] = a[i];
    delete[] a;
    a = temp;
}

template <class T>
void MinHeap<T>::Push(const T& e) {
    if (heapSize == capacity) { //若堆滿，擴充容量
        ChangeSize1D(heap, capacity, 2 * capacity);
        capacity *= 2;
    }
    int currentNode = ++heapSize;//新元素位置
    //自底向上調整（維持最小堆性質）
    while (currentNode != 1 && heap[currentNode / 2] > e) {
        heap[currentNode] = heap[currentNode / 2];
        currentNode /= 2;
    }
    heap[currentNode] = e;
}

template <class T>
void MinHeap<T>::Pop() {
    if (IsEmpty()) throw "Heap is empty. Cannot delete.";
    T lastE = heap[heapSize--];//取出最後一個元素
    int currentNode = 1;
    int child = 2;
    //自頂向下調整（維持最小堆性質）
    while (child <= heapSize) {
        if (child < heapSize && heap[child + 1] < heap[child]) child++; //選出較小的子節點
        if (lastE <= heap[child]) break;
        heap[currentNode] = heap[child];
        currentNode = child;
        child *= 2;
    }
    heap[currentNode] = lastE;
}

//======= 最大堆（MaxHeap）實作 =======
//類似 MinHeap，只是比較方向相反
template <class T>
class MaxHeap : public PQ<T> {
public:
    MaxHeap(int theCapacity = 10);
    ~MaxHeap() { delete[] heap; }

    bool IsEmpty() const override { return heapSize == 0; }
    const T& Top() const override;
    void Push(const T&) override;
    void Pop() override;

private:
    T* heap;
    int capacity;
    int heapSize;
    void ChangeSize1D(T*& a, int oldSize, int newSize);
};

template <class T>
MaxHeap<T>::MaxHeap(int theCapacity) {
    if (theCapacity < 1) throw "Capacity must be >= 1.";
    capacity = theCapacity;
    heapSize = 0;
    heap = new T[capacity + 1]; //從 index 1 開始存放元素
}

template <class T>
const T& MaxHeap<T>::Top() const {
    if (IsEmpty()) throw "Heap is empty.";
    return heap[1];
}

template <class T>
void MaxHeap<T>::ChangeSize1D(T*& a, int oldSize, int newSize) {
    if (newSize < 1) throw "New size must be >= 1.";
    T* temp = new T[newSize + 1];
    for (int i = 1; i <= oldSize; ++i)
        temp[i] = a[i];
    delete[] a;
    a = temp;
}

template <class T>
void MaxHeap<T>::Push(const T& e) {
    if (heapSize == capacity) {
        ChangeSize1D(heap, capacity, 2 * capacity);
        capacity *= 2;
    }
    int currentNode = ++heapSize;
    //自底向上調整（維持最大堆性質）
    while (currentNode != 1 && heap[currentNode / 2] < e) {
        heap[currentNode] = heap[currentNode / 2];
        currentNode /= 2;
    }
    heap[currentNode] = e;
}

template <class T>
void MaxHeap<T>::Pop() {
    if (IsEmpty()) throw "Heap is empty. Cannot delete.";
    T lastE = heap[heapSize--];
    int currentNode = 1;
    int child = 2;
    //自頂向下調整（維持最大堆性質）
    while (child <= heapSize) {
        if (child < heapSize && heap[child + 1] > heap[child]) child++;
        if (lastE >= heap[child]) break;
        heap[currentNode] = heap[child];
        currentNode = child;
        child *= 2;
    }
    heap[currentNode] = lastE;
}

//======= 基準測試函式 =======
void BenchmarkHeap(PQ<int>* pq, const string& name) {
    vector<int> ns = { 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000 };
    double basePushTime = 0, basePopTime = 0, baseEmptyTime = 0, baseTopTime = 0;
    const int repeat = 10000;

    cout << "\n==== " << name << " ====\n";
    cout << left << setw(10) << "n"
        << setw(15) << "Push Time"
        << setw(15) << "Predicted"
        << setw(15) << "Pop Time"
        << setw(15) << "Predicted"
        << setw(15) << "IsEmpty Time"
        << setw(15) << "Predicted"
        << setw(15) << "Top Time"
        << "Predicted" << endl;

    for (size_t i = 0; i < ns.size(); ++i) {
        int n = ns[i];

        delete pq;
        pq = (name == "MinHeap") ? static_cast<PQ<int>*>(new MinHeap<int>(2 * n))
            : static_cast<PQ<int>*>(new MaxHeap<int>(2 * n));

        // 預先建立 heap
        for (int j = 0; j < n; ++j)
            pq->Push(rand());

        // Push 測試（重複 repeat 次）
        auto start = high_resolution_clock::now();
        for (int j = 0; j < repeat; ++j) {
            pq->Push(rand());
            pq->Pop(); // 維持堆大小不變
        }
        auto end = high_resolution_clock::now();
        double pushTime = duration_cast<nanoseconds>(end - start).count() / static_cast<double>(repeat);

        // Pop 測試（預先插入一筆，重複 pop+push）
        pq->Push(rand());
        start = high_resolution_clock::now();
        for (int j = 0; j < repeat; ++j) {
            pq->Pop();
            pq->Push(rand());
        }
        end = high_resolution_clock::now();
        double popTime = duration_cast<nanoseconds>(end - start).count() / static_cast<double>(repeat);

        // IsEmpty 測試（只呼叫，不改變堆狀態）
        start = high_resolution_clock::now();
        for (int j = 0; j < repeat; ++j)
            pq->IsEmpty();
        end = high_resolution_clock::now();
        double emptyTime = duration_cast<nanoseconds>(end - start).count() / static_cast<double>(repeat);

        // Top 測試（只呼叫，不改變堆狀態）
        start = high_resolution_clock::now();
        for (int j = 0; j < repeat; ++j)
            pq->Top();
        end = high_resolution_clock::now();
        double topTime = duration_cast<nanoseconds>(end - start).count() / static_cast<double>(repeat);

        // 預測時間（基準：第一次）
        if (i == 0) {
            basePushTime = pushTime / log2(n);
            basePopTime = popTime / log2(n);
            baseEmptyTime = emptyTime;
            baseTopTime = topTime;
        }

        double predPush = basePushTime * log2(n);
        double predPop = basePopTime * log2(n);
        double predEmpty = baseEmptyTime;
        double predTop = baseTopTime;

        cout << setw(10) << n
            << setw(15) << fixed << setprecision(4) << pushTime
            << setw(15) << predPush
            << setw(15) << popTime
            << setw(15) << predPop
            << setw(15) << emptyTime
            << setw(15) << predEmpty
            << setw(15) << topTime
            << predTop << endl;
    }

    delete pq;
}

int main() {
    srand(time(0));
    cout << "Time unit : ns";
    BenchmarkHeap(new MinHeap<int>(), "MinHeap");
    BenchmarkHeap(new MaxHeap<int>(), "MaxHeap");
    return 0;
}