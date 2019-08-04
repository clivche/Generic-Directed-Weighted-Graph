#include "assignments/dg/graph.h"
#include <stdexcept>

template<typename X, typename Predicate>
std::vector<X> mergeSorted(std::vector<X> v1, std::vector<X> v2,
                           Predicate f = [](X x) { return x; }) {
    auto &it1 = v1.begin();
    auto &it2 = v2.begin();
    std::vector<X> result;
    while (it1 != v1.end() || it2 != v2.end()) {
        if (it1 == v1.end()) {
            result.push(*it2++);
        }
        if (it2 == v2.end()) {
            result.push(*it1++);
        }
        if (f(*it1) < f(*it2)) {
            result.push(*it1++);
        } else if (f(*it2) < f(*it1)) {
            result.push(*it2++);
        } else {
            result.push(*it1++);
            it2++;
        }
    }
    return result;
}


// Node Functions

/**
 * Remove child of node
 *
 * @param n - node to be removed
 */
template<typename N, typename E>
void gdwg::Graph<N, E>::Node::removeChild(const N &n) {
    auto &children = getChildren();
    for (const auto &child : children) {
        if (auto &childShared = child->lock()) {
            if (childShared->getValue() == n) {
                children->erase(child);
                break;
            }
        }
    }
    return;
}

/**
 * Remove the parent of the node
 *
 * @param n - node to be removed
 */
template<typename N, typename E>
void gdwg::Graph<N, E>::Node::removeParent(const N &n) {
    auto &parents = getParents();
    for (const auto &parent : parents) {
        if (auto &parentShared = parent->lock()) {
            if (parentShared->getValue() == n) {
                parents->erase(parent);
                break;
            }
        }
    }
    return;
}

/**
 * Add node as child of another node
 *
 * @param dst - destination node to be added
 */
template<typename N, typename E>
void gdwg::Graph<N, E>::Node::addChild(std::weak_ptr<Node> dst) {

    auto nextChild = children_.begin();
    bool found = false;
    for (auto it = children_.begin(); it != children_.end(); it++) {
        if (auto child = it->lock()) {
            if (dst->getValue() < child->getValue()) {
                nextChild = it;
            } else if (dst->getValue() == child->getValue()) {
                found = true;
                break;
            }
        }
    }

    if (!found) {
        children_.insert(nextChild, dst);
    }
    return;
}

/**
 * Add destination and edge to the node's edges_ list
 *
 * @param dst - destination node
 * @param w - weight of edge
 */
template<typename N, typename E>
bool gdwg::Graph<N, E>::Node::addEdge(const N &dst, const E &w) {
    auto nextEdge = edges_[dst].begin();
    for (auto it = edges_[dst].begin(); it != edges_[dst].end(); it++) {
        if (w < *it) {
            nextEdge = it;
        } else if (w == *it) {
            return false;
        }
    }
    edges_[dst].insert(nextEdge, w);
    return true;
}

/**
 * Add node as a parent of another
 *
 * @param src - node to be added
 */
template<typename N, typename E>
void gdwg::Graph<N, E>::Node::addParent(std::weak_ptr<Node> src) {

    auto nextParent = parents_.begin();
    bool found = false;
    for (auto it = parents_.begin(); it != parents_.end(); it++) {
        if (auto parent = it->lock()) {
            if (src->getValue() < parent->getValue()) {
                nextParent = it;
            } else if (src->getValue() == parent->getValue()) {
                found = true;
                break;
            }
        }
    }

    if (!found) {
        parents_.insert(nextParent, std::weak_ptr<Node>(src));
    }
    return;
}

// Graph Functions
/**
 * Constructor
 * Takes the start and end of a const_iterator to a std:vector<N> and adds those
 * nodes to the graph.
 * @param c1 - beginning of vector of node values
 * @param c2 - end of vector of node values
 */
template<typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<N>::const_iterator c1,
                         typename std::vector<N>::const_iterator c2) {
    sort(c1,c2);
    for (auto &it = c1; it != c2; it++) {
        // Check for duplicates in initaliser vector
        bool alreadyInList = false;
        for (const auto &item : nodeList_) {
            if (item->getValue() == *it) {
                alreadyInList = true;
                break;
            }
        }
        if (!alreadyInList) {
            nodeList_.push(std::shared_ptr<Node>(new Node(*it)));
        }
    }
}

