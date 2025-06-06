#include <iostream>
#include <utility>
#include <random>
#include <vector>
#include <cmath>
#include <chrono>

using namespace std;

double T_op = 0.0; // �Ψ��x�s�榸�ާ@�ɶ�

// �r�夶���]��H���O�^
template <class K, class E>
class Dictionary {
public:
    virtual bool IsEmpty() const = 0;                 // �P�_�r��O�_����
    virtual pair<K, E>* Get(const K&) const = 0;      // ���o���w key ����ȹ�]�Y���s�b�^�� nullptr�^
    virtual void Insert(const pair<K, E>&) = 0;       // ���J��ȹ�]�Y key �w�s�b�h��s�ȡ^
    virtual void Delete(const K&) = 0;                // �R�����w key ����ȹ�
};

// �G���j�M�𪺸`�I�w�q
template <class K, class E>
class TreeNode {
public:
    TreeNode* left;                   // ���l�`�I
    TreeNode* right;                  // �k�l�`�I
    pair<K, E> data;                  // �`�I�x�s����ȹ�

    TreeNode(const pair<K, E>& e) {
        data.first = e.first;
        data.second = e.second;
        left = nullptr;
        right = nullptr;
    }
};

// BSTDictionary �ϥΤG���j�M���@ Dictionary ����
template <class K, class E>
class BSTDictionary : public Dictionary<K, E> {
private:
    TreeNode<K, E>* root; // �ڸ`�I

    // ���j�M����ʾ�]�Ω�Ѻc�禡�^
    void clearTree(TreeNode<K, E>* node) {
        if (node == nullptr) return;
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }

    // ���j���J�`�I
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
            node->data.second = entry.second; // key ���ƫh��s value
        }
        return node;
    }

    // ���j�M��`�I
    TreeNode<K, E>* findNode(TreeNode<K, E>* node, const K& key) const {
        if (node == nullptr) return nullptr;
        if (key < node->data.first) return findNode(node->left, key);
        else if (key > node->data.first) return findNode(node->right, key);
        else return node;
    }

    // ���l�𤤪��̤p�ȸ`�I�]�Ω�R���ɧ���~�^
    TreeNode<K, E>* findMin(TreeNode<K, E>* node) const {
        if (node->left == nullptr) return node;
        return findMin(node->left);
    }

    // ���j�R���`�I
    TreeNode<K, E>* deleteNode(TreeNode<K, E>* node, const K& key) {
        if (node == nullptr) return nullptr;

        if (key < node->data.first) {
            node->left = deleteNode(node->left, key);
        }
        else if (key > node->data.first) {
            node->right = deleteNode(node->right, key);
        }
        else {
            // ���n�R�����`�I
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

    // ���j�p���
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
    // �n���ժ� n �ȡ]�`�I�ơ^
    vector<int> ns = { 100, 500, 1000, 2000, 3000, 4000, 5000,
                       6000, 7000, 8000, 9000, 10000 };

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, 10000000); // ���ͽd�򤺪��ü� key

    cout << "n\tHeight\tlog2(n)\tRatio(Height/log2(n))\tDeleteTime(ns)\tPredicted" << endl;

    for (int n : ns) {
        BSTDictionary<int, int> bst;
        vector<int> insertedKeys;
        insertedKeys.reserve(n);

        // ���J n ���ü� key
        for (int i = 0; i < n; ++i) {
            int key = dist(gen);
            bst.Insert(make_pair(key, key));
            insertedKeys.push_back(key);
        }

        // �p��𰪻P log2(n)
        int height = bst.Height();
        double log2n = log2(static_cast<double>(n));
        double ratio = height / log2n;

        // �H����@�� key �ӧR��
        uniform_int_distribution<int> choice(0, n - 1);
        int idxToDelete = choice(gen);
        int keyToDelete = insertedKeys[idxToDelete];

        // ���ƧR���ާ@ 10000 ���íp�⥭���ɶ��ϵ��G��[�ǽT
        long long totalDeleteTime = 0;
        for (int i = 0; i < 10000; ++i) {
            bst.Insert(make_pair(keyToDelete, keyToDelete)); // ���s���J�ۦP key

            auto t1 = chrono::high_resolution_clock::now();
            bst.Delete(keyToDelete);
            auto t2 = chrono::high_resolution_clock::now();

            totalDeleteTime += chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();
        }

        long long avgDeleteTime = totalDeleteTime / 10000;

        // �O�� n=100 �ɪ� T_op
        if (n == 100) {
            T_op = avgDeleteTime / log2(n);
        }

        // ��X���G
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
