/*
Copyright [2019] Clive Chen, Vaishnavi Bapat
zid - z5166040, z5075858
  == Explanation and rational of testing ==

  Explain and justify how you approached testing, the degree
   to which you're certain you have covered all possibilities,
   and why you think your tests are that thorough.

*/

#include "assignments/dg/graph.h"
#include "assignments/dg/graph.tpp"
#include "catch.h"
#include "cassert"


/*****************************************************/
/**  == Constructor with Edge and Node Insertion == **/
/*****************************************************/
SCENARIO("Default constructor") {
    WHEN("the default constructor is used") {
        gdwg::Graph<std::string, int> g;

        THEN("the result is an empty graph") {
            auto nodeList = g.GetNodeList();
            CHECK(nodeList.size() == 0);
        }
    }
}

SCENARIO("Vector iterator constructor") {
    WHEN("the vector iterator constructor is used") {
        std::vector<std::string> v1{"c", "e", "a", "d", "b"};
        gdwg::Graph<std::string, int> g{v1.begin(), v1.end()};

        THEN("the result is a correctly initialised sorted graph") {
            gdwg::Graph<std::string, int> expected;
            expected.InsertNode("a");
            expected.InsertNode("b");
            expected.InsertNode("c");
            expected.InsertNode("d");
            expected.InsertNode("e");
            CHECK((g == expected));
        }
    }
}

/***************************/
/**  == Node Insertion == **/
/***************************/

SCENARIO("Node insertion") {
    GIVEN("a graph is default constructed") {
        gdwg::Graph<std::string, int> g;

        WHEN("a node is inserted into the graph") {
            std::string str = "hello";
            bool res = g.InsertNode(str);

            THEN("The solitary node should be found in the graph") {

                auto nodeList = g.GetNodeList();
                auto node_iter = nodeList.begin();

                CHECK(res == true);
                CHECK(nodeList.size() == 1);
                CHECK((*node_iter)->GetValue() == str);
                CHECK((*node_iter)->GetChildren().size() == 0);
                CHECK((*node_iter)->GetParents().size() == 0);
                CHECK((*node_iter)->GetEdges().size() == 0);
            }
        }
    }
}

SCENARIO("Node insert with pre-existing node") {
    GIVEN("a graph contains one node") {
        gdwg::Graph<std::string, int> g;
        std::string str = "hello";
        bool res = g.InsertNode(str);

        WHEN("a another node with the same value is inserted into the graph") {
            bool res2 = g.InsertNode(str);

            THEN("The graph should remain unchanged") {

                auto nodeList = g.GetNodeList();
                auto node_iter = nodeList.begin();

                CHECK(res == true);
                CHECK(res2 == false);

                CHECK(nodeList.size() == 1);
                CHECK((*node_iter)->GetValue() == str);
                CHECK((*node_iter)->GetChildren().size() == 0);
                CHECK((*node_iter)->GetParents().size() == 0);
                CHECK((*node_iter)->GetEdges().size() == 0);
            }
        }
    }
}

/***************************/
/**  == Edge Insertion == **/
/***************************/

SCENARIO("Insert edge between two nodes") {
    GIVEN("a graph contains two nodes") {
        gdwg::Graph<std::string, std::string> g;
        std::string n1 = "general";
        std::string n2 = "kenobi";
        g.InsertNode(n1);
        g.InsertNode(n2);

        WHEN("an edge is created between the two nodes") {
            std::string edge = "cough";
            bool res = g.InsertEdge(n1, n2, edge);

            THEN("The graph should contain the newly created edge") {

                auto nodeList = g.GetNodeList();

                CHECK(res == true);
                CHECK(nodeList.size() == 2);

                auto node1 = nodeList[0];
                auto node2 = nodeList[1];
                auto edge_map = (*node1).GetEdges();

                auto edge_key = edge_map.begin()->first;
                auto edge_val = edge_map.begin()->second;

                CHECK((*node1).GetValue() == n1);
                CHECK((*node1).GetChildren().size() == 1);
                CHECK((*node1).GetParents().size() == 0);
                CHECK(edge_map.size() == 1);
                CHECK(edge_key == n2);
                CHECK(edge_val == std::vector<std::string>{edge});

                // check dst remains the same
                CHECK((*node2).GetValue() == n2);
                CHECK((*node2).GetChildren().size() == 0);
                CHECK((*node2).GetParents().size() == 1);
                CHECK((*node2).GetEdges().size() == 0);
            }
        }
    }
}

