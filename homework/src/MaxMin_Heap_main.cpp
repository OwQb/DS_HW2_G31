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

//===== ��H���O PQ(Max�MMin)=====
template <class T>
class PQ {
public:
    virtual ~PQ() {}//�غc�l�Ѻc�l
    virtual bool IsEmpty() const = 0;//�P�_�O�_����
    virtual const T& Top() const = 0;//���o�ﳻ�����]�̤p�γ̤j�^
    virtual void Push(const T&) = 0; //���J����
    virtual void Pop() = 0;                //�����ﳻ����
};

//======= �̤p��]MinHeap�^��@ =======
template <class T>
class MinHeap : public PQ<T> {
public:
    MinHeap(int theCapacity = 10);//�غc�l�A�w�]�e�q 10
    ~MinHeap() { delete[] heap; }//�Ѻc�l

    bool IsEmpty() const override { return heapSize == 0; }
    const T& Top() const override;//�Ǧ^�ﳻ����
    void Push(const T&) override;//���J�s����
    void Pop() override;//�R���ﳻ����

private:
    T* heap;//�x�s�諸�}�C�]heap[0] ���ϥΡ^
    int capacity;//�}�C�̤j�e�q
    int heapSize; //��e�襤�����ƶq
    void ChangeSize1D(T*& a, int oldSize, int newSize); //�ʺA�X�R�}�C�j�p
};

template <class T>
MinHeap<T>::MinHeap(int theCapacity) {
    if (theCapacity < 1) throw "Capacity must be >= 1.";
    capacity = theCapacity;
    heapSize = 0;
    heap = new T[capacity + 1];//�q index 1 �}�l�s�񤸯�
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
    if (heapSize == capacity) { //�Y�ﺡ�A�X�R�e�q
        ChangeSize1D(heap, capacity, 2 * capacity);
        capacity *= 2;
    }
    int currentNode = ++heapSize;//�s������m
    //�۩��V�W�վ�]�����̤p��ʽ�^
    while (currentNode != 1 && heap[currentNode / 2] > e) {
        heap[currentNode] = heap[currentNode / 2];
        currentNode /= 2;
    }
    heap[currentNode] = e;
}

template <class T>
void MinHeap<T>::Pop() {
    if (IsEmpty()) throw "Heap is empty. Cannot delete.";
    T lastE = heap[heapSize--];//���X�̫�@�Ӥ���
    int currentNode = 1;
    int child = 2;
    //�۳��V�U�վ�]�����̤p��ʽ�^
    while (child <= heapSize) {
        if (child < heapSize && heap[child + 1] < heap[child]) child++; //��X���p���l�`�I
        if (lastE <= heap[child]) break;
        heap[currentNode] = heap[child];
        currentNode = child;
        child *= 2;
    }
    heap[currentNode] = lastE;
}

//======= �̤j��]MaxHeap�^��@ =======
//���� MinHeap�A�u�O�����V�ۤ�
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
    heap = new T[capacity + 1]; //�q index 1 �}�l�s�񤸯�
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
    //�۩��V�W�վ�]�����̤j��ʽ�^
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
    //�۳��V�U�վ�]�����̤j��ʽ�^
    while (child <= heapSize) {
        if (child < heapSize && heap[child + 1] > heap[child]) child++;
        if (lastE >= heap[child]) break;
        heap[currentNode] = heap[child];
        currentNode = child;
        child *= 2;
    }
    heap[currentNode] = lastE;
}

//======= ��Ǵ��ը禡 =======
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

        // �w���إ� heap
        for (int j = 0; j < n; ++j)
            pq->Push(rand());

        // Push ���ա]���� repeat ���^
        auto start = high_resolution_clock::now();
        for (int j = 0; j < repeat; ++j) {
            pq->Push(rand());
            pq->Pop(); // ������j�p����
        }
        auto end = high_resolution_clock::now();
        double pushTime = duration_cast<nanoseconds>(end - start).count() / static_cast<double>(repeat);

        // Pop ���ա]�w�����J�@���A���� pop+push�^
        pq->Push(rand());
        start = high_resolution_clock::now();
        for (int j = 0; j < repeat; ++j) {
            pq->Pop();
            pq->Push(rand());
        }
        end = high_resolution_clock::now();
        double popTime = duration_cast<nanoseconds>(end - start).count() / static_cast<double>(repeat);

        // IsEmpty ���ա]�u�I�s�A�����ܰ窱�A�^
        start = high_resolution_clock::now();
        for (int j = 0; j < repeat; ++j)
            pq->IsEmpty();
        end = high_resolution_clock::now();
        double emptyTime = duration_cast<nanoseconds>(end - start).count() / static_cast<double>(repeat);

        // Top ���ա]�u�I�s�A�����ܰ窱�A�^
        start = high_resolution_clock::now();
        for (int j = 0; j < repeat; ++j)
            pq->Top();
        end = high_resolution_clock::now();
        double topTime = duration_cast<nanoseconds>(end - start).count() / static_cast<double>(repeat);

        // �w���ɶ��]��ǡG�Ĥ@���^
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