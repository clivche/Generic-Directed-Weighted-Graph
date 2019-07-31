#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <vector>
#include <map>
#include <memory>

namespace gdwg {

    template <typename N, typename E>
    class Graph {
    public:
        class const_iterator {
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = std::tuple<N, N, E>;
            using reference = std::tuple<const N&, const N&, const E&>;
            using pointer = std::tuple<N*, N*, E*>;
            using difference_type = int;

            reference operator*() const;
            const_iterator& operator++() {
                // perform operation
                return *this;
            }
            const_iterator operator++(int) {
                auto copy{*this};
                ++(*this);
                return copy;
            }

            const_iterator& operator--() {
                // perform operation
                return *this;
            }
            const_iterator operator--(int) {
                auto copy{*this};
                --(*this);
                return copy;
            }

            friend bool operator==(const const_iterator& lhs, const const_iterator rhs) {
//                return it1.getValue() == rhs.outer_ && (lhs.outer_ == lhs.sentinel_ || lhs.inner_ == rhs.inner_);
            }

            friend bool operator!=(const const_iterator& lhs,
                    const const_iterator& rhs) {
                return !(lhs == rhs);
            }

          private:
            std::vector<std::string>::iterator outer_;
            const std::vector<std::string>::iterator sentinel_;
            std::string::iterator inner_;

            friend class Graph;
//            iterator(const decltype(outer_)& outer, const decltype(sentinel_)& sentinel, const decltype(inner_)& inner): outer_{outer}, sentinel_{sentinel}, inner_{inner} {}
        };

        // Graph Constructors
        Graph<N, E>(typename std::vector<N>::const_iterator c1, typename std::vector<N>::const_iterator c2);
        Graph<N, E>(typename std::vector<std::tuple<N, N, E>>::const_iterator, typename std::vector<std::tuple<N, N, E>>::const_iterator);

        bool InsertEdge(const N&, const N&, const E&);
        bool InsertNode(const N&);
        bool DeleteNode(const N&);
        bool Replace(const N&, const N&);
        void MergeReplace(const N& oldData, const N& newData);
        void Clear();
        bool IsNode(const N& val);
        bool IsConnected(const N& src, const N& dst);
        std::vector<N> GetNodes();
        std::vector<N> GetConnected(const N& src);
        std::vector<E> GetWeights(const N& src, const N& dst);
        const_iterator find(const N&, const N&, const E&);
        bool erase(const N& src, const N& dst, const E& w);
        const_iterator erase(const_iterator it);


    private:
        class Node {
        private:
            N value_;
            std::vector<std::weak_ptr<Node>> children_;
            std::vector<std::weak_ptr<Node>> parents_;
            std::map<N, std::vector<E>> edges_;

        public:
            Node();
            Node(N value) : value_(value) {}
            inline std::vector<std::weak_ptr<Node>> getChildren() {return children_;}
            inline std::vector<std::weak_ptr<Node>> getParents() {return parents_;}
            inline std::map<N, std::vector<E>> getEdges() {return edges_;}
            inline N getValue() {return value_;}
            inline void changeValue(N val) {value_ = val; return;}
            bool addEdge(const N&, const E&);
            void addChild(std::weak_ptr<Node>);
            void removeChild(const N&);
            void addParent(std::weak_ptr<Node>);
            void removeParent(const N&);
        };

        std::vector<std::shared_ptr<Node>> nodeList_;
        std::map<std::weak_ptr<Node>, std::weak_ptr<Node>> dstList_;
        std::map<std::tuple<std::weak_ptr<Node>, std::weak_ptr<Node>>, E> weightList_;
    };

}  // namespace gdwg


#include "assignments/dg/graph.tpp"


#endif  // ASSIGNMENTS_DG_GRAPH_H_
