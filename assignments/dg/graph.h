/**
 * Copyright [2019] Clive Chen, Vaishnavi Bapat
 * zid - z5166040, z5075858
 */
#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

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

    friend bool operator==(const const_reverse_iterator& lhs, const const_reverse_iterator rhs) {
      return (lhs.node_iter_ == rhs.node_iter_) && (lhs.edge_iter_ == rhs.edge_iter_) &&
             (lhs.weight_iter_ == rhs.weight_iter_);
    }

    friend bool operator!=(const const_reverse_iterator& lhs, const const_reverse_iterator& rhs) {
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
                           const decltype(node_sentinel_)& node_sentinel,
                           const decltype(reverse_sentinel_)& reverse_sentinel_,
                           const decltype(edge_iter_)& edge_iter,
                           const decltype(weight_iter_) weight_iter)
      : node_iter_{node_iter}, node_sentinel_{node_sentinel}, reverse_sentinel_{reverse_sentinel_},
        edge_iter_{edge_iter}, weight_iter_{weight_iter} {}
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

    friend bool operator==(const const_iterator& lhs, const const_iterator rhs) {
      return (lhs.node_iter_ == rhs.node_iter_) && (lhs.edge_iter_ == rhs.edge_iter_) &&
             (lhs.weight_iter_ == rhs.weight_iter_);
    }

    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
      return !(lhs == rhs);
    }

   private:
    friend class Graph;

    typename std::vector<std::shared_ptr<Node>>::const_iterator node_iter_;
    const typename std::vector<std::shared_ptr<Node>>::const_iterator node_sentinel_;
    const typename std::vector<std::shared_ptr<Node>>::const_iterator reverse_sentinel_;
    typename std::vector<N>::const_iterator edge_iter_;
    typename std::vector<E>::const_iterator weight_iter_;

    const_iterator(const decltype(node_iter_)& node_iter,
                   const decltype(node_sentinel_)& node_sentinel,
                   const decltype(reverse_sentinel_)& reverse_sentinel_,
                   const decltype(edge_iter_)& edge_iter,
                   const decltype(weight_iter_)& weight_iter)
      : node_iter_{node_iter}, node_sentinel_{node_sentinel}, reverse_sentinel_{reverse_sentinel_},
        edge_iter_{edge_iter}, weight_iter_{weight_iter} {}
  };

  // Graph Constructors
  Graph<N, E>() {}

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

    // check nodes
    int max = g1.nodeList_.size();
    for (int counter = 0; counter < max; counter++) {
      // check node values
      if (g1.nodeList_[counter]->GetValue() != g2.nodeList_[counter]->GetValue()) {
        return false;
      }
    }

    // check edges
    for (int counter = 0; counter < max; counter++) {
      Node n1 = *g1.nodeList_[counter];
      Node n2 = *g2.nodeList_[counter];

      std::vector<std::weak_ptr<Node>> children_1 = n1.GetChildren();
      std::map<N, std::vector<E>> edge_map_1 = n1.GetEdges();
      std::vector<N> avail_dst_1;

      std::vector<std::weak_ptr<Node>> children_2 = n1.GetChildren();
      std::map<N, std::vector<E>> edge_map_2 = n1.GetEdges();
      std::vector<N> avail_dst_2;

      for (auto it = children_1.begin(); it != children_1.end(); ++it) {
        if (auto childLock = it->lock()) {
          avail_dst_1.push_back(childLock->GetValue());
        }
      }

      for (auto it = children_2.begin(); it != children_2.end(); ++it) {
        if (auto childLock = it->lock()) {
          avail_dst_2.push_back(childLock->GetValue());
        }
      }

      if (avail_dst_1 != avail_dst_2) {
        return false;
      }

      for (auto dst = avail_dst_1.begin(); dst != avail_dst_1.end(); ++dst) {
        typename std::vector<E> weights_1 = edge_map_1[*dst];
        typename std::vector<E> weights_2 = edge_map_2[*dst];

        if (weights_1 != weights_2) {
          return false;
        }
      }
    }
    return true;
  }

  friend bool operator!=(const Graph& g1, const Graph& g2) { return !(g1 == g2); }

  friend std::ostream& operator<<(std::ostream& os, const Graph& g) {
    const std::string NODE_START = " (";
    const std::string NODE_END = "\n)\n";
    const std::string EDGE_SEPARATOR = " | ";
    const std::string CHILD_START = "\n  ";

    int max = g.nodeList_.size();
    std::vector<N> output;

    for (int counter = 0; counter < max; counter++) {
      Node n = *g.nodeList_[counter];
      os << n.GetValue() << NODE_START;

      std::vector<std::weak_ptr<Node>> children = n.GetChildren();
      std::map<N, std::vector<E>> edge_map = n.GetEdges();
      std::vector<N> avail_dst;

      for (auto it = children.begin(); it != children.end(); ++it) {
        if (auto childLock = it->lock()) {
          avail_dst.push_back(childLock->GetValue());
        }
      }

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

    explicit Node(N value) : value_(value) {}

    inline std::vector<std::weak_ptr<Node>> GetChildren() { return children_; }

    inline std::vector<std::weak_ptr<Node>> GetParents() { return parents_; }

    inline std::map<N, std::vector<E>> GetEdges() { return edges_; }

    inline N GetValue() { return value_; }

    inline void ChangeValue(N val) { value_ = val; }

    void UpdateEdges(std::vector<E> weights, N newNode, N oldNode);

    bool AddEdge(const N&, const E&);

    void AddChild(std::weak_ptr<Node>);

    void RemoveChild(const N&);

    void AddParent(std::weak_ptr<Node>);

    void RemoveParent(const N&);
  };

 private:
  std::vector<std::shared_ptr<Node>> nodeList_;
};

}  // namespace gdwg

#endif  // ASSIGNMENTS_DG_GRAPH_H_