SCENARIO("Insert second edge between two nodes") {
    GIVEN("a graph contains two nodes connected by one edge") {
        gdwg::Graph<std::string, int> g;
        std::string n1 = "general";
        std::string n2 = "kenobi";
        g.InsertNode(n1);
        g.InsertNode(n2);

        int edge1 = 4;
        g.InsertEdge(n1, n2, edge1);

        WHEN("aa second edge is created in the same direction between the two "
             "nodes") {

            int edge2 = 5;
            bool res = g.InsertEdge(n1, n2, edge2);

            THEN("The graph should contain the newly created edge") {

                auto nodeList = g.GetNodeList();

                CHECK(res == true);
                CHECK(nodeList.size() == 2);

                auto node1 = nodeList[0];
                auto node2 = nodeList[1];
                auto edge_map = (*node1).GetEdges();

                auto edge_key = edge_map.begin()->first;
                auto edge_val = edge_map.begin()->second;

                CHECK((*node1).GetValue() == n1);
                CHECK((*node1).GetChildren().size() == 1);
                CHECK((*node1).GetParents().size() == 0);
                CHECK(edge_map.size() == 1);
                CHECK(edge_key == n2);
                CHECK(edge_val == std::vector<int>{edge1, edge2});

                // check dst remains the same
                CHECK((*node2).GetValue() == n2);
                CHECK((*node2).GetChildren().size() == 0);
                CHECK((*node2).GetParents().size() == 1);
                CHECK((*node2).GetEdges().size() == 0);
            }
        }
    }
}

SCENARIO("Insert duplicate edge between two nodes") {
    GIVEN("a graph contains two nodes connected by one edge") {
        gdwg::Graph<std::string, int> g;
        std::string n1 = "general";
        std::string n2 = "kenobi";
        g.InsertNode(n1);
        g.InsertNode(n2);

        int edge = 4;
        g.InsertEdge(n1, n2, edge);

        WHEN("we attempt to duplicate the edge") {

            bool res = g.InsertEdge(n1, n2, edge);

            THEN("The graph should remain the same") {

                auto nodeList = g.GetNodeList();

                CHECK(res == false);
                CHECK(nodeList.size() == 2);

                auto node1 = nodeList[0];
                auto node2 = nodeList[1];
                auto edge_map = (*node1).GetEdges();

                auto edge_key = edge_map.begin()->first;
                auto edge_val = edge_map.begin()->second;

                CHECK((*node1).GetValue() == n1);
                CHECK((*node1).GetChildren().size() == 1);
                CHECK((*node1).GetParents().size() == 0);
                CHECK(edge_map.size() == 1);
                CHECK(edge_key == n2);
                CHECK(edge_val == std::vector<int>{edge});

                // check dst remains the same
                CHECK((*node2).GetValue() == n2);
                CHECK((*node2).GetChildren().size() == 0);
                CHECK((*node2).GetParents().size() == 1);
                CHECK((*node2).GetEdges().size() == 0);
            }
        }
    }
}

SCENARIO("Insert edge with non-existent src") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        std::string n = "general";
        g.InsertNode(n);

        WHEN("we attempt to create an edge with a non-existent source") {
            int edge = 4;

            THEN("An exception is thrown") {
                CHECK_THROWS_WITH(g.InsertEdge("obi-wan", "kenobi", edge),
                        "Cannot call Graph::InsertEdge when either src or dst"
                        " node does not exist");
            }
        }
    }
}

SCENARIO("Insert edge with non-existent dst") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        std::string n = "general";
        g.InsertNode(n);

        WHEN("we attempt to create an edge with a non-existent destination") {
            int edge = 4;

            THEN("An exception is thrown") {
                CHECK_THROWS_WITH(g.InsertEdge("kenobi", "obi-wan", edge),
                        "Cannot call Graph::InsertEdge when either src or dst"
                        " node does not exist");
            }
        }
    }
}

