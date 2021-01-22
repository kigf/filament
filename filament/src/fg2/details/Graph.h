/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TNT_FILAMENT_FG2_GRAPH_H
#define TNT_FILAMENT_FG2_GRAPH_H

#include <utils/ostream.h>

#include <vector>

namespace filament::fg2 {

/**
 * A very simple graph class that support culling of unused nodes
 */
class Graph {
public:
    Graph() noexcept;
    Graph(const Graph&) noexcept = delete;
    Graph& operator=(const Graph&) noexcept = delete;

    /**
     * A generic node
     */
    class Node {
    public:
        Node(Graph& graph) noexcept;
        Node(const Node&) noexcept = delete;
        Node& operator=(const Node&) noexcept = delete;

        Node(Node&&) noexcept = default;

        virtual ~Node() noexcept;

        //! Add a reference to a node
        void addReferenceTo(Node* node) noexcept;

        //! make this node a leaf node
        void makeLeaf() noexcept;

        /**
         * return the reference count of this node.
         * This is only valid after Graph::cull() has been called.
         * @return reference count of this node. 0 if culled.
         */
        uint32_t getRefCount() const noexcept;

        /**
         * Returns whether this node was culled.
         * This is only valid after Graph::cull() has been called.
         * @return true if the node has been culled, false otherwise
         */
        bool isCulled() const noexcept;

    private:
        //! return the name of this node
        virtual char const* getName() const = 0;

        //! called from Graph::cull() when a node a culled
        virtual void onCulled() = 0;

    private:
        friend class Graph;
        uint32_t getId() const noexcept;

        // nodes that read from us: i.e. we have a reference to them
        std::vector<Node*> mReferences;
        // how many references to us
        uint32_t mRefCount = 0;
        uint32_t mId = 0;
    };

    //! cull unreferenced nodes
    void cull() noexcept;

    //! export a graphviz view of the graph
    void export_graphviz(utils::io::ostream& out, const char* name = nullptr);

private:
    uint32_t generateNodeId() noexcept;
    void registerNode(Node* node) noexcept;
    std::vector<Node*> mNodes;
};

} // namespace filament::fg2

#endif //TNT_FILAMENT_FG2_GRAPH_H
