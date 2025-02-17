#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
using namespace std;

 /**
 * Problem 1: LRU Cache (LeetCode 146)
 *
 * Design a Least Recently Used (LRU) cache that supports:
 * - `get(int key)`: Return the value of the key if it exists, otherwise return -1.
 * - `put(int key, int value)`: Insert or update the value of the key.
 *   If the cache reaches capacity, remove the least recently used key.
 *
 * Example:
 * LRUCache lru(5);
 * lru.put(1, 10);
 * lru.put(2, 20);
 * lru.put(3, 30);
 * lru.put(4, 40);
 * lru.put(5, 50);
 * cout << lru.get(1) << endl; // Output: 10
 * lru.put(6, 60); // Removes key 2
 * cout << lru.get(2) << endl; // Output: -1 (not found)
 */
class LRUCache {
public:
    class Node {
    public:
        int key, value;
        Node* next, *prev;
        Node(int key, int value) {
            this->key = key;
            this->value = value;
            next = nullptr;
            prev = nullptr;
        }
    };

    int capacity;
    unordered_map<int, Node*> mp;
    Node* dummyHead = new Node(-1, -1);
    Node* dummyTail = new Node(-1, -1);

    LRUCache(int capacity) {
        this->capacity = capacity;
        dummyHead->next = dummyTail;
        dummyTail->prev = dummyHead;
    }

    ~LRUCache() {
        Node* curr = dummyHead;
        while (curr != nullptr) {
            Node* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
    }

    void deleteNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void insertAtHead(Node* node) {
        Node* temp = dummyHead->next;
        dummyHead->next = node;
        node->prev = dummyHead;

        node->next = temp;
        temp->prev = node;
    }

    int get(int key) {
        if (mp.find(key) == mp.end()) return -1;
        else {
            Node* curNode = mp[key];
            deleteNode(curNode);
            insertAtHead(curNode);
            return curNode->value;
        }
    }

    void put(int key, int value) {
        if (mp.find(key) != mp.end()) {
            Node* currNode = mp[key];
            currNode->value = value;
            deleteNode(currNode);
            insertAtHead(currNode);
        } else {
            Node* newNode = new Node(key, value);
            if (mp.size() == capacity) {
                Node* todel = dummyTail->prev;
                mp.erase(todel->key);
                deleteNode(todel);
                delete todel;
            }
            insertAtHead(newNode);
            mp[key] = newNode;
        }
    }
};

/**
 * Problem 2: Implement Trie (LeetCode 208)
 *
 * Design a Trie (prefix tree) that supports:
 * - `insert(string word)`: Inserts `word` into the trie.
 * - `search(string word)`: Returns `true` if `word` is in the trie.
 * - `startsWith(string prefix)`: Returns `true` if any word in the trie starts with `prefix`.
 *
 * Example:
 * Trie trie;
 * trie.insert("hello");
 * cout << trie.search("hello") << endl; // Output: true
 * cout << trie.startsWith("he") << endl; // Output: true
 */
class TrieNode {
public:
    unique_ptr<TrieNode> child[26];
    bool isWord;
    TrieNode() : isWord(false) {}
};

class Trie {
private:
    unique_ptr<TrieNode> root;
public:
    Trie() {
        root = make_unique<TrieNode>();
    }

    void insert(string word) {
        TrieNode* node = root.get();
        for (char ch : word) {
            int index = ch - 'a';
            if (!node->child[index]) {
                node->child[index] = make_unique<TrieNode>();
            }
            node = node->child[index].get();
        }
        node->isWord = true;
    }

    bool search(string word) {
        TrieNode* node = root.get();
        for (char ch : word) {
            int index = ch - 'a';
            if (!node->child[index]) {
                return false;
            }
            node = node->child[index].get();
        }
        return node->isWord;
    }

    bool startsWith(string prefix) {
        TrieNode* node = root.get();
        for (char ch : prefix) {
            int index = ch - 'a';
            if (!node->child[index]) {
                return false;
            }
            node = node->child[index].get();
        }
        return true;
    }
};

/**
 * Problem 3: LFU Cache (Custom Implementation)
 *
 * Design a Least Frequently Used (LFU) cache that supports:
 * - `get(int key)`: Return the value of the key if it exists, otherwise return -1.
 * - `put(int key, int value)`: Insert or update the value of the key.
 *   If the cache reaches capacity, remove the least frequently used key.
 *
 * Example:
 * LFUCache lfu(3);
 * lfu.put(1, 10);
 * lfu.put(2, 20);
 * lfu.put(3, 30);
 * cout << lfu.get(1) << endl; // Output: 10
 * lfu.put(4, 40); // Removes key 2 (least frequently used)
 * cout << lfu.get(2) << endl; // Output: -1 (not found)
 */
class LFUCache {
private:
    struct Node {
        int key, value, freq;
        Node(int k, int v, int f) : key(k), value(v), freq(f) {}
    };