/*********************/
/**  == Equality == **/
/*********************/

SCENARIO("Evaluate the equality of two graphs that have had nodes deleted") {
    GIVEN("two graphs") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("a");
        g.InsertNode("d");
        g.InsertNode("b");
        g.InsertEdge("a", "b", 10);
        g.InsertEdge("a", "b", 1);
        g.InsertEdge("a", "d", 4);
        g.InsertEdge("a", "d", 50);
        g.InsertEdge("b", "b", 2);
        g.InsertEdge("d", "b", 23);
        g.InsertEdge("d", "d", 5);

        // c is interconnected with rest of graph
        g.InsertNode("c");
        g.InsertEdge("a", "c", 999);
        g.InsertEdge("a", "c", -1);
        g.InsertEdge("c", "a", 3);
        g.InsertEdge("c", "c", 3);
        g.InsertEdge("c", "b", 3);
        g.InsertEdge("b", "c", 2);
        g.InsertEdge("d", "c", 12);
        g.InsertEdge("d", "c", 1);
        g.DeleteNode("c");

        gdwg::Graph<std::string, int> g2;
        g2.InsertNode("a");
        g2.InsertNode("d");
        g2.InsertNode("b");
        g2.InsertEdge("a", "b", 10);
        g2.InsertEdge("a", "b", 1);
        g2.InsertEdge("a", "d", 4);
        g2.InsertEdge("a", "d", 50);
        g2.InsertEdge("b", "b", 2);
        g2.InsertEdge("d", "b", 23);
        g2.InsertEdge("d", "d", 5);

        // x,y,z are disconnected with rest of graph
        g2.InsertNode("x");
        g2.InsertNode("y");
        g2.InsertNode("z");
        g2.InsertEdge("x", "y", 1);
        g2.InsertEdge("y", "y", 9);
        g2.InsertEdge("z", "x", 9);
        g2.InsertEdge("z", "y", 22);
        g2.DeleteNode("x");
        g2.DeleteNode("y");
        g2.DeleteNode("z");

        THEN("we can check their equality") {
            bool res = (g == g2);
            CHECK(res == true);
        }
    }
}


// TODO comparing graphs that have been created in 2 different ways
SCENARIO("Evaluate the equality of two differently constructed graphs") {
    GIVEN("two graphs") {
        gdwg::Graph<std::string, int> g;
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

        gdwg::Graph<std::string, int> g2;
        g2.InsertNode("b");
        g2.InsertEdge("b", "b", 2);
        g2.InsertNode("a");
        g2.InsertEdge("a", "b", 10);
        g2.InsertEdge("a", "b", 1);
        g2.InsertNode("c");
        g2.InsertEdge("c", "b", 3);
        g2.InsertEdge("b", "c", 2);
        g2.InsertNode("d");
        g2.InsertEdge("a", "d", 4);
        g2.InsertEdge("a", "d", 50);
        g2.InsertEdge("d", "b", 23);
        g2.InsertEdge("d", "d", 5);

        THEN("we can check their equality") {
            bool res = (g == g2);
            CHECK(res == true);
        }
    }
}

SCENARIO("Evaluate the equality of two graph") {
    GIVEN("two graphs") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("hello");
        g.InsertNode("you?");
        g.InsertNode("how");
        g.InsertNode("are");

        g.InsertEdge("hello", "how", 5);
        g.InsertEdge("hello", "are", 8);
        g.InsertEdge("hello", "are", 2);
        g.InsertEdge("how", "you?", 1);
        g.InsertEdge("how", "hello", 4);
        g.InsertEdge("are", "you?", 3);

        gdwg::Graph<std::string, int> g2;
        g2.InsertNode("hello");
        g2.InsertNode("you?");
        g2.InsertNode("how");
        g2.InsertNode("are");

        g2.InsertEdge("hello", "how", 5);
        g2.InsertEdge("hello", "are", 8);
        g2.InsertEdge("hello", "are", 2);
        g2.InsertEdge("how", "you?", 1);
        g2.InsertEdge("how", "hello", 4);
        g2.InsertEdge("are", "you?", 3);

        THEN("we can check their equality") {
            bool res = (g == g2);
            CHECK(res == true);
        }
    }
}

