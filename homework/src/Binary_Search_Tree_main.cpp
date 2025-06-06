#include <iostream>
#include <utility>
#include <random>
#include <vector>
#include <cmath>
#include <chrono>

using namespace std;

double T_op = 0.0; // 用來儲存單次操作時間

// 字典介面（抽象類別）
template <class K, class E>
class Dictionary {
public:
    virtual bool IsEmpty() const = 0;                 // 判斷字典是否為空
    virtual pair<K, E>* Get(const K&) const = 0;      // 取得指定 key 的鍵值對（若不存在回傳 nullptr）
    virtual void Insert(const pair<K, E>&) = 0;       // 插入鍵值對（若 key 已存在則更新值）
    virtual void Delete(const K&) = 0;                // 刪除指定 key 的鍵值對
};

// 二元搜尋樹的節點定義
template <class K, class E>
class TreeNode {
public:
    TreeNode* left;                   // 左子節點
    TreeNode* right;                  // 右子節點
    pair<K, E> data;                  // 節點儲存的鍵值對

    TreeNode(const pair<K, E>& e) {
        data.first = e.first;
        data.second = e.second;
        left = nullptr;
        right = nullptr;
    }
};

// BSTDictionary 使用二元搜尋樹實作 Dictionary 介面
template <class K, class E>
class BSTDictionary : public Dictionary<K, E> {
private:
    TreeNode<K, E>* root; // 根節點

    // 遞迴清除整棵樹（用於解構函式）
    void clearTree(TreeNode<K, E>* node) {
        if (node == nullptr) return;
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }

    // 遞迴插入節點
    TreeNode<K, E>* insertNode(TreeNode<K, E>* node, const pair<K, E>& entry) {
        if (node == nullptr) {
            return new TreeNode<K, E>(entry);
        }
        if (entry.first < node->data.first) {
            node->left = insertNode(node->left, entry);
        }
        else if (entry.first > node->data.first) {
            node->right = insertNode(node->right, entry);
        }
        else {
            node->data.second = entry.second; // key 重複則更新 value
        }
        return node;
    }

    // 遞迴尋找節點
    TreeNode<K, E>* findNode(TreeNode<K, E>* node, const K& key) const {
        if (node == nullptr) return nullptr;
        if (key < node->data.first) return findNode(node->left, key);
        else if (key > node->data.first) return findNode(node->right, key);
        else return node;
    }

    // 找到子樹中的最小值節點（用於刪除時找後繼）
    TreeNode<K, E>* findMin(TreeNode<K, E>* node) const {
        if (node->left == nullptr) return node;
        return findMin(node->left);
    }

    // 遞迴刪除節點
    TreeNode<K, E>* deleteNode(TreeNode<K, E>* node, const K& key) {
        if (node == nullptr) return nullptr;

        if (key < node->data.first) {
            node->left = deleteNode(node->left, key);
        }
        else if (key > node->data.first) {
            node->right = deleteNode(node->right, key);
        }
        else {
            // 找到要刪除的節點
            if (node->left == nullptr && node->right == nullptr) {
                delete node; return nullptr;
            }
            else if (node->left == nullptr) {
                TreeNode<K, E>* temp = node->right;
                delete node;
                return temp;
            }
            else if (node->right == nullptr) {
                TreeNode<K, E>* temp = node->left;
                delete node;
                return temp;
            }
            else {
                TreeNode<K, E>* succ = findMin(node->right);
                node->data = succ->data;
                node->right = deleteNode(node->right, succ->data.first);
            }
        }
        return node;
    }

    // 遞迴計算樹高
    int computeHeight(TreeNode<K, E>* node) const {
        if (node == nullptr) return 0;
        int leftH = computeHeight(node->left);
        int rightH = computeHeight(node->right);
        return 1 + max(leftH, rightH);
    }

public:
    BSTDictionary() : root(nullptr) {}
    ~BSTDictionary() { clearTree(root); }

    bool IsEmpty() const override { return (root == nullptr); }
    pair<K, E>* Get(const K& key) const override {
        TreeNode<K, E>* node = findNode(root, key);
        if (node == nullptr) return nullptr;
        return &(node->data);
    }

    void Insert(const pair<K, E>& entry) override {
        root = insertNode(root, entry);
    }

    void Delete(const K& key) override {
        root = deleteNode(root, key);
    }

    int Height() const { return computeHeight(root); }
};

int main() {
    // 要測試的 n 值（節點數）
    vector<int> ns = { 100, 500, 1000, 2000, 3000, 4000, 5000,
                       6000, 7000, 8000, 9000, 10000 };

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, 10000000); // 產生範圍內的亂數 key

    cout << "n\tHeight\tlog2(n)\tRatio(Height/log2(n))\tDeleteTime(ns)\tPredicted" << endl;

    for (int n : ns) {
        BSTDictionary<int, int> bst;
        vector<int> insertedKeys;
        insertedKeys.reserve(n);

        // 插入 n 筆亂數 key
        for (int i = 0; i < n; ++i) {
            int key = dist(gen);
            bst.Insert(make_pair(key, key));
            insertedKeys.push_back(key);
        }

        // 計算樹高與 log2(n)
        int height = bst.Height();
        double log2n = log2(static_cast<double>(n));
        double ratio = height / log2n;

        // 隨機選一個 key 來刪除
        uniform_int_distribution<int> choice(0, n - 1);
        int idxToDelete = choice(gen);
        int keyToDelete = insertedKeys[idxToDelete];

        // 重複刪除操作 10000 次並計算平均時間使結果更加準確
        long long totalDeleteTime = 0;
        for (int i = 0; i < 10000; ++i) {
            bst.Insert(make_pair(keyToDelete, keyToDelete)); // 重新插入相同 key

            auto t1 = chrono::high_resolution_clock::now();
            bst.Delete(keyToDelete);
            auto t2 = chrono::high_resolution_clock::now();

            totalDeleteTime += chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();
        }

        long long avgDeleteTime = totalDeleteTime / 10000;

        // 記錄 n=100 時的 T_op
        if (n == 100) {
            T_op = avgDeleteTime / log2(n);
        }

        // 輸出結果
        cout << n << "\t"
            << height << "\t"
            << log2n << "\t"
            << ratio << "\t\t\t"
            << avgDeleteTime << "\t\t"
            << T_op * log2(n)
            << endl;
    }

    return 0;
}
