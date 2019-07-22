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
  Graph<N, E>();
  gdwg::Graph<N, E>(std::vector<N>::const_iterator, std::vector<N>::const_iterator);
  bool InsertNode(const N&);

 private:

  class Node {
   public:
    explicit Node(N value) {
      this->value_ = value;
      this->deg_in_ = 0;
      this->deg_out_ = 0;
    };
    std::map<N&, E> getChildren() {return children_;};
    bool addEdge(const N&, const E&);

   private:
    N value_;
    int deg_in_;
    int deg_out_;
    std::map<N&,E> parents_;
    std::map<N&,E> children_;
  };

  bool addEdge(const N& dest, const E& weight) {
    this->deg_out++;
    this->children_.push(dest,weight);
  }

  std::vector<std::unique_ptr<Node>> nodeList_;

};

template<typename N, typename E>
gdwg::Graph<N, E>() {

}

template<typename N, typename E>
bool gdwg::InsertNode(const N& val) {
  if (this->checkExists(val)) {
    return false;
  }
  std::unique_ptr<Node> newNode = std::make_unique<Node> (val);
  this->nodeList_.push(newNode);
  return true;
}


}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
