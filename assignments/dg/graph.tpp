/**
 * Copyright [2019] Clive Chen, Vaishnavi Bapat
 * zid - z5166040, z5075858
 */

#include "assignments/dg/graph.h"
#include <stdexcept>
#include <algorithm>

//template<typename X, typename Predicate>
//std::vector<X> mergeSorted(std::vector<X> v1, std::vector<X> v2,
//                           Predicate f = [](X x) { return x; }) {
//    auto &it1 = v1.begin();
//    auto &it2 = v2.begin();
//    std::vector<X> result;
//    while (it1 != v1.end() || it2 != v2.end()) {
//        if (it1 == v1.end()) {
//            result.push(*it2++);
//        }
//        if (it2 == v2.end()) {
//            result.push(*it1++);
//        }
//        if (f(*it1) < f(*it2)) {
//            result.push(*it1++);
//        } else if (f(*it2) < f(*it1)) {
//            result.push(*it2++);
//        } else {
//            result.push(*it1++);
//            it2++;
//        }
//    }
//    return result;
//}
template<typename X>
std::vector<X> mergeSorted(std::vector<X> v1, std::vector<X> v2) {
    auto it1 = v1.begin();
    auto it2 = v2.begin();
    std::vector<X> result;
    while (it1 != v1.end() || it2 != v2.end()) {
        if (it1 == v1.end()) {
            result.push_back(*it2);
            ++it2;
        } else if (it2 == v2.end()) {
            result.push_back(*it1);
            ++it1;
        } else {
            if (*it1 < *it2) {
                result.push_back(*it1);
                ++it1;
            } else if (*it2 < *it1) {
                result.push_back(*it2);
                ++it2;
            } else {
                result.push_back(*it1);
                ++it1;
                ++it2;
            }
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
void gdwg::Graph<N, E>::Node::RemoveChild(const N &n) {
    auto children = GetChildren();
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (auto childShared = it->lock()) {
            if (childShared->GetValue() == n) {
                children.erase(it);
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
void gdwg::Graph<N, E>::Node::RemoveParent(const N &n) {
    auto parents = GetParents();
    for (auto it = parents.begin(); it != parents.end(); ++it) {
        if (auto parentShared = it->lock()) {
            if (parentShared->GetValue() == n) {
                parents.erase(it);
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
void gdwg::Graph<N, E>::Node::AddChild(std::weak_ptr<Node> dst) {

    bool found = false;
    auto dstLock = dst.lock();
    auto it = children_.begin();
    while (it != children_.end()) {
        if (auto child = it->lock()) {
            if (dstLock->GetValue() == child->GetValue()) {
                found = true;
                break;
            }
            else if (dstLock->GetValue() > child->GetValue()) {
                it++;
            }
            else {
                break;
            }
        }
    }

    if (!found) {
        children_.insert(it, dst);
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
bool gdwg::Graph<N, E>::Node::AddEdge(const N &dst, const E &w) {
    auto it = edges_[dst].begin();
    while (it != edges_[dst].end()) {
        if (w > *it) {
            it++;
        }
        else if (w == *it) {
          return false;
        }
        else {
            break;
        }
    }
    edges_[dst].insert(it, w);
    std::sort(edges_[dst].begin(), edges_[dst].end());
    return true;
}

/**
 * Add node as a parent of another
 *
 * @param src - node to be added
 */
template<typename N, typename E>
void gdwg::Graph<N, E>::Node::AddParent(std::weak_ptr<Node> src) {

    auto nextParent = parents_.begin();
    bool found = false;
    auto srcLock = src.lock();
    for (auto it = parents_.begin(); it != parents_.end(); it++) {
        if (auto parent = it->lock()) {
            if (srcLock->GetValue() < parent->GetValue()) {
                nextParent = it;
            } else if (srcLock->GetValue() == parent->GetValue()) {
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

/**
 * Replaces the destination node in teh edges_ map
 *
 * @param weights - new vector of weights from this node to newNode
 * @param newNode - new dst
 * @param oldNode - old dst
 */
template<typename N, typename E>
void gdwg::Graph<N, E>::Node::UpdateEdges(std::vector<E> weights, N newNode,
                                          N oldNode) {

    this->edges_[newNode] = weights;
    auto old_key_it = this->edges_.find(oldNode);
    this->edges_.erase(old_key_it);
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
    // TODO: can't sort const_iterator
    sort(c1,c2);
    for (auto &it = c1; it != c2; it++) {
        // Check for duplicates in initaliser vector
        bool alreadyInList = false;
        for (const auto &item : nodeList_) {
            if (item->GetValue() == *it) {
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
                if (node->GetValue() == *it[i]) {
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
    // TODO: can't sort initializer list
    sort(args.begin(), args.end());
    for (auto &it = args.begin(); it != args.begin(); it++) {
        // Check for duplicates in initaliser vector
        bool alreadyInList = false;
        for (const auto &item : nodeList_) {
            if (item->GetValue() == *it) {
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
    for (auto it = g.GetNodes().begin(); it != g.GetNodes().end(); it++) {
        this->InsertNode(*it);
    }
    // Copy every edge into graph
    auto it = g.begin();
    std::tuple<N,N,E> temp = *it;

//    for (auto it = g.begin(); it != g.end(); ++it) {
////        this->InsertEdge(*it);
//        std::tuple<N,N,E> temp = *it;
//    }
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
    auto it = nodeList_.begin();

    while (it != nodeList_.end()) {
        if (n == (*it)->GetValue()) {
            return false;
        }

        // Insert node before every value it is less than
        if (n > (*it)->GetValue()) {
            ++it;
        } else {
            nodeList_.insert(it, std::shared_ptr<Node>(new Node(n)));
            return true;
        }
    }

    nodeList_.push_back(std::shared_ptr<Node>(new Node(n)));
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
    bool srcPresent = false;
    bool dstPresent = false;

    // Find pointers to src and dst nodes
    std::shared_ptr<Node> srcNode;
    std::shared_ptr<Node> dstNode;
    for (const auto &node : nodeList_) {
        if (node->GetValue() == src) {
            srcNode = node;
            srcPresent = true;
        }
        if (node->GetValue() == dst) {
            dstNode = node;
            dstPresent = true;
        }
    }

    // Exception Handling
    if (srcPresent == false || dstPresent == false) {
        throw std::runtime_error("Cannot call Graph::InsertEdge when either "
                                 "src or dst node does not exist");
    }

    // Check if an edge exists
    if (srcNode->AddEdge(dst, w)) {
        srcNode->AddChild(dstNode);
        dstNode->AddParent(srcNode);
        return true;
    }
    return false;
}

/**
 * Deletes a given node and all its associated incoming and outgoing edges.
 * This function does nothing (returns false) if the node that is to be deleted
 * does not exist in the graph.
 *
 * @param n - node to be deleted
 */
template<typename N, typename E>
bool gdwg::Graph<N, E>::DeleteNode(const N &n) {
    for (auto it = nodeList_.begin(); it != nodeList_.end(); ++it) {
        if ((*it)->GetValue() == n) {
            nodeList_.erase(it);
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

    // Find old and new node
    auto old = nodeList_.end();
    for (auto it = nodeList_.begin(); it != nodeList_.end(); it++) {
        if ((*it)->GetValue() == newData) {
            return false;
        }
        if ((*it)->GetValue() == oldData) {
            old = it;
        }
    }

    // If old value was found
    if (old != nodeList_.end()) {
        // Replace value
        (*old)->ChangeValue(newData);

        // edit edges map in parents of oldNode
        auto parents = (*old)->GetParents();
        for (auto it = parents.begin(); it != parents.end(); ++it) {
            if (auto sharedParent = it->lock()) {

                auto edges = sharedParent->GetEdges();
                std::vector<E> newWeightVector;

                for (const auto& edge : edges[oldData]) {
                    newWeightVector.push_back(edge);
                }

                sharedParent->UpdateEdges(newWeightVector, newData, oldData);
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
    auto oldPtr = nodeList_.end();
    auto newPtr = nodeList_.end();
    for (auto it = nodeList_.begin(); it != nodeList_.end(); it++) {
        if ((*it)->GetValue() == newData) {
            newPtr = it;
        }
        if ((*it)->GetValue() == oldData) {
            oldPtr = it;
        }
    }

    // Exception Handling
    if (oldPtr == nodeList_.end() || newPtr == nodeList_.end()) {
        throw std::runtime_error(
                "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
    }
    auto oldNode = *oldPtr;
    auto newNode = *newPtr;

    // Handle Parents of oldNode
    auto parentList = oldNode->GetParents();

std::cout << "parentList of " << oldNode->GetValue()<<": ";
for (const auto& item : parentList) {
    const auto parentShared = item.lock();
    std::cout << (parentShared)->GetValue() << ' ';
}
std::cout << '\n';

    for (auto it = parentList.begin(); it != parentList.end(); it++) {
        if (const auto parentShared = it->lock()) {
            if (parentShared->GetValue() == oldData) {
                newNode->AddChild(std::weak_ptr<Node>(newNode));
                auto pEdge = parentShared->GetEdges();
                for (const auto& w : pEdge[oldData]) {
                    this->InsertEdge(newData, newData, w);
                }
                continue;
            }

            
//std::cout << "----------------- " <<(parentShared)->GetValue() << " -----------------\n";
//std::cout << "Children of " << (parentShared)->GetValue() <<" BEFORE: ";
//auto CofP = parentShared->GetChildren();
//for (const auto& item : CofP) {
//if (auto itemlock = item.lock()) {
//std::cout << itemlock->GetValue() << ' ';
//}
//}
//std::cout << '\n';




            // Alter parent->children_
            //     Add newNode child
            parentShared->AddChild(std::weak_ptr<Node>(newNode));
            //     Remove child
            parentShared->RemoveChild(oldData);

//std::cout << "Children of " << (parentShared)->GetValue() <<" AFTER: ";
//auto newCofP = parentShared->GetChildren();
//for (auto it2 = newCofP.begin(); it2 != newCofP.end(); it2++) {
//if (auto itemlock = it2->lock()) {
//std::cout << itemlock->GetValue() << ' ';
//}
//}
//std::cout << '\n';

            // Merge/Replace parent->edges_
            auto pEdge = parentShared->GetEdges();

//std::cout << "Before Merge:\n";
//std::cout << parentShared->GetValue()<<"->"<<oldData<<": ";
//for (const auto& item : pEdge[oldData]) {
//std::cout << item << ' ';
//}
//std::cout << '\n';
//
//std::cout << parentShared->GetValue()<<"->"<<newData<<": ";
//for (const auto& item : pEdge[newData]) {
//std::cout << item << ' ';
//}
//std::cout << '\n';
//


            auto mergedWeights = mergeSorted(pEdge[oldData], pEdge[newData]);

            for (const auto& w : mergedWeights) {
                this->InsertEdge(parentShared->GetValue(), newData, w);
            }

//            std::cout << "Merge Result: ";
//            for (const auto& t : temp) {
//                std::cout << t << ' ';
//            }
//            std::cout << '\n';
//
//            pEdge[newData].clear();
//            pEdge.insert({newData, temp});
//            pEdge[oldData].clear();

            pEdge = parentShared->GetEdges();

            //std::cout << "After Merge and Clear:\n";
//std::cout << parentShared->GetValue()<<"->"<<oldData<<": ";
//for (const auto& item : pEdge[oldData]) {
//std::cout << item << ' ';
//}
//std::cout << '\n';
//
//std::cout << parentShared->GetValue()<<"->"<<newData<<": ";
//for (const auto& item : pEdge[newData]) {
//std::cout << item << ' ';
//}
//std::cout << '\n';
//
//std::cout << "Parents of newNode before add: ";
//auto PofNN = newNode->GetParents();
//for (const auto& item : PofNN) {
//    if (auto itemlock = item.lock()) {
//        std::cout << itemlock->GetValue() << ' ';
//    }
//}
//std::cout << '\n';

            // Add parent to newNode
            newNode->AddParent(*it);

//std::cout << "Parents of newNode after add: ";
//PofNN = newNode->GetParents();
//for (const auto& item : PofNN) {
//    if (auto itemlock = item.lock()) {
//        std::cout << itemlock->GetValue() << ' ';
//    }
//}
//std::cout << '\n';
        }
    }

    // Handle Children of oldNode
    for (const auto child : oldNode->GetChildren()) {
        if (const auto childShared = child.lock()) {
            // Alter child->parent_
            //     Remove parent
            childShared->RemoveParent(oldData);
            //     Add parent
            childShared->AddParent(std::weak_ptr<Node>(newNode));

            // Add child to newNode
            newNode->AddChild(child);
        }
    }

    // Handle Edges
    auto newEdges = newNode->GetEdges();
    auto oldEdges = oldNode->GetEdges();
    for (auto it = oldEdges.begin(); it != oldEdges.end(); it++) {
        auto newEdgeVector = mergeSorted(newEdges[it->first], it->second);
        newEdges[it->first] = newEdgeVector;
    }

    nodeList_.erase(oldPtr);
}

/**
 * Remove all nodes and edges from the graph. New nodes or edges can be added
 * to the graph afterwards.
 */
template<typename N, typename E>
void gdwg::Graph<N, E>::Clear() {
    nodeList_.clear();

    // TODO :: see test "Reconstruct on a cleared graph"
}

/**
 * Returns true if a node with value val exists in the graph and false
 * otherwise.
 * @param val - value of potential node
 */
template<typename N, typename E>
bool gdwg::Graph<N, E>::IsNode(const N &val) {
    for (const auto &node : nodeList_) {
        if (node->GetValue() == val) {
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
        if ((*it)->GetValue() == src) {
            srcNode = it;
        }
        if ((*it)->GetValue() == dst) {
            dstFound = true;
        }
    }
    if (srcNode == nodeList_.end() || !dstFound) {
        throw std::runtime_error(
                "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
    }

    // Check srcNode edge list
    auto edges = (*srcNode)->GetEdges();
    if (!edges[dst].empty()) {
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
        res.push_back(node->GetValue());
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
        if ((*it)->GetValue() == src) {

            auto children = (*it)->GetChildren();
            for (auto dst = children.begin(); dst != children.end(); ++dst) {
                if (auto dstShared = dst->lock()) {
                    res.push_back(dstShared->GetValue());
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
        if (it->GetValue() == src) {
            srcNode = it;
        }
        if (it->GetValue() == dst) {
            dstFound = true;
        }
    }
    if (srcNode == nodeList_.end() || !dstFound) {
        throw std::runtime_error(
                "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
    }

    // return src-dst edge list
    auto srcEdges = srcNode->GetEdges();
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
        if (it->GetValue() == src) {
            auto edges = it->GetEdges();
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

//    std::vector<std::weak_ptr<Node>> children = (*node_iter_)->GetChildren();
//    std::map<N, std::vector<E>> edge_map = (*node_iter_)->GetEdges();
//    std::vector<E> edge_vector = edge_map[*edge_iter_.first];

    std::vector<N> children;
    for (const auto& child : (*node_iter_)->GetChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->GetValue());
    }
    std::map<N, std::vector<E>> edge_map = (*node_iter_)->GetEdges();
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

                std::map<N, std::vector<E>> edges = (*node_iter_)->GetEdges();
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
    for (const auto& child : (*node_iter_)->GetChildren()) {
        children.push_back(child->GetValue());
    }
    std::map<N, std::vector<E>> edge_map = (*node_iter_)->GetEdges();
    std::vector<E> weights = edge_map[*edge_iter_];

    if (weight_iter_ == weights.begin()) {

        if (edge_iter_ == children.begin()) {

            do {
                --node_iter_;
            } while (node_iter_ != reverse_sentinel_
                     && children.begin() == children.end());


            if (children.begin() != children.end()) {

                children = (*node_iter_)->GetChildren();
                edge_map = (*node_iter_)->GetEdges();

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
    return {(*node_iter_)->GetValue(), *edge_iter_, *weight_iter_};
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
    while ((*it)->GetChildren().size() == 0) {
        ++it;
        if (it == nodeList_.end()) {
            return end();
        }
    }

    std::vector<N> children;
    for (const auto& child : (*it)->GetChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->GetValue());
    }
    std::map<N, std::vector<E>> edges = (*it)->GetEdges();
    std::vector<E> weights = edges.begin()->second;

    std::vector<std::shared_ptr<Node>>first = nodeList_;
    std::vector<N> fourth;
    std::vector<E> fifth;
//    return {first.begin(), first.end(), first.begin(), children.begin(), weights.begin()};
    return {&it, &nodeList_.end(), &nodeList_.begin(), children.begin(), weights
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
    while ((*it)->GetChildren().size() == 0) {
        --it;
    }

    std::vector<N> children;
    for (const auto& child : (*it)->GetChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->GetValue());
    }
    std::map<N, std::vector<E>> edges = nodeList_.begin()->GetEdges();
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
    for (const auto& child : (*node_iter_)->GetChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->GetValue());
    }
    std::map<N, std::vector<E>> edge_map = (*node_iter_)->GetEdges();
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

                std::map<N, std::vector<E>> edges = (*node_iter_)->GetEdges();
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

//    std::vector<std::weak_ptr<Node>> children = (*node_iter_)->GetChildren();
//    std::map<N, std::vector<E>> edge_map = (*node_iter_)->GetEdges();
//    std::vector<E> edge_vector = edge_map[(*edge_iter_).first];

    std::vector<N> children;
    for (const auto& child : (*node_iter_)->GetChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->GetValue());
    }
    std::map<N, std::vector<E>> edge_map = (*node_iter_)->GetEdges();
    std::vector<E> weights = edge_map[*edge_iter_];

    if (weight_iter_ == weights.rbegin()) {

        if (edge_iter_ == children.rbegin()) {

            do {
                --node_iter_;
            } while (node_iter_ != reverse_sentinel_
                     && children.begin() == children.end());


            if (children.begin() != children.end()) {

                children = (*node_iter_)->GetChildren();
                edge_map = (*node_iter_)->GetEdges();

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
    return {(*node_iter_).GetValue(), *edge_iter_, *weight_iter_};
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

    while (*it->GetChildren().size() == 0) {
        ++it;
    }

    if (it == nodeList_.rend()) {
        return rend();
    }


    std::vector<N> children;
    for (const auto& child : (*it)->GetChildren()) {
        auto child_lock = child.lock();
        children.push_back(child_lock->GetValue());
    }
    std::map<N, std::vector<E>> edges = (*it).GetEdges();
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

