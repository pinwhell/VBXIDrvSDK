#pragma once

#pragma once

#include <etl/unordered_map.h>

template <typename K, typename V, unsigned int CAPACITY>
class BidirectionalMap {
public:
    inline void insert(const K& key, const V& value) {
        forward[key] = value;
        backward[value] = key;
    }

    inline const V& getValue(const K& key) const {
        return forward.at(key);
    }

    inline const K& getKey(const V& value) const {
        return backward.at(value);
    }

    inline bool containsKey(const K& key) const {
        return forward.find(key) != forward.end();
    }

    inline bool containsValue(const V& value) const {
        return backward.find(value) != backward.end();
    }

    inline void eraseByKey(const K& key) {
        if (containsKey(key) == false)
            return;

        const V& value = forward[key];
        forward.erase(key);
        backward.erase(value);
    }

    inline void eraseByValue(const V& value) {
        if (containsValue(value) == false)
            return;

        const K& key = backward[value];
        backward.erase(value);
        forward.erase(key);
    }

    etl::unordered_map<K, V, CAPACITY> forward;
    etl::unordered_map<V, K, CAPACITY> backward;
};