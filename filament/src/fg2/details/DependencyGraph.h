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
 * A very simple dependency graph (DAG) class that support culling of unused nodes
 */
class DependencyGraph {
public:
    DependencyGraph() noexcept;
    DependencyGraph(const DependencyGraph&) noexcept = delete;
    DependencyGraph& operator=(const DependencyGraph&) noexcept = delete;

    /**
     * A generic node
     */
    class Node {
    public:
        Node(DependencyGraph& graph) noexcept;
        Node(Node const&) noexcept = delete;
        Node& operator=(Node const&) noexcept = delete;
        Node& operator=(Node&&) noexcept = delete;

        Node(Node&&) noexcept = default;

        virtual ~Node() noexcept;

        //! returns a unique id for this node
        uint32_t getId() const noexcept;

        /**
         * Add a link to a node and increase its reference count.
         * No check is made that the graph stays acyclic.
         * @param node Node to link to.
         */
        void linkTo(Node* node) noexcept;

        //! make this node a leaf node
        void makeLeaf() noexcept;

        //! returns the list of nodes we're linking to
        std::vector<Node*> const& getLinks() const noexcept;

        /**
         * return the reference count of this node. That is how many other nodes have links to us.
         * @return Number of nodes linking to this one.
         */
        uint32_t getRefCount() const noexcept;

        //! remove a reference from us and return the new reference count.
        uint32_t decRef() noexcept;

        /**
         * Returns whether this node was culled.
         * This is only valid after DependencyGraph::cull() has been called.
         * @return true if the node has been culled, false otherwise.
         */
        bool isCulled() const noexcept;

    public:
        //! return the name of this node
        virtual char const* getName() const = 0;

        //! called from DependencyGraph::cull() when a node a culled
        virtual void onCulled() = 0;

    private:
        // nodes that read from us: i.e. we have a reference to them
        std::vector<Node*> mLinks;  // nodes we are linked to
        uint32_t mRefCount = 0;     // how many references to us
        const uint32_t mId;         // unique id
    };

    //! cull unreferenced nodes. Links ARE NOT removed, only reference counts are updated.
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
