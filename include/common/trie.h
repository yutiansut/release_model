#pragma once

#include <stdexcept>
#include <functional>
#include <vector>

/*
 * support ASCII key
 */
namespace husky {
    const int KEY_SIZE = 'z' - 'a' + 1 + 10; // alphabet + number

    template<typename T>
    class trie_node {
    public:
        trie_node *children[KEY_SIZE];
        T data;

        trie_node& operator=(trie_node& other) {
            this->children = other.children;
            this->data = other.data;
            return *this;
        }

        trie_node() {
            memset(children, 0, sizeof(children));
        }

        ~trie_node() {
            for (int i = 0; i < KEY_SIZE; ++i) {
                if (children[i] != nullptr) {
                    delete children[i];
                    children[i] = nullptr;
                }
            }
        }
    };


    template<typename T>
    class trie {
    public:
        trie() {
            root = new trie_node<T>();
        }

        trie& operator=(trie& other) {
            this->keys = other.keys;
            this->root = other.root;
            return *this;
        }

        trie(const trie& other) {
            this->keys = other.keys;
            this->root = other.root;
        }

        ~trie() {
            if (root != nullptr) {
                delete root;
                root = nullptr;
            }
        }

        // convert char to the corresponding index
        inline static int char2idx(const int c) {
            if (c <= 57) { // number
                return c - 22;
            } else if (c <= 90) { // upper case
                return c + 32 - 'a';
            } else { // lower case
                return c - 'a';
            }
        }

        void insert(const char *s, int N, T t) {
            trie_node<T> *current = root;
            for (int i = 0; i < N; ++i) {
                int curidx = char2idx(s[i]);
                if (current->children[curidx] == nullptr) {
                    current->children[curidx] = new trie_node<T>();
                }
                current = current->children[curidx];
            }
            current->data = t;
            keys.push_back(s);
        }

        T& search(const char *s, int N) {
            if (N == 0) throw std::runtime_error("trie N cannot be zero");
            trie_node<T> *current = root;
            for (int i = 0; i < N; ++i) {
                current = current->children[char2idx(s[i])];
                if (current == nullptr) throw std::runtime_error("cannot find " + std::string(s) + " in trie");
            }
            return current->data;
        }

        T& search(const char *s) {
            trie_node<T> *current = root;
            int i = 0;
            while (s[i] != '\0') {
                current = current->children[char2idx(s[i])];
                if (current == nullptr) throw std::runtime_error("cannot find " + std::string(s) + " in trie");
                ++i;
            }
            return current->data;
        }

        T* try_search(const char *s) {
            trie_node<T> *current = root;
            int i = 0;
            while (s[i] != '\0') {
                current = current->children[char2idx(s[i])];
                if (current == nullptr) return nullptr;
                ++i;
            }
            return &current->data;
        }


        T* try_search(const char *s, int N) {
            trie_node<T> *current = root;
            for (int i = 0; i < N; ++i) {
                current = current->children[char2idx(s[i])];
                if (current == nullptr) return nullptr;
            }
            return &current->data;
        }

        void batch_process(std::function<void(T&)> func) {
            for (const auto& key: keys) {
                func(this->search(key.c_str(), key.size()));
            }
        }

        T& operator[](int index) {
            return this->search(keys[index].c_str(), keys[index].size());
        }

        bool contains(const char *s, int N) {
            if (N == 0) throw std::runtime_error("trie N cannot be zero");
            trie_node<T> *current = root;
            for (int i = 0; i < N; ++i) {
                current = current->children[char2idx(s[i])];
                if (current == nullptr) return false;
            }
            return true;
        }

        const std::vector<std::string>& get_keys() {
            return keys;
        }

        void clear() {
            delete root;
            root = nullptr;
            keys.clear();
            root = new trie_node<T>();
        }

        unsigned int size() {
            return keys.size();
        }

    private:
        trie_node<T> *root;
        std::vector<std::string> keys;
    };
}