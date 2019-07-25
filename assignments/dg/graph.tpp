

#include "assignments/dg/graph.h"

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
  for (const auto& node : nodeList_) {
    if (node->getValue() == n) {
      return false;
    }
  }
  nodeList_.push(std::shared_ptr<Node>(new Node(n)));
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
  // src and dst must be inside nodeList_ because of InsertNode()
  auto wkSrcNode = std::weak_ptr<Node>(srcNode);
  auto wkDstNode = std::weak_ptr<Node>(dstNode);

  for (const auto& edge : edgeList_) {
    if (edge->getSrc->getValue == src && edge->getDst->getValue == dst && edge->getWeight == w) {
      return false;
    }
  }
  edgeList_.push(srcNode, dstNode, w);
  return true;
}

//template<typename N, typename E>
//bool gdwg::Graph<N, E>::DeleteNode(const N& n) {
//
//}