template<typename N, typename E>
bool sortTuple(const std::tuple<N,N,E>& a,
               const std::tuple<N,N,E>& b)
{
    if (std::get<1>(a) != std::get<1>(b)) {
        return (std::get<1>(a) == std::get<1>(b));
    }
    else {
        if (std::get<2>(a) != std::get<2>(b)) {
            return (std::get<2>(a) < std::get<2>(b));
        }
        else {
            return (std::get<3>(a) < std::get<3>(b));
        }
    }
}

/**
 * Constructor
 * Iterators over tuples of (source node, destination node, edge weight) and
 * adds them to the graph.
 *
 * @param c1 - beginning of vector of tuples
 * @param c2 - end of over vector of tuples
 */
template<typename N, typename E>
gdwg::Graph<N, E>::Graph(
        typename std::vector<std::tuple<N, N, E>>::const_iterator c1,
        typename std::vector<std::tuple<N, N, E>>::const_iterator c2) {
    sort(c1,c2, sortTuple);
    for (auto &it = c1; it != c2; it++) {
        // Check for duplicates in initaliser vector
        bool alreadyInList = false;
        for (auto i = 0; i < 2; i++) {
            for (const auto &node : nodeList_) {
                if (node->getValue() == *it[i]) {
                    alreadyInList = true;
                    break;
                }
            }
            if (!alreadyInList) {
                nodeList_.push(std::shared_ptr<Node>(new Node(*it)));
            }
        }
        InsertEdge(it[0], it[1], it[2]);
    }
}


template<typename N, typename E>
gdwg::Graph<N, E>::Graph(std::initializer_list<N> args) {
    sort(args.begin(), args.end());
    for (auto &it = args.begin(); it != args.begin(); it++) {
        // Check for duplicates in initaliser vector
        bool alreadyInList = false;
        for (const auto &item : nodeList_) {
            if (item->getValue() == *it) {
                alreadyInList = true;
                break;
            }
        }
        if (!alreadyInList) {
            nodeList_.push(std::shared_ptr<Node>(new Node(*it)));
        }
    }
}



/**
 * Copy Constructor
 */
template<typename N, typename E>
gdwg::Graph<N, E>::Graph(gdwg::Graph<N, E>& g) {
    // Initialise copied graph with nodeList_
    for (auto& it = g->GetNodes().begin(); it != g->GetNodes().end(); it++) {
        this->InsertNode(*it);
    }
    // Copy every edge into graph
    for (auto& it = g.begin(); it != g.end(); g++) {
        this->InsertEdge(*it);
    }
}

/**
 * Move Constructor
 */
template<typename N, typename E>
gdwg::Graph<N, E>::Graph(gdwg::Graph<N, E>&& g) {
    this->nodeList_ = std::move(g.nodeList_);
}

/**
 * Destructor
 */
template<typename N, typename E>
gdwg::Graph<N, E>::~Graph() {
//    for (auto& node : nodeList_) {
//        delete node;
//    }
//    delete[] nodeList_;
}

/**
 * A copy assignment operator overload
 */
template<typename N, typename E>
gdwg::Graph<N, E> &gdwg::Graph<N, E>::operator=(const gdwg::Graph<N, E>& g) {
    if (&g == *this) {
      return *this;
    }
    // Initialise copied graph with nodeList_
    for (auto& it = g->GetNodes().begin(); it != g->GetNodes().end(); it++) {
        this->InsertNode(*it);
    }
    // Copy every edge into graph
    for (auto& it = g.begin(); it != g.end(); g++) {
        this->InsertEdge(*it);
    }
    return *this;
}

/**
 * A move assignment operator overload
 */
template<typename N, typename E>
gdwg::Graph<N, E> &gdwg::Graph<N, E>::operator=(gdwg::Graph<N, E>&& g) {
    if (&g == *this) {
        return *this;
    }
    this->nodeList_ = std::move(g.nodeList_);
    return *this;
}

