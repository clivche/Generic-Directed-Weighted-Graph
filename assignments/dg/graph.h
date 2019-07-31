#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <vector>
#include <map>
#include <memory>

class Node;
class Edge;

namespace gdwg {

    template <typename N, typename E>
    class Graph {
    public:
        class const_iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = std::tuple<N, N, E>;
            using reference = std::tuple<const N&, const N&, const E&>;
            using pointer = std::tuple<N*, N*, E*>;
            using difference_type = int;

            // This one isn't strictly required, but it's nice to have.
            pointer operator->() const { return &(operator*()); }
            reference operator*() const;
            const_iterator& operator++();
            const const_iterator operator++(int) {
                auto copy{*this};
                ++(*this);
                return copy;
            }
            const_iterator& operator--();
            const const_iterator operator--(int) {
                auto copy{*this};
                --(*this);
                return copy;
            }

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

            std::vector<std::shared_ptr<Node>>::const_iterator node_iter_;
            const std::vector<std::shared_ptr<Node>>::const_iterator node_sentinel_;
            typename std::map<N, std::vector<E>>::const_iterator edge_iter_;
            typename std::vector<E>::const_iterator weight_iter_;

            const_iterator(const decltype(node_iter_)& node_iter,
                    const decltype (node_sentinel_)& node_sentinel,
                    const decltype (edge_iter_)& edge_iter,
                    const decltype (weight_iter_) weight_iter):
                    node_iter_{node_iter},
                    node_sentinel_{node_sentinel},
                    edge_iter_{edge_iter},
                    weight_iter_{weight_iter} {}
        };

        const_iterator begin();
        const_iterator end();
        const_iterator rbegin();
        const_iterator rend();

        // Vector Constructor
        Graph<N, E>();
        Graph<N, E>(typename std::vector<N>::const_iterator c1, typename std::vector<N>::const_iterator c2);
        Graph<N, E>(typename std::vector<std::tuple<N, N, E>>::const_iterator, typename std::vector<std::tuple<N, N, E>>::const_iterator);

        bool InsertEdge(const N&, const N&, const E&);
        bool InsertNode(const N&);
        bool DeleteNode(const N&);

    private:
        class Node {
        private:
            N value_;
//            std::vector<Node*> parents_;
            std::vector<std::weak_ptr<Node>> children_;
            std::map<std::weak_ptr<N>, std::vector<E>> edges_;

        public:
            Node();
            Node(N value) : value_(value) {}
            inline std::vector<std::weak_ptr<Node>> getChildren() {return children_;}
            inline std::map<std::weak_ptr<N>, std::vector<E>> getEdges() {return edges_;}
            inline N getValue() {return value_;}
            bool addEdge(std::weak_ptr<Node>, const E&);
        };

        class Edge {
        private:
            Node* src_;
            Node* dst_;
            E weight_;
        public:
            Edge(N src, N dst, E weight) {
                src_(std::weak_ptr<Node>(new Node(src))), dst_(std::weak_ptr<Node>(new Node(dst))), weight_(weight);
            }
            Node* getSrc() {return src_;}
            Node* getDst() {return dst_;}
            E getWeight() {return weight_;}
        };

        std::vector<std::shared_ptr<Node>> nodeList_;
        std::vector<std::shared_ptr<Edge>> edgeList_;
    };

}  // namespace gdwg


#include "assignments/dg/graph.tpp"

//template<typename N, typename E>
//gdwg::Graph<N, E>(typename std::vector<N>::const_iterator c1, typename std::vector<N>::const_iterator c2) {  }

template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator& gdwg::Graph<N, E>::const_iterator::operator++() {
    ++weight_iter_;

    if (weight_iter_ == *edge_iter_->end()) {
        ++edge_iter_;

        if (edge_iter_ == (*node_iter_)->end()) {

            do {
                ++node_iter_;
            } while (node_iter_ != node_sentinel_
                     && (*node_iter_)->begin() == (*node_iter_)->end());

            if (node_iter_ != node_sentinel_) {
                edge_iter_ = (*node_iter_)->begin();
                weight_iter_ = edge_iter_->begin();
            }
        }
    }
}

template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator& gdwg::Graph<N, E>::const_iterator::operator--() {

    if (weight_iter_ == edge_iter_->begin()) {

        if (edge_iter_ == (*node_iter_)->begin()) {

            do {
                --node_iter_;
            } while (node_iter_ != node_sentinel_
                     && (*node_iter_)->begin() == (*node_iter_)->end());

            if (node_iter_ != node_sentinel_) {
                edge_iter_ = (*node_iter_)->end();
                --edge_iter_;

                weight_iter_ = edge_iter_->end();
                --weight_iter_;
            }

        } else {
            --edge_iter_;

            weight_iter_ = edge_iter_->end();
            --weight_iter_;
        }

    } else {
        --weight_iter_;
    }

}

//template<typename N, typename E>
//typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::const_iterator::operator++(int) {
//    auto copy{*this};
//    ++(*this);
//    return copy;
//}
//
//template<typename N, typename E>
//typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::const_iterator::operator--(int) {
//    auto copy{*this};
//    --(*this);
//    return copy;
//}

template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator::reference
gdwg::Graph<N, E>::const_iterator::operator*() const {
    return {(*node_iter_).getValue(), *edge_iter_, *weight_iter_};
}

template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::begin() {
    typename std::vector<std::shared_ptr<Node>>::iterator it = nodeList_.begin();

    while (*it.getChildren().size() == 0) {
        ++it;
    }

    if (it == nodeList_.end()) {
        return end();
    }

    std::map<N, std::vector<E>> children = (*it).getChildren();
    std::vector<E> weights = children[0];
    return {it, nodeList_.end(), children.begin(), weights.begin()};
}

template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::end() {
    return {nodeList_.end(), nodeList_.end(), {}, {}};
}

template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::rbegin() {
    typename std::vector<std::shared_ptr<Node>>::const_iterator it = nodeList_
            .rbegin();

    while (*it.getChildren().size() == 0) {
        ++it;
    }

    if (it == nodeList_.rend()) {
        return rend();
    }

    std::map<N, std::vector<E>> children = *it.getChildren();
    std::vector<E> weights = children[children.size()-1];
    return {it, nodeList_.rend(), children.rbegin(), weights.rbegin()};
}

template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::rend() {
    return {nodeList_.rend(), nodeList_.rend(), {}, {}};
}

#endif  // ASSIGNMENTS_DG_GRAPH_H_
