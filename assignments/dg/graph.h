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
      Graph<N, E>(typename std::vector<N>::const_iterator c1, typename std::vector<N>::const_iterator c2);
      Graph<N, E>(typename std::vector<std::tuple<N, N, E>>::const_iterator, typename std::vector<std::tuple<N, N, E>>::const_iterator);

      bool InsertEdge(const N&, const N&, const E&);
      bool InsertNode(const N&);
      bool DeleteNode(const N&);


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

#endif  // ASSIGNMENTS_DG_GRAPH_H_
