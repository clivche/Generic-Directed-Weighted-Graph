#include "assignments/dg/graph.h"
#include <stdexcept>

template<typename X, typename Predicate>
std::vector<X> mergeSorted(std::vector<X> v1, std::vector<X> v2, Predicate f = [] (X x) {return x;} ) {
    auto& it1 = v1.begin();
    auto& it2 = v2.begin();
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
        }
        else if (f(*it2) < f(*it1)) {
            result.push(*it2++);
        }
        else {
            result.push(*it1++);
            it2++;
        }
    }
    return result;
}


// Node Functions

template<typename N, typename E>
void gdwg::Graph<N, E>::Node::removeChild(const N& n) {
    auto& children = getChildren();
    for (const auto& child : children) {
        if (auto& childShared = child->lock()) {
            if (childShared->getValue() == n) {
                children->erase(child);
                break;
            }
        }
    }
    return;
}

template<typename N, typename E>
void gdwg::Graph<N, E>::Node::removeParent(const N& n) {
    auto& parents = getParents();
    for (const auto& parent : parents) {
        if (auto& parentShared = parent->lock()) {
            if (parentShared->getValue() == n) {
                parents->erase(parent);
                break;
            }
        }
    }
    return;
}

template<typename N, typename E>
void gdwg::Graph<N, E>::Node::addChild(std::weak_ptr<Node> dst) {

    auto nextChild = children_.begin();
    bool found = false;
    for (auto it = children_.begin(); it != children_.end(); it++) {
        if (auto child = it->lock()) {
            if (dst->getValue() < child->getValue()) {
                nextChild = it;
            }
            else if (dst->getValue() == child->getValue()) {
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

template<typename N, typename E>
bool gdwg::Graph<N, E>::Node::addEdge(const N& dst, const E& w) {
    auto nextEdge = edges_[dst].begin();
    for (auto it = edges_[dst].begin(); it != edges_[dst].end(); it++) {
        if (w < *it) {
            nextEdge = it;
        }
        else if (w == *it) {
            return false;
        }
    }
    edges_[dst].insert(nextEdge, w);
    return true;
}

template<typename N, typename E>
void gdwg::Graph<N, E>::Node::addParent(std::weak_ptr<Node> src) {

    auto nextParent = parents_.begin();
    bool found = false;
    for (auto it = parents_.begin(); it != parents_.end(); it++) {
        if (auto parent = it->lock()) {
            if (src->getValue() < parent->getValue()) {
                nextParent = it;
            }
            else if (src->getValue() == parent->getValue()) {
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

template<typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<N>::const_iterator c1, typename std::vector<N>::const_iterator c2) {
  for (auto& it = c1; it != c2; it++) {
    // Check for duplicates in initaliser vector
    bool alreadyInList = false;
    for (const auto& item : nodeList_) {
      if (item->getValue() == *it) {
        alreadyInList = true;
        break;
      }
    }
    if (!alreadyInList) {
      nodeList_.insert(std::shared_ptr<Node>(new Node(*it)));


        //TODO
    }
  }
}

template<typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator c1, typename std::vector<std::tuple<N, N, E>>::const_iterator c2) {
    for (auto& it = c1; it != c2; it++) {
        // Check for duplicates in initaliser vector
        bool alreadyInList = false;
        for (auto i = 0; i < 2; i++) {
            for (const auto& node : nodeList_) {
                if (node->getValue() == *it[i]) {
                    alreadyInList = true;
                    break;
                }
            }
            if (!alreadyInList) {
                nodeList_.insert(std::shared_ptr<Node>(new Node(*it)));


                //TODO
            }
        }
        InsertEdge(it[0], it[1], it[2]);
    }
}

template<typename N, typename E>
bool gdwg::Graph<N, E>::InsertNode(const N& n) {
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

template<typename N, typename E>
bool gdwg::Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {

    // Insert nodes into list
    InsertNode(src);
    InsertNode(dst);

    // Find pointers to appropriate nodes
    std::shared_ptr<Node> srcNode;
    std::shared_ptr<Node> dstNode;
    for (const auto& node : nodeList_) {
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

template<typename N, typename E>
bool gdwg::Graph<N, E>::DeleteNode(const N& n) {
    for (const auto& node : nodeList_) {
        if (node->getValue() == n) {
            nodeList_.erase(node);
            return true;
        }
    }
    return false;
}

template<typename N, typename E>
bool gdwg::Graph<N, E>::Replace(const N& oldData, const N& newData) {

    auto& old = nodeList_.end();
    for (const auto& node : nodeList_) {
        if (node->getValue() == newData) {
            return false;
        }
        if (node->getValue() == oldData) {
            old = node;
        }
    }

    if (old != nodeList_.end()) {
        old->changeValue(newData);
        for (const auto& parent : old->getParents()) {
            if (auto& sharedParent = parent->lock()) {
                auto& edges = sharedParent->getEdges();
                edges[newData] = edges[oldData];
                edges[oldData]->clear();
            }
        }
        return true;
    }
    throw std::runtime_error("Cannot call Graph::Replace on a node that doesn't exist");
}

template<typename N, typename E>
void gdwg::Graph<N, E>::MergeReplace(const N& oldData, const N& newData) {

    // Find shared_ptr to nodes within nodeList_
    auto& oldNode = nodeList_.end();
    auto& newNode = nodeList_.end();
    for (const auto& node : nodeList_) {
        if (node->getValue() == newData) {
            newNode = node;
        }
        if (node->getValue() == oldData) {
            oldNode = node;
        }
    }

    // Exception Handling
    if (oldNode == nodeList_.end() || newNode == nodeList_.end()) {
        throw std::runtime_error("Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
    }

    // Handle Parents
    for (const auto& parent : oldNode->getParents()) {
        if (const auto& parentShared = parent->lock()) {

            // Alter parent->children_
            //     Add child
            parentShared->addChild(std::weak_ptr<Node>(*newNode));
            //     Remove child
            parentShared->removeChild(oldData);

            // Merge/Replace parent->edges_
            auto& pEdge = parentShared->getEdges();
            pEdge[newData] = mergeSorted(pEdge[oldData], pEdge[newData]);
            pEdge[oldData]->clear();

            // Add parent to newNode
            newNode->addParent(parent);
        }
    }

    // Handle Children
    for (const auto& child : oldNode->getChildren()) {
        if (const auto& childShared = child.lock()) {
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
    for (const auto& [key, val] : oldEdges) {
        newEdges[key] = mergeSorted(newEdges[key], val);
    }

    nodeList_.erase(oldNode);
}

template<typename N, typename E>
void gdwg::Graph<N, E>::Clear() {
    nodeList_.clear();
}

template<typename N, typename E>
bool gdwg::Graph<N, E>::IsNode(const N& val) {
    for (const auto& node : nodeList_) {
        if (node->getValue() == val) {
            return true;
        }
    }
    return false;
}

template<typename N, typename E>
bool gdwg::Graph<N, E>::IsConnected(const N& src, const N& dst) {
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
        throw std::runtime_error("Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
    }

    // Check srcNode edge list
    auto edges = srcNode->getEdges();
    if (!edges[dst].empty) {
        return true;
    }
    return false;
}

template<typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetNodes() {
    std::vector<N> res;
    for (const auto& node : nodeList_) {
        res.push_back(node->getValue());
    }
    return res;
}

template<typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetConnected(const N& src) {
    std::vector<N> res;
    for (auto it = nodeList_.begin(); it != nodeList_.end(); it++) {
        if (it->getValue() == src) {
            for (const auto& dst : it->getChildren()) {
                if (auto dstShared = dst->lock()) {
                  res.push_back(dstShared.getValue());
                }
            }
            return res;
        }
    }
    throw std::runtime_error("Cannot call Graph::GetConnected if src doesn't exist in the graph");
}

template<typename N, typename E>
std::vector<E> gdwg::Graph<N, E>::GetWeights(const N& src, const N& dst) {

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
        throw std::runtime_error("Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
    }

    // return src-dst edge list
    auto srcEdges = srcNode->getEdges();
    return srcEdges[dst];

}

template<typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N, E>::find(const N&, const N&, const E&) {
    return typename gdwg::Graph<N,E>::const_iterator();
}

template<typename N, typename E>
bool gdwg::Graph<N, E>::erase(const N& src, const N& dst, const E& w) {

    for (auto it = nodeList_.begin(); it != nodeList_.end(); it++) {
        if (it->getValue() == src) {
            auto edges = it->getEdges();
            for (auto edge = edges[dst].begin(); edge != edges[dst].end(); edge++) {
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

template<typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N, E>::erase(typename gdwg::Graph<N,E>::const_iterator it) {
    return typename gdwg::Graph<N,E>::const_iterator();
}