SCENARIO("Evaluate the inequality of two graphs") {
    GIVEN("two equal graphs") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("hello");
        g.InsertNode("you?");
        g.InsertNode("how");
        g.InsertNode("are");

        g.InsertEdge("hello", "how", 5);
        g.InsertEdge("hello", "are", 8);
        g.InsertEdge("hello", "are", 2);
        g.InsertEdge("how", "you?", 1);
        g.InsertEdge("how", "hello", 4);
        g.InsertEdge("are", "you?", 3);

        gdwg::Graph<std::string, int> g2;
        g2.InsertNode("hello");
        g2.InsertNode("you?");
        g2.InsertNode("how");
        g2.InsertNode("are");

        g2.InsertEdge("hello", "how", 5);
        g2.InsertEdge("hello", "are", 8);
        g2.InsertEdge("hello", "are", 2);
        g2.InsertEdge("how", "you?", 1);
        g2.InsertEdge("how", "hello", 4);
        g2.InsertEdge("are", "you?", 3);

        CHECK((g == g2) == true);

        WHEN("the first graph has changed") {
            g.InsertNode("spanner");

            THEN("the two graphs are no longer equal") {
                CHECK((g != g2) == true);
            }
        }
    }
}


/***********************/
/**  == DeleteNode == **/
/***********************/

SCENARIO("Delete a solitary node") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("hello");
        g.InsertNode("you?");
        g.InsertNode("how");
        g.InsertNode("are");

        g.InsertEdge("hello", "how", 5);
        g.InsertEdge("hello", "are", 8);
        g.InsertEdge("hello", "are", 2);
        g.InsertEdge("how", "hello", 4);

        WHEN("we delete a node") {
            bool res = g.DeleteNode("you?");

            THEN("the deleted node is no longer in the graph") {
                gdwg::Graph<std::string, int> expected;
                expected.InsertNode("hello");
                expected.InsertNode("how");
                expected.InsertNode("are");
                expected.InsertEdge("hello", "how", 5);
                expected.InsertEdge("hello", "are", 8);
                expected.InsertEdge("hello", "are", 2);
                expected.InsertEdge("how", "hello", 4);

                CHECK(res == true);
                CHECK((g == expected) == true);
            }

        }
    }
}

SCENARIO("Delete a node with children and parents") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("hello");
        g.InsertNode("you?");
        g.InsertNode("are");

        g.InsertEdge("hello", "are", 8);
        g.InsertEdge("hello", "are", 2);
        g.InsertEdge("are", "you?", 3);

        WHEN("we delete a node with parents and children") {
            bool res = g.DeleteNode("are");

            THEN("the deleted node is no longer in the graph") {
                gdwg::Graph<std::string, int> expected;
                expected.InsertNode("hello");
                expected.InsertNode("you?");

                CHECK(res == true);
                CHECK((g == expected) == true);
            }
        }
    }
}

SCENARIO("Delete a non-existent node") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;

        THEN("we can delete a non-existent node") {
            bool res = g.DeleteNode("dummy");
            CHECK(res == false);
        }
    }
}


/*******************/
/**  == IsNode == **/
/*******************/

SCENARIO("Check if node is present") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, std::string> g;
        std::string n = "general";
        g.InsertNode(n);

        THEN("we can know whether a given node is present in the graph") {
            bool general_res = g.IsNode("general");
            bool kenobi_res = g.IsNode("kenobi");

            CHECK(general_res == true);
            CHECK(kenobi_res == false);
        }
    }
}


/************************/
/**  == IsConnected == **/
/************************/

SCENARIO("Check if two nodes are connected") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        std::string n1 = "general";
        std::string n2 = "kenobi";
        std::string n3 = "anakin";
        g.InsertNode(n1);
        g.InsertNode(n2);
        g.InsertNode(n3);

        int edge = 4;
        g.InsertEdge(n1, n2, edge);

        THEN("we can know whether an edge is present between two nodes") {
            bool conn_res = g.IsConnected(n1, n2);
            bool non_conn_res = g.IsConnected(n1, n3);

            CHECK(conn_res == true);
            CHECK(non_conn_res == false);
        }
    }
}

