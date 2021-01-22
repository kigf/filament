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

#include "fg2/details/Graph.h"

namespace filament::fg2 {

Graph::Graph() noexcept = default;

uint32_t Graph::generateNodeId() noexcept {
    return mNodes.size();
}

void Graph::registerNode(Node* node) noexcept {
    // node* is not fully constructed here
    mNodes.push_back(node);
}

void Graph::cull() noexcept {
    auto& nodes = mNodes;
    // update the reference counts
    for (Node* pNode : nodes) {
        auto& references = pNode->mReferences;
        for (Node* pReaders : references) {
            pReaders->mRefCount++;
        }
    }
    // cull nodes with a 0 reference count
    std::vector<Node*> stack;
    for (Node* pNode : nodes) {
        if (pNode->mRefCount == 0) {
            stack.push_back(pNode);
        }
    }
    while (!stack.empty()) {
        Node* const pNode = stack.back();
        stack.pop_back();
        auto& references = pNode->mReferences;
        for (Node* pReference : references) {
            assert(pReference->mRefCount >= 1);
            if (--pReference->mRefCount == 0) {
                stack.push_back(pReference);
            }
        }
        pNode->onCulled();
    }
}

void Graph::export_graphviz(utils::io::ostream& out, char const* name) {
#ifndef NDEBUG
    const char* label = name ? name : "graph";
    out << "digraph \"" << label << "\" {\n";
    out << "rankdir = LR\n";
    out << "bgcolor = black\n";
    out << "node [shape=rectangle, fontname=\"helvetica\", fontsize=10]\n\n";

    auto const& nodes = mNodes;

    for (Node const* node : nodes) {
        uint32_t id = node->getId();
        const char* const label = node->getName();
        uint32_t refCount = node->getRefCount();

        out << "\"N" << id << "\" [label=\"" << label
            << "\\nrefs: " << refCount
            << "\\nseq: " << id
            << "\", style=filled, fillcolor="
            << (refCount ? "darkorange" : "darkorange4") << "]\n";
    }

    out << "\n";
    for (Node const* node : nodes) {
        uint32_t id = node->getId();
        out << "N" << id << " -> { ";
        for (Node const* ref : node->mReferences) {
            out << "N" << ref->getId() << " ";
        }
        out << "} [color=red2]\n";
    }

    out << "}" << utils::io::endl;
#endif
}

// ------------------------------------------------------------------------------------------------

Graph::Node::Node(Graph& graph) noexcept : mId(graph.generateNodeId()) {
    graph.registerNode(this);
}

Graph::Node::~Node() noexcept = default;

void Graph::Node::addReferenceTo(Node* node) noexcept {
    mReferences.push_back(node);
}

void Graph::Node::makeLeaf() noexcept {
    assert(!mRefCount);
    mRefCount = 1;
}

uint32_t Graph::Node::getRefCount() const noexcept {
    return mRefCount;
}

uint32_t Graph::Node::getId() const noexcept {
    return mId;
}

bool Graph::Node::isCulled() const noexcept {
    return mRefCount == 0;
}

} // namespace filament::fg2