    int capacity, minFreq;
    unordered_map<int, list<Node>> freqMap; // Frequency to nodes mapping
    unordered_map<int, list<Node>::iterator> keyMap; // Key to node mapping

public:
    LFUCache(int capacity) : capacity(capacity), minFreq(0) {}

    int get(int key) {
        if (!keyMap.count(key)) return -1;
        auto it = keyMap[key];
        int val = it->value, freq = it->freq;
        freqMap[freq].erase(it);
        if (freqMap[freq].empty() && freq == minFreq) minFreq++;
        freqMap[freq + 1].push_front({key, val, freq + 1});
        keyMap[key] = freqMap[freq + 1].begin();
        return val;
    }

    void put(int key, int value) {
        if (capacity == 0) return;
        if (keyMap.count(key)) {
            keyMap[key]->value = value;
            get(key); // Update frequency
        } else {
            if (keyMap.size() == capacity) {
                auto node = freqMap[minFreq].back();
                keyMap.erase(node.key);
                freqMap[minFreq].pop_back();
            }
            freqMap[1].push_front({key, value, 1});
            keyMap[key] = freqMap[1].begin();
            minFreq = 1;
        }
    }
};

/**
 * Problem 4: Disjoint Set Union (Union-Find)
 *
 * Implement Disjoint Set Union (Union-Find) with:
 * - `find(int x)`: Returns the representative of `x`'s set.
 * - `unite(int x, int y)`: Merges sets containing `x` and `y`.
 *
 * Example:
 * DisjointSetUnion dsu(5);
 * dsu.unite(0, 1);
 * dsu.unite(2, 3);
 * cout << dsu.find(1) << endl; // Output: Representative of set containing 1
 * cout << dsu.find(3) << endl; // Output: Representative of set containing 3
 */
class DisjointSetUnion {
private:
    vector<int> parent, rank;

public:
    DisjointSetUnion(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]); // Path compression
        }
        return parent[x];
    }

    void unite(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        if (rootX != rootY) {
            if (rank[rootX] > rank[rootY]) {
                parent[rootY] = rootX;
            } else if (rank[rootX] < rank[rootY]) {
                parent[rootX] = rootY;
            } else {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
        }
    }
};

/**
 * Main Function to Test Data Structures
 */
int main() {
    // Test LRU Cache
    LRUCache lru(5);
    lru.put(1, 10);
    lru.put(2, 20);
    lru.put(3, 30);
    lru.put(4, 40);
    lru.put(5, 50);
    cout << "LRU Get(1): " << lru.get(1) << endl; // 10
    lru.put(6, 60); // Removes key 2 (LRU policy)
    cout << "LRU Get(2): " << lru.get(2) << endl; // -1 (not found)
    cout << "LRU Get(3): " << lru.get(3) << endl; // 30
    lru.put(7, 70); // Removes key 3
    cout << "LRU Get(3): " << lru.get(3) << endl; // -1 (not found)

    // Test Trie
    Trie trie;
    trie.insert("hello");
    trie.insert("world");
    trie.insert("hi");
    cout << "Trie Search(hello): " << trie.search("hello") << endl; // true
    cout << "Trie Search(world): " << trie.search("world") << endl; // true
    cout << "Trie Search(hell): " << trie.search("hell") << endl; // false
    cout << "Trie StartsWith(he): " << trie.startsWith("he") << endl; // true
    cout << "Trie StartsWith(wo): " << trie.startsWith("wo") << endl; // true

    // Test LFU Cache
    LFUCache lfu(3);
    lfu.put(1, 10);
    lfu.put(2, 20);
    lfu.put(3, 30);
    cout << "LFU Get(1): " << lfu.get(1) << endl; // 10
    lfu.put(4, 40); // Removes key 2 (least frequently used)
    cout << "LFU Get(2): " << lfu.get(2) << endl; // -1 (not found)
    cout << "LFU Get(3): " << lfu.get(3) << endl; // 30
    lfu.get(1); // Increase frequency of key 1
    lfu.put(5, 50); // Removes key 3 (least frequently used now)
    cout << "LFU Get(3): " << lfu.get(3) << endl; // -1 (not found)
    cout << "LFU Get(1): " << lfu.get(1) << endl; // 10

    // Test Disjoint Set Union
    DisjointSetUnion dsu(5);
    dsu.unite(0, 1);
    dsu.unite(2, 3);
    cout << "DSU Find(1): " << dsu.find(1) << endl;
    cout << "DSU Find(3): " << dsu.find(3) << endl;
    dsu.unite(1, 2);
    cout << "DSU Find(0): " << dsu.find(0) << endl; // Now 0, 1, 2, and 3 are in the same set
    return 0;

    return 0;
}