SCENARIO("Check if a non-existent src node is connected") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        std::string n1 = "general";
        std::string n2 = "kenobi";
        g.InsertNode(n1);
        g.InsertNode(n2);

        int edge = 4;
        g.InsertEdge(n1, n2, edge);

        WHEN("we try to see is a non-existent src node is connected to "
             "another") {
            std::string n3 = "anakin";

            THEN("An error is thrown") {
                CHECK_THROWS_WITH(g.IsConnected(n3, n2), "Cannot call "
                        "Graph::IsConnected if src or dst node don't exist in"
                        " the graph");
            }
        }
    }
}

SCENARIO("Check if a non-existent dst node is connected") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        std::string n1 = "general";
        std::string n2 = "kenobi";
        g.InsertNode(n1);
        g.InsertNode(n2);

        int edge = 4;
        g.InsertEdge(n1, n2, edge);

        WHEN("we try to see is a non-existent src node is connected to "
             "another") {
            std::string n3 = "anakin";

            THEN("An error is thrown") {
                CHECK_THROWS_WITH(g.IsConnected(n2, n3), "Cannot call "
                        "Graph::IsConnected if src or dst node don't exist in"
                        " the graph");
            }
        }
    }
}


/******************/
/**  == Clear == **/
/******************/

SCENARIO("Clear a graph") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("hello");
        g.InsertNode("you?");
        g.InsertNode("how");
        g.InsertNode("are");

        g.InsertEdge("hello", "how", 5);
        g.InsertEdge("hello", "are", 8);
        g.InsertEdge("hello", "are", 2);
        g.InsertEdge("how", "hello", 4);

        WHEN("we clear the graph") {
            auto nodeList = g.GetNodeList();
            CHECK(nodeList.size() == 4);
            g.Clear();

            THEN("the graph is empty") {
                nodeList = g.GetNodeList();
                CHECK(nodeList.size() == 0);
            }
        }
    }
}



/************************/
/**  == ReplaceNode == **/
/************************/

SCENARIO("Replace a node with children and parents") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("hello");
        g.InsertNode("you?");
        g.InsertNode("are");

        g.InsertEdge("hello", "are", 8);
        g.InsertEdge("hello", "are", 2);
        g.InsertEdge("are", "you?", 3);

        WHEN("we replace a node with parents and children") {
            bool res = g.Replace("are", "chicken");

            THEN("the node value is replaced in the graph") {
                gdwg::Graph<std::string, int> expected;
                expected.InsertNode("hello");
                expected.InsertNode("you?");
                expected.InsertNode("chicken");

                expected.InsertEdge("hello", "chicken", 8);
                expected.InsertEdge("hello", "chicken", 2);
                expected.InsertEdge("chicken", "you?", 3);

                CHECK(res == true);
                CHECK((g == expected) == true);
            }
        }
    }
}

SCENARIO("Replace a non-existent node") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("hello");
        g.InsertNode("you?");
        g.InsertNode("are");

        g.InsertEdge("hello", "are", 8);
        g.InsertEdge("hello", "are", 2);
        g.InsertEdge("are", "you?", 3);

        WHEN("we replace a non-existent node with parents and children") {
            std::string non_existent_node = "how";

            THEN("An exception is thrown") {
                CHECK_THROWS_WITH(g.Replace(non_existent_node, "chicken"),
                    "Cannot call Graph::Replace on a node that doesn't exist");
            }
        }
    }
}

/*************************/
/**  == MergeReplace == **/
/*************************/

