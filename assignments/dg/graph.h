#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <vector>
#include <map>
#include <memory>

namespace gdwg {

  template <typename N, typename E>
  class Graph {
    public:
      class const_iterator {};

      // Vector Constructor
      Graph<N, E>(typename std::vector<N>::const_iterator c1, typename std::vector<N>::const_iterator c2) {
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
      };

    private:
      class Node {
        N value_;
        std::vector<Node*> parents_;
        std::vector<Node*> children_;

        Node(N value) : value_(value) {}
        inline std::vector<Node*> getChildren() {return children_;}
        inline std::vector<Node*> getParents() {return parents_;}
        inline N getValue() {return value_;}
      };
      class Edge {
       private:
        Node* start_;
        Node* end_;
        E weight_;
       public:
        Edge(N start, N end, E weight) {
          start_(std::weak_ptr<Node>(new Node(start))), end_(std::weak_ptr<Node>(new Node(end))), weight_(weight);
        }
      };
      std::vector<std::shared_ptr<Node>> nodeList_;
      std::vector<std::shared_ptr<Edge>> edgeList_;
    };

}  // namespace gdwg


#include "assignments/dg/graph.tpp"

//template<typename N, typename E>
//gdwg::Graph<N, E>(typename std::vector<N>::const_iterator c1, typename std::vector<N>::const_iterator c2) {  }

#endif  // ASSIGNMENTS_DG_GRAPH_H_
