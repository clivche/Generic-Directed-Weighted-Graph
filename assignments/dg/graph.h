#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <vector>
#include <map>
#include <memory>

namespace gdwg {

    template <typename N, typename E>
    class Graph {

    public:
        class Node;

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
                typename std::vector<std::shared_ptr<Node>>::iterator node_iter_;
                const typename std::vector<std::shared_ptr<Node>>::iterator node_sentinel_;
                const typename std::vector<std::shared_ptr<Node>>::iterator reverse_sentinel_;
                typename std::vector<N>::iterator edge_iter_;
                typename std::vector<E>::iterator weight_iter_;

                const_reverse_iterator(const decltype(node_iter_)& node_iter,
                                       const decltype (node_sentinel_)& node_sentinel,
                                       const decltype (reverse_sentinel_)& reverse_sentinel_,
                                       const decltype (edge_iter_)& edge_iter,
                                       const decltype (weight_iter_) weight_iter):
                        node_iter_{node_iter},
                        node_sentinel_{node_sentinel},
                        reverse_sentinel_{reverse_sentinel_},
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

                typename std::vector<std::shared_ptr<Node>>::iterator node_iter_;
                const typename std::vector<std::shared_ptr<Node>>::iterator node_sentinel_; //end
                const typename std::vector<std::shared_ptr<Node>>::iterator reverse_sentinel_; //begin
                typename std::vector<N>::iterator edge_iter_;
                typename std::vector<E>::iterator weight_iter_;

                const_iterator(const decltype(node_iter_)& node_iter,
                                 const decltype (node_sentinel_)& node_sentinel,
                                 const decltype (reverse_sentinel_)& reverse_sentinel_,
                                 const decltype (edge_iter_)& edge_iter,
                                 const decltype (weight_iter_)& weight_iter):
                      node_iter_{node_iter},
                      node_sentinel_{node_sentinel},
                      reverse_sentinel_{reverse_sentinel_},
                      edge_iter_{edge_iter},
                      weight_iter_{weight_iter} {}
        };

        // Graph Constructors

        // TODO initializer list constructor

        Graph<N, E>() {};

        Graph<N, E>(typename std::vector<N>::const_iterator c1,
                    typename std::vector<N>::const_iterator c2);

        Graph<N, E>(typename std::vector<std::tuple<N, N, E>>::const_iterator,
                    typename std::vector<std::tuple<N, N, E>>::const_iterator);

        Graph<N, E>(std::initializer_list<N>);

        Graph<N, E>(gdwg::Graph<N, E>& g);

        Graph<N, E>(gdwg::Graph<N, E>&& g);

        ~Graph<N, E>();

        Graph<N, E>& operator=(const gdwg::Graph<N, E>& g);

        Graph<N, E>& operator=(gdwg::Graph<N, E>&& g);

        friend bool operator==(const Graph& g1, const Graph& g2) {

            // check nodelist size
            if (g1.nodeList_.size() != g2.nodeList_.size()) {
                return false;

            }

            int max = g1.nodeList_.size();
            for (int counter = 0; counter < max; counter++) {
                // check node values
                if (g1.nodeList_[counter]->getValue() != g2
                .nodeList_[counter]->getValue()) {
                    return false;
                }
            }


            for (int counter = 0; counter < max; counter++) {
                std::map<N, std::vector<E>> e1 = *g1.nodeList_[counter]
                        .getEdges();
                std::map<N, std::vector<E>> e2 = *g2.nodeList_[counter]
                        .getEdges();

                // check size of children vector
                if (e1.size() != e2.size()) {
                    return false;
                }

                // check if child values are all the same
                for (int cc = 0; cc < e1.size(); cc++) {
                    std::pair<N, std::vector<E>> child1 = *e1[cc];
                    std::pair<N, std::vector<E>> child2 = *e2[cc];

                    if (child1.first != child2.first) {
                        return false;
                    }
                }

                // check if edges are all the same weight
                for (int cc = 0; cc < e1.size(); cc++) {
                    std::pair<N, std::vector<E>> child1 = *e1[cc];
                    std::pair<N, std::vector<E>> child2 = *e2[cc];

                    std::vector<E> ce1 = child1.second;
                    std::vector<E> ce2 = child2.second;

                    if (ce1 != ce2) {
                        return false;
                    }
                }
            }

            return true;
        }

        friend bool operator!=(const Graph& g1, const Graph& g2) {
            return !(g1 == g2);
        }

        friend std::ostream& operator<<(std::ostream& os, const Graph& g) {
            const std::string NODE_START = " (";
            const std::string NODE_END = "\n)\n";
            const std::string EDGE_SEPARATOR = " | ";
            const std::string CHILD_START = "\n  ";

            int max = g.nodeList_.size();
            std::vector<N> output;

            for (int counter = 0; counter < max; counter++) {
                Node n = *g.nodeList_[counter];
                os << n.getValue() << NODE_START;

                std::vector<std::weak_ptr<Node>> children = n.getChildren();
                std::map<N, std::vector<E>> edge_map= n.getEdges();
                std::vector<N> avail_dst;

                for (auto it = children.begin(); it != children.end(); ++it) {
                    if (auto childLock = it->lock()) {
                        avail_dst.push_back(childLock->getValue());
                        std::cout<<childLock->getValue();
                    }
                }


//                for (const auto& item : edge_map) {
//                    std::cout<< item.first << "\n";
//                }
                for (auto dst = avail_dst.begin(); dst != avail_dst.end(); ++dst) {
                    typename std::vector<E> weights = edge_map[*dst];

                    for (auto it = weights.begin(); it != weights.end(); ++it) {
                        os << CHILD_START << *dst << EDGE_SEPARATOR << *it;
                    }
                }

                os << NODE_END;
            }
            return os;
        }


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

        inline const_iterator begin() const { return cbegin(); }

        inline const_iterator end() const { return cend(); }

        inline const_reverse_iterator rbegin() const { return crbegin(); }

        inline const_reverse_iterator rend() const { return crend(); }

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

        std::vector<N> dummyN;
        std::vector<E> dummyE;

    private:
        std::vector<std::shared_ptr<Node>> nodeList_;

    };

}  // namespace gdwg


#endif  // ASSIGNMENTS_DG_GRAPH_H_
