#include <iostream>
using namespace std;

#define ARRAYLIST_HPP
#define ARRAYLIST_DEFAULT_SIZE 10

template <typename T>
class ArrayList {

    T *arr;
    long cap, len;

    void grow() {
        T *a = new T[cap *= 2];
        for (long i = 0; i < len; i++) {
            a[i] = arr[i];
        }
        delete[](arr);
        arr = a;
    }
    
    public:

    ArrayList() {
        arr = new T[cap = ARRAYLIST_DEFAULT_SIZE];
        len = 0;
    }

    ArrayList(long c) {
        arr = new T[cap = c];
        len = 0;
    }

    ~ArrayList() {
        delete[](arr);
        cap = len = 0;
        arr = nullptr;
    }

    inline T &operator [] (long ind) {
        return arr[ind];
    }

    inline long size() {
        return len;
    }

    void add(T val) {
        if (len == cap) {
            grow();
        }
        arr[len++] = val;
    }

    void add(T val, long ind) {
        if (ind == len) {
            return add(val);
        } if (ind < 0 || ind > len) {
            return;
        }

        if (len == cap) {
            grow();
        }
        for (long i = len - 1; i >= ind; i--) {
            arr[i + 1] = arr[i];
        }
        arr[ind] = val;
        len++;
    }

    void addAll(T *a, long n) {
        for (long i = 0; i < n; i++) {
            add(a[i]);
        }
    }

    void addAll(ArrayList<T> &list) {
        for (long i = 0; i < list.size(); i++) {
            add(list[i]);
        }
    }

    void remove(long ind) {
        if (ind < 0 || ind >= len) {
            return;
        }
        len--;
        for (long i = ind; i < len; i++) {
            arr[i] = arr[i + 1];
        }
    }

    void initialise() {
        if (cap != 0) {
            ArrayList::~ArrayList();
        }
        ArrayList();
    }
};

template <typename T>
ostream &operator <<(ostream &os, ArrayList<T> &list) {
    os << "[";
    for (long i = 0; i < list.size(); i++) {
        os << list[i] << (i == list.size() - 1 ? "]" : ", ");
    }
    return os;
}
