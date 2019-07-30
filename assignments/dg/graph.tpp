

#include "assignments/dg/graph.h"
#include <stdexcept>
template<typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<N>::const_iterator c1, typename std::vector<N>::const_iterator c2) {
  for (auto it = c1; it != c2; it++) {
    // Check for duplicates in initaliser vector
    bool alreadyInList = false;
    for (const auto& item : nodeList_) {
      if (*item.getValue() == *it) {
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
gdwg::Graph<N, E>::Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator c1, typename std::vector<std::tuple<N, N, E>>::const_iterator c2) {
  for (auto it = c1; it != c2; it++) {
    // Check for duplicates in initaliser vector
    bool alreadyInList = false;
    for (auto i = 0; i < 2; i++) {
      for (const auto& node : nodeList_) {
        if (*node.getValue() == *it[i]) {
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
bool gdwg::Graph<N, E>::InsertNode(const N& n) {
  // Insert node at beginning by default
  auto next = nodeList_.begin();
  for (const auto& node : nodeList_) {
    // Insert node before every value it is less than
    if (n < node->getValue()) {
      next = node;
    }
    if (node->getValue() == n) {
      return false;
    }
  }
  nodeList_.insert(next, std::shared_ptr<Node>(new Node(n)));
  return true;
}

template<typename N, typename E>
bool gdwg::Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {

    InsertNode(src);
    InsertNode(dst);
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

    if (srcNode.addChild(dstNode, w)) {
        dstNode.addParent(srcNode);
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
    auto old = nodeList_.end();
    for (const auto& node : nodeList_) {
        if (node->getValue() == newData) {
            return false;
        }
        if (node->getValue() == oldData) {
            old = node;
        }
    }
    if (old != nodeList_.end()) {
        *old.changeValue(newData);
        return true;
    }
    throw std::runtime_error("Cannot call Graph::Replace on a node that doesn't exist");
}


template<typename N, typename E>
bool gdwg::Graph<N, E>::Node::addChild(std::shared_ptr<Node> dst, const E& w) {

    auto nextChild = children_.begin();
    bool found = false;
    for (const auto& item : children_) {
        if (auto child = *item.lock()) {
            if (dst->getValue() < child->getValue()) {
                nextChild = item;
            }
            if (dst->getValue() == child->getValue()) {
                found = true;
                break;
            }
        }
    }

    if (!found) {
        children_.insert(nextChild, std::weak_ptr<Node>(dst));
    }

    // Add to edge array
    auto nextEdge = edges_[dst].begin();
    for (const auto& edge : edges_[dst]) {
        if (w < edge) {
            nextEdge = edge;
        }
        if (w == edge) {
            return false;
        }
    }
    edges_[dst].insert(nextEdge, w);

    return true;
}

template<typename N, typename E>
bool gdwg::Graph<N, E>::Node::addParent(std::shared_ptr<Node> src) {

    auto nextParent = parents_.begin();
    bool found = false;
    for (const auto& item : parents_) {
        if (auto parent = *item.lock()) {
            if (src->getValue() < parent->getValue()) {
                nextParent = item;
            }
            if (src->getValue() == parent->getValue()) {
                found = true;
                break;
            }
        }
    }

    if (!found) {
        parents_.insert(nextParent, std::weak_ptr<Node>(src));
    }
    return true;
}