SCENARIO("MergeReplace with a non-connected node") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("c");
        g.InsertNode("d");
        g.InsertNode("b");
        g.InsertNode("a");
        g.InsertNode("e");
        g.InsertEdge("a", "b", 10);
        g.InsertEdge("a", "b", 1);
        g.InsertEdge("a", "d", 4);
        g.InsertEdge("a", "d", 50);
        g.InsertEdge("b", "b", 2);
        g.InsertEdge("c", "b", 3);
        g.InsertEdge("b", "c", 2);
        g.InsertEdge("d", "b", 23);
        g.InsertEdge("d", "d", 5);
        WHEN("we replace a node with parents and children with an isolated node") {
            g.MergeReplace("d", "e");
            THEN("we get the correct result") {
                gdwg::Graph<std::string, int> expected;
                expected.InsertNode("c");
                expected.InsertNode("b");
                expected.InsertNode("a");
                expected.InsertNode("e");
                expected.InsertEdge("a", "b", 10);
                expected.InsertEdge("a", "b", 1);
                expected.InsertEdge("a", "e", 4);
                expected.InsertEdge("a", "e", 50);
                expected.InsertEdge("b", "b", 2);
                expected.InsertEdge("c", "b", 3);
                expected.InsertEdge("b", "c", 2);
                expected.InsertEdge("e", "b", 23);
                expected.InsertEdge("e", "e", 5);
                CHECK(g == expected);
            }
        }
    }
}

SCENARIO("MergeReplace a self-connected node") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
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
        WHEN("we replace a node with self edges") {
            g.MergeReplace("d", "a");
            THEN("we get the correct result") {
                gdwg::Graph<std::string, int> expected;
                expected.InsertNode("c");
                expected.InsertNode("b");
                expected.InsertNode("a");
                expected.InsertEdge("a", "b", 10);
                expected.InsertEdge("a", "b", 1);
                expected.InsertEdge("a", "a", 4);
                expected.InsertEdge("a", "a", 50);
                expected.InsertEdge("b", "b", 2);
                expected.InsertEdge("c", "b", 3);
                expected.InsertEdge("b", "c", 2);
                expected.InsertEdge("a", "b", 23);
                expected.InsertEdge("a", "a", 5);
                CHECK(g == expected);
            }
        }
    }
}

SCENARIO("MergeReplace a non-connected node") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("c");
        g.InsertNode("d");
        g.InsertNode("b");
        g.InsertNode("a");
        g.InsertNode("e");
        g.InsertEdge("a", "b", 10);
        g.InsertEdge("a", "b", 1);
        g.InsertEdge("a", "d", 4);
        g.InsertEdge("a", "d", 50);
        g.InsertEdge("b", "b", 2);
        g.InsertEdge("c", "b", 3);
        g.InsertEdge("b", "c", 2);
        g.InsertEdge("d", "b", 23);
        g.InsertEdge("d", "d", 5);
        WHEN("we replace a node with no parents and children with a connected node") {
            g.MergeReplace("e", "d");
            THEN("we get the correct result") {
                gdwg::Graph<std::string, int> expected;
                expected.InsertNode("c");
                expected.InsertNode("d");
                expected.InsertNode("b");
                expected.InsertNode("a");
                expected.InsertEdge("a", "b", 10);
                expected.InsertEdge("a", "b", 1);
                expected.InsertEdge("a", "d", 4);
                expected.InsertEdge("a", "d", 50);
                expected.InsertEdge("b", "b", 2);
                expected.InsertEdge("c", "b", 3);
                expected.InsertEdge("b", "c", 2);
                expected.InsertEdge("d", "b", 23);
                expected.InsertEdge("d", "d", 5);
                CHECK(g == expected);
            }
        }
    }
}

SCENARIO("MergeReplace a node with itself") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
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
        WHEN("we MergeReplace a node with itself") {
            g.MergeReplace("a", "a");
            THEN("we get the same graph") {
                gdwg::Graph<std::string, int> expected;
                expected.InsertNode("c");
                expected.InsertNode("d");
                expected.InsertNode("b");
                expected.InsertNode("a");
                expected.InsertEdge("a", "b", 10);
                expected.InsertEdge("a", "b", 1);
                expected.InsertEdge("a", "d", 4);
                expected.InsertEdge("a", "d", 50);
                expected.InsertEdge("b", "b", 2);
                expected.InsertEdge("c", "b", 3);
                expected.InsertEdge("b", "c", 2);
                expected.InsertEdge("d", "b", 23);
                expected.InsertEdge("d", "d", 5);
                CHECK(g == expected);
            }
        }
    }
}

