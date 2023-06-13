#include <iostream>
#ifndef ARRAYLIST_HPP
    #include "ArrayList.hpp" // Assuming this header file contains the implementation of ArrayList
#endif
using namespace std;

#define FQT_HPP
#define InnerList ArrayList<FQT::InnerNode *> // Alias for ArrayList of InnerNode pointers
#define LeafList ArrayList<FQT::LeafNode *> // Alias for ArrayList of LeafNode pointers
#define DataList ArrayList<FQT::Data *> // Alias for ArrayList of Data pointers

#ifndef abs
    #define abs(a) ((a) > 0 ? (a) : -1 * (a)) // Macro definition for absolute value
#endif
#ifndef min
    #define min(a, b) ((a) < (b) ? (a) : (b)) // Macro definition for minimum value
#endif

class FQT {

public:

    struct Data {
        double data;

        Data() {
            data = 0;
        }

        Data(double r) {
            data = r;
        }
    };

    static inline long dist(Data &d1, Data &d2) {
        // Calculate the distance between two Data objects
        double dist = abs(d1.data - d2.data);
        if (dist - (long)dist < 0.5) {
            return (long)dist; // Return the distance as a long value
        }
        return (long)dist + 1;
    }

private:

    struct LeafNode {
        long dist; // Distance from the pivot
        DataList data; // List of Data objects

        LeafNode(long d) {
            dist = d;
        }
    };

    struct InnerNode {
        long dist; // Distance from the pivot
        InnerList children; // List of child InnerNodes
        LeafList leaves; // List of LeafNodes

        InnerNode() {
            dist = 0;
            leaves.~ArrayList();
        }

        InnerNode(long d, bool hasLeaf) {
            dist = d;
            if (hasLeaf) {
                children.~ArrayList();
            }
            else {
                leaves.~ArrayList();
            }
        }

        ~InnerNode() {
            // Destructor to free memory
            for (long i = 0; i < min(children.size(), leaves.size()); i++) {
                delete(children[i]);
                delete(leaves[i]);
            }
            for (long i = 0; i < children.size(); i++) {
                delete(children[i]);
            }
            for (long i = 0; i < leaves.size(); i++) {
                delete(leaves[i]);
            }
        }

        void add(Data &data, Data *pivots, long len) {
            // Add a Data object to the FQT

            long i = 0, d = FQT::dist(data, pivots[0]); // Calculate the distance from the first pivot

            if (len == 1) {
                // If there is only one pivot, add the data to the corresponding LeafNode
                for (; i < leaves.size() && d < leaves[i]->dist; i++);
                if (i == leaves.size() || d != leaves[i]->dist) {
                    leaves.add(new LeafNode(d), i); // Create a new LeafNode if the distance doesn't exist in the list
                }
                leaves[i]->data.add(&data); // Add the Data object to the LeafNode
                return;
            }

            // If there are multiple pivots, add the data to the corresponding InnerNode
            for (; i < children.size() && children[i]->dist < d; i++);
            if (i == children.size() || d != children[i]->dist) {
                children.add(new InnerNode(d, (len == 1 ? true : false)), i); // Create a new InnerNode if the distance doesn't exist in the list
            }
            children[i]->add(data, pivots + 1, len - 1); // Recursively add the Data object to the child InnerNode
        }

        DataList& srch(Data &q, long r, Data *pivots, long len) {
            // Search for Data objects within a given range from a query point

            long d = FQT::dist(q, pivots[0]); // Calculate the distance from the first pivot
            DataList *list = new DataList; // Create a new DataList to store the results

            if (len == 1) {
                // If there is only one pivot, perform a binary search on the LeafNodes
                long s = 0, e = leaves.size(), m;
                while (s < e) {
                    m = s + (e - s) / 2;
                    long d2 = leaves[m]->dist;
                    if (d - r > d2) {
                        s = m + 1;
                    }
                    else if (d + r < d2) {
                        e = m;
                    }
                    else {
                        list->addAll(leaves[m]->data); // Add the Data objects from the LeafNode to the result list
                        for (long i = m - 1; i > -1 && (d2 = leaves[i]->dist) <= d + r && d2 >= d - r; i--) {
                            list->addAll(leaves[i]->data); // Add the Data objects from the previous LeafNodes within the range
                        }
                        for (long i = m + 1; i < leaves.size() && (d2 = leaves[i]->dist) <= d + r && d2 >= d - r; i++) {
                            list->addAll(leaves[i]->data); // Add the Data objects from the next LeafNodes within the range
                        }
                        return *list;
                    }
                }
            }

            // If there are multiple pivots, perform a binary search on the child InnerNodes
            long s = 0, e = children.size(), m;
            while (s < e) {
                m = s + (e - s) / 2;
                long d2 = children[m]->dist;
                if (d - r > d2) {
                    s = m + 1;
                }
                else if (d + r < d2) {
                    e = m;
                }
                else {
                    DataList &temp = children[m]->srch(q, r, pivots + 1, len - 1); // Recursively search the child InnerNode
                    list->addAll(temp); // Add the Data objects from the child InnerNode to the result list
                    delete(&temp);
                    for (long i = m - 1; i > -1 && (d2 = children[i]->dist) <= d + r && d2 >= d - r; i--) {
                        temp = children[i]->srch(q, r, pivots + 1, len - 1); // Recursively search the previous child InnerNodes within the range
                        list->addAll(temp); // Add the Data objects to the result list
                        delete(&temp);
                    }
                    for (long i = m + 1; i < children.size() && (d2 = children[i]->dist) <= d + r && d2 >= d - r; i++) {
                        temp = children[i]->srch(q, r, pivots + 1, len - 1); // Recursively search the next child InnerNodes within the range
                        list->addAll(temp); // Add the Data objects to the result list
                        delete(&temp);
                    }
                    return *list;
                }
            }
            return *list; // Return the result list
        }

        DataList& NN(Data &q, Data *pivots, long len) {
            // Find the nearest neighbor of a query point

            for (long r = 1;; r++) {
                DataList &list = srch(q, r, pivots, len); // Search for Data objects within increasing ranges
                if (list.size() != 0) {
                    return list; // Return the result list if Data objects are found
                }
                delete(&list); // Free memory if Data objects are not found
            }
        }

        void print() {
            // Print the Data objects in the FQT
            for (long i = 0; i < leaves.size(); i++) {
                for (long j = 0; j < leaves[i]->data.size(); j++) {
                    cout << leaves[i]->data[j]->data << endl; // Print the data value of each Data object
                }
            }
            for (long i = 0; i < children.size(); i++) {
                children[i]->print(); // Recursively print the Data objects in the child InnerNodes
            }
        }
    };

    InnerNode root; // Root node of the FQT
    Data *pivots; // Array of pivot Data objects
    long height; // Number of pivots

public:

    FQT(Data *p, long len) {
        pivots = new Data[height = len]; // Allocate memory for the pivot array and store the number of pivots
        for (long i = 0; i < len; i++) {
            pivots[i] = p[i]; // Copy the pivot Data objects into the pivot array
        }
    }

    ~FQT() {
        delete[](pivots); // Free memory allocated for the pivot array
    }

    inline void add(Data &d) {
        root.add(d, pivots, height); // Add a Data object to the FQT by calling the add() function of the root node
    }

    inline DataList& search(Data &query, long radius) {
        return root.srch(query, radius, pivots, height); // Search for Data objects within a given range from a query point
    }

    inline DataList& nearestNeighbor(Data &query) {
        return root.NN(query, pivots, height); // Find the nearest neighbor of a query point
    }

    inline void print() {
        root.print(); // Print the Data objects in the FQT
    }
};
