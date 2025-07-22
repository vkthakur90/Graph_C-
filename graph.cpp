#include <iostream>
#include <vector>
#include <cstddef>
#include <limits>
#include <algorithm>

// Status codes
enum GraphStatus {
    GRAPH_SUCCESS = 0,
    GRAPH_ERR_INVALID_PARENT,
    GRAPH_ERR_INVALID_NODE,
    GRAPH_ERR_INVALID_EDGE
};

struct Graph {
    std::vector<double> node_data{};
    std::vector<std::vector<std::size_t>> adj{};
};

// Add a node with value 'val'. If parent != SIZE_MAX, also add an edge from parent to new node.
void Graph_addNode(Graph &g,
                   const double val,
                   const std::size_t parent,
                   std::size_t &out_node,
                   GraphStatus &status) noexcept {
    const std::size_t new_index = g.node_data.size();
    g.node_data.push_back(val);
    g.adj.emplace_back(); // Create empty adjacency list for new node
    out_node = new_index;

    if (parent != std::numeric_limits<std::size_t>::max()) {
        if (parent < new_index) {
            g.adj[parent].push_back(new_index);
            status = GRAPH_SUCCESS;
        } else {
            status = GRAPH_ERR_INVALID_PARENT;
        }
    } else {
        status = GRAPH_SUCCESS;
    }
}

// Add an edge from 'source' to 'target'
void Graph_addEdge(Graph &g,
                   const std::size_t source,
                   const std::size_t target,
                   GraphStatus &status) noexcept {
    if (source >= g.node_data.size() || target >= g.node_data.size()) {
        status = GRAPH_ERR_INVALID_EDGE;
        return;
    }
    g.adj[source].push_back(target);
    status = GRAPH_SUCCESS;
}

// Remove an edge from 'source' to 'target' (first occurrence)
void Graph_removeEdge(Graph &g,
                      const std::size_t source,
                      const std::size_t target,
                      GraphStatus &status) noexcept {
    if (source >= g.adj.size()) {
        status = GRAPH_ERR_INVALID_EDGE;
        return;
    }
    auto &neighbors = g.adj[source];
    auto it = std::find(neighbors.begin(), neighbors.end(), target);
    if (it != neighbors.end()) {
        neighbors.erase(it);
        status = GRAPH_SUCCESS;
    } else {
        status = GRAPH_ERR_INVALID_EDGE;
    }
}

// Remove a node and all incident edges; reindex everything
void Graph_removeNode(Graph &g,
                      const std::size_t node,
                      GraphStatus &status) noexcept {
    if (node >= g.node_data.size()) {
        status = GRAPH_ERR_INVALID_NODE;
        return;
    }

    g.node_data.erase(g.node_data.begin() + node);
    g.adj.erase(g.adj.begin() + node);

    for (auto &neighbors : g.adj) {
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), node), neighbors.end());
        std::for_each(neighbors.begin(), neighbors.end(), [node](std::size_t &n) {
            if (n > node) --n;
        });
    }

    status = GRAPH_SUCCESS;
}

// Print the graph
void Graph_print(const Graph &g) {
    std::cout << "Graph Nodes and Adjacency Lists:\n";
    for (std::size_t i = 0; i < g.node_data.size(); ++i) {
        std::cout << "Node " << i << " (" << g.node_data[i] << ") -> ";
        std::for_each(g.adj[i].begin(), g.adj[i].end(), [](std::size_t target) {
            std::cout << target << " ";
        });
        std::cout << "\n";
    }
}

// Example usage
int main() {
    Graph g;
    std::size_t root, child;
    GraphStatus status;

    Graph_addNode(g, 1.23, std::numeric_limits<std::size_t>::max(), root, status);
    std::cout << "Added root node #" << root << " status=" << status << "\n";

    Graph_addNode(g, 4.56, root, child, status);
    std::cout << "Added child node #" << child << " status=" << status << "\n";

    Graph_addEdge(g, root, child, status);
    std::cout << "Added edge (" << root << "," << child << ") status=" << status << "\n";

    Graph_print(g);

    Graph_removeEdge(g, root, child, status);
    std::cout << "Removed edge status=" << status << "\n";

    Graph_removeNode(g, child, status);
    std::cout << "Removed node #" << child << " status=" << status << "\n";

    Graph_print(g);

    return 0;
}