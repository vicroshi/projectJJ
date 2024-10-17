#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <list>
#include <random>
#include <vector>
#include <iterator>
class Node {
    public:
        int id;
        int deg;
};
using namespace std;
template <typename Iterator, typename Predicate>
class FilterIterator {
    public:
        FilterIterator(Iterator begin, Iterator end, Predicate pred)
                : current_(begin), end_(end), pred_(pred) {
            // Advance to the first element that satisfies the predicate
            advance();
        }

        // Dereference
        decltype(auto) operator*() const {
            return *current_;
        }

        // Increment (move to the next element that satisfies the predicate)
        FilterIterator& operator++() {
            ++current_;
            advance();
            return *this;
        }

        // Equality comparison
        bool operator!=(const FilterIterator& other) const {
            return current_ != other.current_;
        }

    private:
        Iterator current_, end_;
        Predicate pred_;

        // Advances current to the next element that satisfies the predicate
        void advance() {
            while (current_ != end_ && !pred_(*current_)) {
                ++current_;
            }
        }
};

// Helper function to create filtering iterators
template <typename Iterator, typename Predicate>
auto make_filter_iterator(Iterator begin, Iterator end, Predicate pred) {
    return FilterIterator<Iterator, Predicate>(begin, end, pred);
}
int main() {
    Node n;
    n.id = 10;
    n.deg = 20;
    Node n1, n2;
    n1.id = 20;
    n1.deg = 30;
    n2.id = 30;
    n2.deg = 40;
    unordered_map<int, list<Node>> m;
    vector<int> a = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vector<int> b;
    int r = 7;
    list<int> l;
//    ranges::sample(a.begin(),a.end(), back_inserter(l), 3, mt19937{random_device{}()});
    int exclude_value = 4;
    FilterIterator begin = make_filter_iterator(a.begin(), a.end(),
                         [exclude_value](int num) { return num != exclude_value; });
    std::uniform_int_distribution<> dist(0, a.size() - 1);
    std::random_device rd;
    std::mt19937 gen(rd());
    while (b.size() < 8) {
        int sample = a[dist(gen)];
        if (sample != exclude_value) {
            b.push_back(sample);
        }
    }
//    sample(a.begin(), a.begin()+3, back_inserter(b), 2, mt19937{random_device{}()});
//    sample(a.begin()+3, a.end(), back_inserter(b), r-2, mt19937{random_device{}()});
    for (auto i : b) {
        cout << i << " ";
    }
    return 0;
}