/**
 * Adds a new node with value val to the graph. This function returns true if
 * the node is added to the graph and false if there is already a node
 * containing val in the graph (with the graph unchanged).
 *
 * @param n - node to be inserted
 */
template<typename N, typename E>
bool gdwg::Graph<N, E>::InsertNode(const N &n) {
    // Insert node at beginning by default
    auto next = nodeList_.begin();
    for (auto it = nodeList_.begin(); it != nodeList_.end(); it++) {
        // Insert node before every value it is less than
        if (n < (*it)->getValue()) {
            next = it;
        }
        if (n == (*it)->getValue()) {
            return false;
        }
    }
    nodeList_.insert(next, std::shared_ptr<Node>(new Node(n)));
    return true;
}

/**
 * Adds a new edge src → dst with weight w. This function returns true if the
 * edge is added and false if the edge src → dst with weight w already exists
 * in the graph. Note: Nodes are allowed to be connected to themselves.
 *
 * @param src - source node
 * @param dst - destination node
 * @param w - weight of edge
 */
template<typename N, typename E>
bool gdwg::Graph<N, E>::InsertEdge(const N &src, const N &dst, const E &w) {

    // Insert nodes into list (will do nothing if already inserted)
    InsertNode(src);
    InsertNode(dst);

    // Find pointers to src and dst nodes
    std::shared_ptr<Node> srcNode;
    std::shared_ptr<Node> dstNode;
    for (const auto &node : nodeList_) {
        if (node->getValue() == src) {
            srcNode = node;
        }
        if (node->getValue() == dst) {
            dstNode = node;
        }
    }

    // Check if an edge exists
    if (srcNode->addEdge(dst, w)) {
        srcNode->addChild(dstNode);
        dstNode->addParent(srcNode);
        return true;
    }
    return false;
}

/**
 * Deletes a given node and all its associated incoming and outgoing edges.
 * This function does nothing if the node that is to be deleted does not
 * exist in the graph.
 *
 * @param n - node to be deleted
 */
template<typename N, typename E>
bool gdwg::Graph<N, E>::DeleteNode(const N &n) {
    for (const auto &node : nodeList_) {
        if (node->getValue() == n) {
            nodeList_.erase(node);
            return true;
        }
    }
    return false;
}

/**
 * Replaces the original data, oldData, stored at this particular node by the
 * replacement data, newData. If an exception is not thrown, this function
 * returns false if a node that contains value newData already exists in the
 * graph (with the graph unchanged) and true otherwise.
 *
 * @param oldData - to be replaced
 * @param newData - replaced with
 */
template<typename N, typename E>
bool gdwg::Graph<N, E>::Replace(const N &oldData, const N &newData) {

    auto &old = nodeList_.end();
    for (const auto &node : nodeList_) {
        if (node->getValue() == newData) {
            return false;
        }
        if (node->getValue() == oldData) {
            old = node;
        }
    }

    if (old != nodeList_.end()) {
        old->changeValue(newData);
        for (const auto &parent : old->getParents()) {
            if (auto &sharedParent = parent->lock()) {
                auto &edges = sharedParent->getEdges();
                edges[newData] = edges[oldData];
                edges[oldData]->clear();
            }
        }
        return true;
    }
    throw std::runtime_error(
            "Cannot call Graph::Replace on a node that doesn't exist");
}

/**
 * All instances of node oldData in the graph are replaced with instances of
 * newData. After completing, every incoming and outgoing edge of oldData
 * becomes an incoming/outgoing edge of newData, except that duplicate edges
 * must be removed.
 *
 * @param oldData - to be replaced
 * @param newData - replaced with
 */