/***************************/
/**  == Printing Graph == **/
/***************************/

SCENARIO("Printing empty graph") {
    GIVEN("an empty graph") {
        gdwg::Graph<std::string, int> g;
        WHEN("we print it") {
            std::stringstream buffer;
            buffer << g;
            THEN("we get no printing") {
                std::string expected = "";
                CHECK(buffer.str() == expected);
            }
        }
    }
}

SCENARIO("Printing an unconnected graph") {
    GIVEN("an unconnected graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("c");
        g.InsertNode("d");
        g.InsertNode("b");
        g.InsertNode("a");
        WHEN("we print it") {
            std::stringstream buffer;
            buffer << g;
            THEN("we get the correct output") {
                std::string expected = "a (\n)\nb (\n)\nc (\n)\nd (\n)\n";
                CHECK(buffer.str() == expected);
            }
        }
    }
}

SCENARIO("Printing a complex graph") {
    GIVEN("a complex graph") {
        gdwg::Graph<std::string, int> g;
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
        WHEN("we print it") {
            std::stringstream buffer;
            buffer << g;
            THEN("we get the correct output") {
                std::string expected = "a (\n  b | 1\n  b | 10\n  d | 4\n  d | 50\n)\nb (\n  b | 2\n  c | 2\n)\nc (\n  b | 3\n)\nd (\n  b | 23\n  d | 5\n)\n";
                CHECK(buffer.str() == expected);
            }
        }
    }
}

SCENARIO("Printing a disjointed graph") {
    GIVEN("a disjointed graph") {
        gdwg::Graph<std::string, int> g;
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
        WHEN("we print it") {
            std::stringstream buffer;
            buffer << g;
            THEN("we get the correct output") {
                std::string expected = "a (\n  b | 1\n  b | 10\n  d | 4\n  d | 50\n)\nb (\n  b | 2\n  c | 2\n)\nc (\n  b | 3\n)\nd (\n  b | 23\n  d | 5\n)\nw (\n  w | 23\n  x | 10\n  y | 4\n)\nx (\n  z | 3\n)\ny (\n  y | 5\n)\nz (\n  w | 2\n  x | 2\n  y | 50\n  z | 1\n)\n";
                CHECK(buffer.str() == expected);
            }
        }
    }
}
/*********************/
/**  == GetNodes == **/
/*********************/

SCENARIO("Getting nodes vector of empty graph") {
    GIVEN("an empty graph") {
        gdwg::Graph<std::string, int> g;
        WHEN("calling GetNodes") {
            std::vector<std::string> res = g.GetNodes();
            THEN("an empty vector is returned") {
                std::vector<std::string> expected;
                CHECK(res == expected);
            }
        }
    }
}

SCENARIO("Getting nodes vector of graph with one node") {
    GIVEN("a single node graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("a");
        WHEN("calling GetNodes") {
            std::vector<std::string> res = g.GetNodes();
            THEN("the correct vector is returned") {
                std::vector<std::string> expected{"a"};
                CHECK(res == expected);
            }
        }
    }
}

SCENARIO("Getting nodes vector of graph with multiple nodes") {
    GIVEN("a multiple node graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("a");
        g.InsertNode("d");
        g.InsertNode("c");
        g.InsertNode("b");
        WHEN("calling GetNodes") {
            std::vector<std::string> res = g.GetNodes();
            THEN("the correct vector is returned") {
                std::vector<std::string> expected{"a", "b", "c", "d"};
                CHECK(res == expected);
            }
        }
    }
}


/*************************/
/**  == GetConnected == **/
/*************************/

SCENARIO("Getting connected nodes of an isolated node of graph") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
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
        WHEN("calling GetConnected") {
            std::vector<std::string> res = g.GetConnected("x");
            THEN("the correct vector is returned") {
                std::vector<std::string> expected;
                CHECK(res == expected);
            }
        }
    }
}

SCENARIO("Getting connected nodes of connected node in graph") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
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
        WHEN("calling GetConnected") {
            std::vector<std::string> res = g.GetConnected("a");
            THEN("the correct vector is returned") {
                std::vector<std::string> expected{"b", "d"};
                CHECK(res == expected);
            }
        }
    }
}

