#include <iostream>
#include <string>

#include "assignments/dg/graph.h"
#include "assignments/dg/graph.tpp"

// Note: At the moment, there is no client.sampleout. Please do your own testing

int main() {
  gdwg::Graph<std::string, int> g;
    std::cout << g << '\n';

//    g.InsertNode("h");
//    g.InsertNode("g");
//    g.InsertNode("f");
//    g.InsertNode("e");
        g.InsertNode("c");
        g.InsertNode("d");
        g.InsertNode("b");
        g.InsertNode("a");
        g.InsertEdge("a", "b", 10);
        g.InsertEdge("a", "b", 1);
        g.InsertEdge("a", "d", 4);
        g.InsertEdge("a", "d", 50);
        g.InsertEdge("b", "b", 2);
        g.InsertEdge("c", "b", 3);
        g.InsertEdge("b", "c", 2);
        g.InsertEdge("d", "b", 23);
        g.InsertEdge("d", "d", 5);
        g.InsertNode("x");
        g.InsertNode("y");
        g.InsertNode("z");
        g.InsertNode("w");
        g.InsertEdge("w", "x", 10);
        g.InsertEdge("z", "z", 1);
        g.InsertEdge("w", "y", 4);
        g.InsertEdge("z", "y", 50);
        g.InsertEdge("z", "w", 2);
        g.InsertEdge("x", "z", 3);
        g.InsertEdge("z", "x", 2);
        g.InsertEdge("w", "w", 23);
        g.InsertEdge("y", "y", 5);
        std::cout << g << '\n';

    std::cout << "MergeReplace" << '\n';

    g.MergeReplace("d", "a");
//    g.Replace("b", "p");
//    std::vector<int> v1{1,2,4,8,10};
//    std::vector<int> v2{2,3,5,6,10,17};
//    auto result = mergeSorted(v1, v2);
//    for (const auto& item : result) {
//        std::cout << item << "\n";
//    }
//    g.InsertEdge("a", "c", 3);
//    g.InsertEdge("a", "c", 2);
//    g.InsertEdge("a", "a", 5);
//    g.InsertEdge("b", "a", 4);
//    g.InsertEdge("c", "c", 10);
//    g.InsertEdge("c", "c", 1);
//    g.InsertEdge("c", "c", 1);
//
//
//    g.InsertNode("x");
//    g.InsertEdge("x", "x", 10);
//    g.InsertEdge("x", "x", 0);
//    g.InsertEdge("x", "x", 1);
//    g.InsertEdge("x", "x", 21);
//    g.InsertEdge("x", "x", 13);
//    g.InsertEdge("x", "x", 51);
//    g.InsertEdge("x", "x", -1);
//
//    g.Replace("x", "y");
//    g.Replace("y", "x");
//    g.Replace("are", "chicken");
//    g.MergeReplace("a", "b");
    std::cout << g << '\n';


//  gdwg::Graph<std::string, int> g2{g};

//  std::cout << g2 << "\n";
//
//  // This is a structured binding.
//  // https://en.cppreference.com/w/cpp/language/structured_binding
//  // It allows you to unpack your tuple.
//  for (const auto& [from, to, weight] : g) {
//    std::cout << from << " -> " << to << " (weight " << weight << ")\n";
//  }
}