template<typename N, typename E>
void gdwg::Graph<N, E>::MergeReplace(const N &oldData, const N &newData) {

    // Find shared_ptr to nodes within nodeList_
    auto &oldNode = nodeList_.end();
    auto &newNode = nodeList_.end();
    for (const auto &node : nodeList_) {
        if (node->getValue() == newData) {
            newNode = node;
        }
        if (node->getValue() == oldData) {
            oldNode = node;
        }
    }

    // Exception Handling
    if (oldNode == nodeList_.end() || newNode == nodeList_.end()) {
        throw std::runtime_error(
                "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
    }

    // Handle Parents
    for (const auto &parent : oldNode->getParents()) {
        if (const auto &parentShared = parent->lock()) {

            // Alter parent->children_
            //     Add child
            parentShared->addChild(std::weak_ptr<Node>(*newNode));
            //     Remove child
            parentShared->removeChild(oldData);

            // Merge/Replace parent->edges_
            auto &pEdge = parentShared->getEdges();
            pEdge[newData] = mergeSorted(pEdge[oldData], pEdge[newData]);
            pEdge[oldData]->clear();

            // Add parent to newNode
            newNode->addParent(parent);
        }
    }

    // Handle Children
    for (const auto &child : oldNode->getChildren()) {
        if (const auto &childShared = child.lock()) {
            // Alter child->parent_
            //     Remove parent
            childShared->removeParent(oldData);
            //     Add parent
            childShared->addParent(std::weak_ptr<Node>(*newNode));

            // Add child to newNode
            newNode->addChild(child);
        }
    }

    // Handle Edges
    auto newEdges = newNode->getEdges();
    auto oldEdges = oldNode->getEdges();
    for (const auto&[key, val] : oldEdges) {
        newEdges[key] = mergeSorted(newEdges[key], val);
    }

    nodeList_.erase(oldNode);
}

/**
 * Remove all nodes and edges from the graph. New nodes or edges can be added
 * to the graph afterwards.
 */
template<typename N, typename E>
void gdwg::Graph<N, E>::Clear() {
    nodeList_.clear();
}

/**
 * Returns true if a node with value val exists in the graph and false
 * otherwise.
 * @param val - value of potential node
 */
template<typename N, typename E>
bool gdwg::Graph<N, E>::IsNode(const N &val) {
    for (const auto &node : nodeList_) {
        if (node->getValue() == val) {
            return true;
        }
    }
    return false;
}

/**
 * Returns true if the edge src → dst exists in the graph and false otherwise.
 *
 * @param src - source node
 * @param dst - destination node
 * @return
 */
template<typename N, typename E>
bool gdwg::Graph<N, E>::IsConnected(const N &src, const N &dst) {
    auto srcNode = nodeList_.end();
    bool dstFound = false;
    for (auto it = nodeList_.begin(); it != nodeList_.end(); it++) {
        if (it->getValue() == src) {
            srcNode = it;
        }
        if (it->getValue() == dst) {
            dstFound = true;
        }
    }
    if (srcNode == nodeList_.end() || !dstFound) {
        throw std::runtime_error(
                "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
    }

    // Check srcNode edge list
    auto edges = srcNode->getEdges();
    if (!edges[dst].empty) {
        return true;
    }
    return false;
}

/**
 * Returns a vector of all nodes in the graph. Sorted by increasing order of
 * node.
 */
template<typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetNodes() {
    std::vector<N> res;
    for (const auto &node : nodeList_) {
        res.push_back(node->getValue());
    }
    return res;
}

/**
 * Returns a vector of the nodes (found from any immediate outgoing edge)
 * connected to the src node passed in. Sorted by increasing order of node
 * (of those nodes that are connected).
 *
 * @param src - source node
 */
template<typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetConnected(const N &src) {
    std::vector<N> res;
    for (auto it = nodeList_.begin(); it != nodeList_.end(); it++) {
        if (it->getValue() == src) {
            for (const auto &dst : it->getChildren()) {
                if (auto dstShared = dst->lock()) {
                    res.push_back(dstShared.getValue());
                }
            }
            return res;
        }
    }
    throw std::runtime_error(
            "Cannot call Graph::GetConnected if src doesn't exist in the graph");
}

/**
 * Returns a vector of the weights of edges between two nodes. Sorted by
 * increasing order of edge. These edges are all outgoing edges of src toward
 * dst.
 *
 * @param src - source node
 * @param dst - destination node
 * @return
 */
template<typename N, typename E>
std::vector<E> gdwg::Graph<N, E>::GetWeights(const N &src, const N &dst) {

    auto srcNode = nodeList_.end();
    bool dstFound = false;
    for (auto it = nodeList_.begin(); it != nodeList_.end(); it++) {
        if (it->getValue() == src) {
            srcNode = it;
        }
        if (it->getValue() == dst) {
            dstFound = true;
        }
    }
    if (srcNode == nodeList_.end() || !dstFound) {
        throw std::runtime_error(
                "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
    }

    // return src-dst edge list
    auto srcEdges = srcNode->getEdges();
    return srcEdges[dst];

}

/**
 * Returns an iterator to the found node in the graph. If the edge is not
 * found the equivalent value of gdwg::Graph<N, E>::cend() is returned.
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator
gdwg::Graph<N, E>::find(const N &, const N &, const E &) {
    // TODO
    return gdwg::Graph<N, E>::const_iterator();
}

/**
 * Deletes an edge from src to dst with weight w, only if the edge exists in
 * the graph. This function returns an boolean as to whether the item has
 * been removed or not (true if removed). If the edge is not in the graph it
 * returns false.
 *
 * @param src - source node
 * @param dst - destination node
 * @param w - weight of edge
 */
template<typename N, typename E>
bool gdwg::Graph<N, E>::erase(const N &src, const N &dst, const E &w) {

    for (auto it = nodeList_.begin(); it != nodeList_.end(); it++) {
        if (it->getValue() == src) {
            auto edges = it->getEdges();
            for (auto edge = edges[dst].begin();
                 edge != edges[dst].end(); edge++) {
                if (*edge == w) {
                    edges[dst].erase(edge);
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

/**
 * This function removes the edge at the location the iterator points to.
 * This function returns an iterator to the element AFTER the one that has
 * been removed. If no erase can be made, the equivalent of gdwg::Graph<N,
 * E>::end() is returned.
 *
 * @param iterator
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::erase
        (gdwg::Graph<N, E>::const_iterator it) {
    it;
    // TODO
    return gdwg::Graph<N, E>::const_iterator();
}

// const_iterator

/**
 * Pre-increment Operator overload for const_iterator
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator &gdwg::Graph<N,
        E>::const_iterator::operator++() {
    ++weight_iter_;

//    std::vector<std::weak_ptr<Node>> children = (*node_iter_)->getChildren();
//    std::map<N, std::vector<E>> edge_map = (*node_iter_)->getEdges();
//    std::vector<E> edge_vector = edge_map[*edge_iter_.first];

    std::vector<N> children;
    for (const auto& child : (*node_iter_)->getChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->getValue());
    }
    std::map<N, std::vector<E>> edge_map = (*node_iter_)->getEdges();
    std::vector<E> weights = edge_map[*edge_iter_];

    if (weight_iter_ == weights.end()) {
        ++edge_iter_;

        if (edge_iter_ == children.end()) {

            do {
                ++node_iter_;
            } while (node_iter_ != node_sentinel_
                    && children.begin() == children.end());

            if (node_iter_ != node_sentinel_) {
                edge_iter_ = children.begin();

                std::map<N, std::vector<E>> edges = (*node_iter_)->getEdges();
                weights = edges[*edge_iter_];
                weight_iter_ = weights.begin();
            }
        }
    }

    return *this;
}

/**
 * Pre-decrement Operator overload for const_iterator
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator &gdwg::Graph<N,
        E>::const_iterator::operator--() {

    std::vector<N> children;
    for (const auto& child : (*node_iter_)->getChildren()) {
        children.push_back(child->getValue());
    }
    std::map<N, std::vector<E>> edge_map = (*node_iter_)->getEdges();
    std::vector<E> weights = edge_map[*edge_iter_];

    if (weight_iter_ == weights.begin()) {

        if (edge_iter_ == children.begin()) {

            do {
                --node_iter_;
            } while (node_iter_ != reverse_sentinel_
                     && children.begin() == children.end());


            if (children.begin() != children.end()) {

                children = (*node_iter_)->getChildren();
                edge_map = (*node_iter_)->getEdges();

                edge_iter_ = children.end();
                --edge_iter_;

                weight_iter_ = edge_map[*edge_iter_].end();
                --weight_iter_;

            } else if (node_iter_ == reverse_sentinel_) {
                throw std::runtime_error(
                    "Cannot cannot decrement past begin().");
            }

        } else {
            --edge_iter_;

            weight_iter_ = edge_map[*edge_iter_].end();
            --weight_iter_;
        }

    } else {
        --weight_iter_;
    }

    return *this;
}

/**
 * Post-increment Operator overload for const_iterator
 */
template<typename N, typename E>
const typename gdwg::Graph<N, E>::const_iterator
gdwg::Graph<N, E>::const_iterator::operator++(int) {
    auto copy{*this};
    ++(*this);
    return copy;
}

/**
 * Post-decrement Operator overload for const_iterator
 */
template<typename N, typename E>
const typename gdwg::Graph<N, E>::const_iterator
gdwg::Graph<N, E>::const_iterator::operator--(int) {
    auto copy{*this};
    --(*this);
    return copy;
}

/**
 * * Operator overload for const_iterator
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator::reference
gdwg::Graph<N, E>::const_iterator::operator*() const {
    return {(*node_iter_)->getValue(), *edge_iter_, *weight_iter_};
}

/**
 * Returns a const_iterator pointing to the first in the container. A
 * const_iterator is an iterator that points to const content. This iterator
 * can be increased and decreased (unless it is itself also const), but it
 * cannot be used to modify the contents it points to, even if the content it
 * points to is not itself const.
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cbegin() const {

    if (nodeList_.begin() == nodeList_.end()) {
        return end();
    }

    auto it = nodeList_.begin();

    // find a node that has children
    while ((*it)->getChildren().size() == 0) {
        ++it;
        if (it == nodeList_.end()) {
            return end();
        }
    }

    std::vector<N> children;
    for (const auto& child : (*it)->getChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->getValue());
    }
    std::map<N, std::vector<E>> edges = (*it)->getEdges();
    std::vector<E> weights = edges.begin()->second;

    return {it, nodeList_.end(), nodeList_.begin(), children.begin(), weights
    .begin()};

    //todo what happens when key maps to emoty vector
}

/**
 * Returns a const_iterator pointing to the past-the-end element in the
 * container. A const_iterator is an iterator that points to const content.
 * This iterator can be increased and decreased (unless it is itself also
 * const), but it cannot be used to modify the contents it points to, even if
 * the content it points to is not itself const.
 * If the container is empty, this function returns the same as
 * vector::cbegin. The value returned shall not be dereferenced.
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cend() const {

    if (nodeList_.size() == 0) {
        //todo exit early
    }

    auto it = nodeList_.end();

    // find a node that has children
    while ((*it)->getChildren().size() == 0) {
        --it;
    }

    std::vector<N> children;
    for (const auto& child : (*it)->getChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->getValue());
    }
    std::map<N, std::vector<E>> edges = nodeList_.begin()->getEdges();
    std::vector<E> last_weights = edges.rbegin()->second;

    return {nodeList_.end(), nodeList_.end(), nodeList_.begin(), children
    .end(), last_weights.end()};
//
//    return {nodeList_.end(), nodeList_.end(), nodeList_.begin(), dummyN.end()
//            , dummyE.end()};
}


// const_reverse_iterator
/**
 * Pre-increment Operator overload for const_reverse_iterator
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator &gdwg::Graph<N,
        E>::const_reverse_iterator::operator++() {

    ++weight_iter_;

    std::vector<N> children;
    for (const auto& child : (*node_iter_)->getChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->getValue());
    }
    std::map<N, std::vector<E>> edge_map = (*node_iter_)->getEdges();
    std::vector<E> weights = edge_map[*edge_iter_];

    if (weight_iter_ == weights.rend()) {
        ++edge_iter_;

        if (edge_iter_ == children.rend()) {

            do {
                ++node_iter_;
            } while (node_iter_ != node_sentinel_
                     && children.rbegin() == children.rend());

            if (node_iter_ != node_sentinel_) {
                edge_iter_ = children.rbegin();

                std::map<N, std::vector<E>> edges = (*node_iter_)->getEdges();
                weights = edges[*edge_iter_];
                weight_iter_ = weights.begin();
            }
        }
    }

    return *this;
}

/**
 * Pre-decrement Operator overload for const_reverse_iterator
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator &gdwg::Graph<N,
        E>::const_reverse_iterator::operator--() {

//    std::vector<std::weak_ptr<Node>> children = (*node_iter_)->getChildren();
//    std::map<N, std::vector<E>> edge_map = (*node_iter_)->getEdges();
//    std::vector<E> edge_vector = edge_map[(*edge_iter_).first];

    std::vector<N> children;
    for (const auto& child : (*node_iter_)->getChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->getValue());
    }
    std::map<N, std::vector<E>> edge_map = (*node_iter_)->getEdges();
    std::vector<E> weights = edge_map[*edge_iter_];

    if (weight_iter_ == weights.rbegin()) {

        if (edge_iter_ == children.rbegin()) {

            do {
                --node_iter_;
            } while (node_iter_ != reverse_sentinel_
                     && children.begin() == children.end());


            if (children.begin() != children.end()) {

                children = (*node_iter_)->getChildren();
                edge_map = (*node_iter_)->getEdges();

                edge_iter_ = children.rend();
                --edge_iter_;

                weight_iter_ = edge_map[*edge_iter_].rend();
                --weight_iter_;

            } else if (node_iter_ == reverse_sentinel_) {
                throw std::runtime_error(
                        "Cannot cannot decrement past begin().");
            }

        } else {
            --edge_iter_;

            weight_iter_ = edge_map[*edge_iter_].rend();
            --weight_iter_;
        }

    } else {
        --weight_iter_;
    }

    return *this;
}

/**
 * Post-increment Operator overload for const_reverse_iterator
 */
template<typename N, typename E>
const typename gdwg::Graph<N, E>::const_reverse_iterator
gdwg::Graph<N, E>::const_reverse_iterator::operator++(int) {
    auto copy{*this};
    --(*this);
    return copy;
}

/**
 * Post-decrement Operator overload for const_reverse_iterator
 */
template<typename N, typename E>
const typename gdwg::Graph<N, E>::const_reverse_iterator
gdwg::Graph<N, E>::const_reverse_iterator::operator--(int) {
    auto copy{*this};
    ++(*this);
    return copy;
}

/**
 * * Operator overload for const_reverse_iterator
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator::reference
gdwg::Graph<N, E>::const_reverse_iterator::operator*() const {
    return {(*node_iter_).getValue(), *edge_iter_, *weight_iter_};
}

/**
 * Returns a const_reverse_iterator pointing to the last element in the
 * container. A const_reverse_iterator is a reverse iterator that points to
 * const content. This iterator can be increased and decreased (unless it is
 * itself also const), but it cannot be used to modify the contents it points
 * to, even if the content it points to is not itself const.
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator
gdwg::Graph<N, E>::crbegin() const {
    typename std::vector<std::shared_ptr<Node>>::iterator it = nodeList_
            .rbegin();

    while (*it->getChildren().size() == 0) {
        ++it;
    }

    if (it == nodeList_.rend()) {
        return rend();
    }


    std::vector<N> children;
    for (const auto& child : (*it)->getChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->getValue());
    }
    std::map<N, std::vector<E>> edges = (*it).getEdges();
    std::vector<E> last_weights = edges.rbegin()->second;

    return {it, nodeList_.rend(), nodeList_.end(),
    children.rbegin(), last_weights.rbegin()};
}

/**
 * Returns a const_reverse_iterator pointing to the before-the-first element
 * in the container. A const_reverse_iterator is a reverse iterator that
 * points to const content. This iterator can be increased and decreased
 * (unless it is itself also const), but it cannot be used to modify the
 * contents it points to, even if the content it points to is not itself
 * const.
 * If the container is empty, this function returns the same as
 * vector::crbegin. The value returned shall not be dereferenced.
 */
template<typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator
gdwg::Graph<N, E>::crend() const {

    if (nodeList_.size() == 0) {
        // todo
        return reverse_iterator(end());
    }

    return {nodeList_.rend(), nodeList_.rend(), nodeList_
    .end(), {}, };
}