SCENARIO("Getting connected nodes of a node with self edges") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
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
        WHEN("calling GetConnected") {
            std::vector<std::string> res = g.GetConnected("b");
            THEN("the correct vector is returned") {
                std::vector<std::string> expected{"b", "c"};
                CHECK(res == expected);
            }
        }
    }
}

/***********************/
/**  == GetWeights == **/
/***********************/

SCENARIO("Getting edges between unconnected nodes") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("c");
        g.InsertNode("d");
        g.InsertNode("b");
        g.InsertNode("a");
        g.InsertEdge("a", "b", 10);
        g.InsertEdge("a", "b", 1);
        g.InsertEdge("a", "d", 4);
        g.InsertEdge("a", "d", 50);
        g.InsertEdge("c", "b", 3);
        g.InsertEdge("b", "b", 2);
        g.InsertEdge("b", "c", 2);
        g.InsertEdge("d", "b", 23);
        g.InsertEdge("d", "d", 5);
        WHEN("calling GetConnected") {
            std::vector<int> res = g.GetWeights("a", "c");
            THEN("an empty vector is returned") {
                std::vector<int> expected;
                CHECK(res == expected);
            }
        }
    }
}

SCENARIO("Getting edges between two different connected nodes") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("c");
        g.InsertNode("d");
        g.InsertNode("b");
        g.InsertNode("a");
        g.InsertEdge("a", "b", 10);
        g.InsertEdge("a", "b", 1);
        g.InsertEdge("a", "d", 4);
        g.InsertEdge("a", "d", 50);
        g.InsertEdge("c", "b", 3);
        g.InsertEdge("b", "b", 2);
        g.InsertEdge("b", "c", 2);
        g.InsertEdge("d", "b", 23);
        g.InsertEdge("d", "d", 5);
        WHEN("calling GetConnected") {
            std::vector<int> res = g.GetWeights("a", "d");
            THEN("an empty vector is returned") {
                std::vector<int> expected {4,50};
                CHECK(res == expected);
            }
        }
    }
}

SCENARIO("Getting edges between a self connected node") {
    GIVEN("a graph") {
        gdwg::Graph<std::string, int> g;
        g.InsertNode("c");
        g.InsertNode("d");
        g.InsertNode("b");
        g.InsertNode("a");
        g.InsertEdge("a", "b", 10);
        g.InsertEdge("a", "b", 1);
        g.InsertEdge("a", "d", 4);
        g.InsertEdge("a", "d", 50);
        g.InsertEdge("c", "b", 3);
        g.InsertEdge("b", "b", 2);
        g.InsertEdge("b", "b", 9);
        g.InsertEdge("b", "b", 1);
        g.InsertEdge("b", "b", 4);
        g.InsertEdge("b", "c", 2);
        g.InsertEdge("d", "b", 23);
        g.InsertEdge("d", "d", 5);
        WHEN("calling GetConnected") {
            std::vector<int> res = g.GetWeights("b", "b");
            THEN("an empty vector is returned") {
                std::vector<int> expected{1,2,4,9};
                CHECK(res == expected);
            }
        }
    }
}
//SCENARIO("") {
//    WHEN("") {
//        gdwg::Graph<std::string, int> g;
//
//        THEN("") {
//
//        }
//    }
//}
//
//SCENARIO("") {
//    WHEN("") {
//        gdwg::Graph<std::string, int> g;
//
//        THEN("") {
//
//        }
//    }
//}
//
//SCENARIO("") {
//    WHEN("") {
//        gdwg::Graph<std::string, int> g;
//
//        THEN("") {
//
//        }
//    }
//}
//
//SCENARIO("") {
//    WHEN("") {
//        gdwg::Graph<std::string, int> g;
//
//        THEN("") {
//
//        }
//    }
//}
//
//
//SCENARIO("") {
//    WHEN("") {
//        gdwg::Graph<std::string, int> g;
//
//        THEN("") {
//
//        }
//    }
//}
//
//SCENARIO("") {
//    WHEN("") {
//        gdwg::Graph<std::string, int> g;
//
//        THEN("") {
//
//        }
//    }
//}