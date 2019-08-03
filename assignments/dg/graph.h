#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <vector>
#include <map>
#include <memory>

class Node;

namespace gdwg {

    template <typename N, typename E>
    class Graph {
    public:

        class const_reverse_iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = std::tuple<N, N, E>;
            using reference = std::tuple<const N&, const N&, const E&>;
            using pointer = std::tuple<N*, N*, E*>;
            using difference_type = int;

            pointer operator->() const { return &(operator*()); }

            reference operator*() const;

            const_reverse_iterator& operator++();

            const const_reverse_iterator operator++(int);

            const_reverse_iterator& operator--();

            const const_reverse_iterator operator--(int);

            friend bool operator==(const const_reverse_iterator& lhs,
                                   const const_reverse_iterator rhs) {
                return (lhs.node_iter_ == rhs.node_iter_)
                       && (lhs.edge_iter_ == rhs.edge_iter_)
                       && (lhs.weight_iter_ == rhs.weight_iter_);
            }

            friend bool operator!=(const const_reverse_iterator& lhs,
                                   const const_reverse_iterator& rhs) {
                return !(lhs == rhs);
            }

        private:
            friend class Graph;

            std::vector<std::shared_ptr<Node>>::iterator node_iter_;
            const std::vector<std::shared_ptr<Node>>::iterator node_sentinel_;
            typename std::map<N, std::vector<E>>::iterator edge_iter_;
            typename std::vector<E>::iterator weight_iter_;

            const_reverse_iterator(const decltype(node_iter_)& node_iter,
                                   const decltype (node_sentinel_)& node_sentinel,
                                   const decltype (edge_iter_)& edge_iter,
                                   const decltype (weight_iter_) weight_iter):
                    node_iter_{node_iter},
                    node_sentinel_{node_sentinel},
                    edge_iter_{edge_iter},
                    weight_iter_{weight_iter} {}
        };

        class const_iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = std::tuple<N, N, E>;
            using reference = std::tuple<const N&, const N&, const E&>;
            using pointer = std::tuple<N*, N*, E*>;
            using difference_type = int;

            pointer operator->() const { return &(operator*()); }

            reference operator*() const;

            const_iterator& operator++();

            const const_iterator operator++(int);

            const_iterator& operator--();

            const const_iterator operator--(int);

            friend bool operator==(const const_iterator& lhs,
                                   const const_iterator rhs) {
                return (lhs.node_iter_ == rhs.node_iter_)
                       && (lhs.edge_iter_ == rhs.edge_iter_)
                       && (lhs.weight_iter_ == rhs.weight_iter_);
            }

            friend bool operator!=(const const_iterator& lhs,
                                   const const_iterator& rhs) {
                return !(lhs == rhs);
            }

        private:
            friend class Graph;

            std::vector<std::shared_ptr<Node>>::iterator node_iter_;
            const std::vector<std::shared_ptr<Node>>::iterator node_sentinel_;
            typename std::map<N, std::vector<E>>::iterator edge_iter_;
            typename std::vector<E>::iterator weight_iter_;

            const_iterator(const decltype(node_iter_)& node_iter,
                           const decltype (node_sentinel_)& node_sentinel,
                           const decltype (edge_iter_)& edge_iter,
                           const decltype (weight_iter_) weight_iter):
                    node_iter_{node_iter},
                    node_sentinel_{node_sentinel},
                    edge_iter_{edge_iter},
                    weight_iter_{weight_iter} {}
        };

        // Graph Constructors

        // TODO initializer list constructor

        Graph<N, E>(typename std::vector<N>::const_iterator c1,
                    typename std::vector<N>::const_iterator c2);

        Graph<N, E>(typename std::vector<std::tuple<N, N, E>>::const_iterator,
                    typename std::vector<std::tuple<N, N, E>>::const_iterator);

        Graph<N, E>(gdwg::Graph<N, E>& g);

        Graph<N, E>(gdwg::Graph<N, E>&& g);

        ~Graph<N, E>();

        Graph<N, E>& operator=(const gdwg::Graph<N, E>& g);

        Graph<N, E>& operator=(gdwg::Graph<N, E>&& g);

//        friend bool operator==(const Graph& g1, const Graph& g2) {
//            // TODO
//            return true;
//        }
//
//        friend bool operator!=(const Graph& g1, const Graph& g2) {
//            return !(g1 == g2);
//        }
//
//        friend std::ostream& operator<<(std::ostream& os, const Graph& g) {
//            // TODO
//        }

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

        const_iterator cbegin() const;

        const_iterator cend() const;

        const_reverse_iterator crbegin() const;

        const_reverse_iterator crend() const;

        const_iterator begin() const { return cbegin(); }

        const_iterator end() const { return cend(); }

        const_reverse_iterator rbegin() const { return crbegin(); }

        const_reverse_iterator rend() const { return crend(); }

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

            inline std::vector<std::weak_ptr<Node>> getChildren() {
                return children_;
            }

            inline std::vector<std::weak_ptr<Node>> getParents() {
                return parents_;
            }

            inline std::map<N, std::vector<E>> getEdges() { return edges_; }

            inline N getValue() {return value_;}

            inline void changeValue(N val) { value_ = val; }

            bool addEdge(const N&, const E&);

            void addChild(std::weak_ptr<Node>);

            void removeChild(const N&);

            void addParent(std::weak_ptr<Node>);

            void removeParent(const N&);
        };

        std::vector<std::shared_ptr<Node>> nodeList_;
//        std::map<std::weak_ptr<Node>, std::weak_ptr<Node>> dstList_;
//        std::map<std::tuple<std::weak_ptr<Node>, std::weak_ptr<Node>>, E> weightList_;
    };

}  // namespace gdwg

#endif  // ASSIGNMENTS_DG_GRAPH_H_
