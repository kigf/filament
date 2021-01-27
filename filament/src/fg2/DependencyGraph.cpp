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

#include "fg2/details/DependencyGraph.h"

namespace filament::fg2 {

DependencyGraph::DependencyGraph() noexcept = default;

uint32_t DependencyGraph::generateNodeId() noexcept {
    return mNodes.size();
}

void DependencyGraph::registerNode(Node* node) noexcept {
    // node* is not fully constructed here
    mNodes.push_back(node);
}

void DependencyGraph::cull() noexcept {
    auto& nodes = mNodes;

    // cull nodes with a 0 reference count
    std::vector<Node*> stack;
    stack.reserve(nodes.size());
    for (Node* const pNode : nodes) {
        if (pNode->getRefCount() == 0) {
            stack.push_back(pNode);
        }
    }
    while (!stack.empty()) {
        Node* const pNode = stack.back();
        stack.pop_back();
        auto const& links = pNode->getLinks();
        for (Node* pLinkedNode : links) {
            if (pLinkedNode->decRef() == 0) {
                stack.push_back(pLinkedNode);
            }
        }
        pNode->onCulled();
    }
}

void DependencyGraph::export_graphviz(utils::io::ostream& out, char const* name) {
#ifndef NDEBUG
    const char* graphName = name ? name : "graph";
    out << "digraph \"" << graphName << "\" {\n";
    out << "rankdir = LR\n";
    out << "bgcolor = black\n";
    out << "node [shape=rectangle, fontname=\"helvetica\", fontsize=10]\n\n";

    auto const& nodes = mNodes;

    for (Node const* node : nodes) {
        uint32_t id = node->getId();
        const char* const nodeName = node->getName();
        uint32_t refCount = node->getRefCount();

        out << "\"N" << id << "\" [label=\"" << nodeName
            << "\\nrefs: " << refCount
            << "\\nseq: " << id
            << "\", style=filled, fillcolor="
            << (refCount ? "darkorange" : "darkorange4") << "]\n";
    }

    out << "\n";
    for (Node const* node : nodes) {
        uint32_t id = node->getId();
        out << "N" << id << " -> { ";
        for (Node const* ref : node->getLinks()) {
            out << "N" << ref->getId() << " ";
        }
        out << "} [color=red2]\n";
    }

    out << "}" << utils::io::endl;
#endif
}

// ------------------------------------------------------------------------------------------------

DependencyGraph::Node::Node(DependencyGraph& graph) noexcept : mId(graph.generateNodeId()) {
    graph.registerNode(this);
}

DependencyGraph::Node::~Node() noexcept = default;

void DependencyGraph::Node::linkTo(Node* node) noexcept {
    node->mRefCount++;
    mLinks.push_back(node);
}

void DependencyGraph::Node::makeLeaf() noexcept {
    assert(!mRefCount);
    mRefCount = 1;
}

std::vector<DependencyGraph::Node*> const& DependencyGraph::Node::getLinks() const noexcept {
    return mLinks;
}

uint32_t DependencyGraph::Node::getRefCount() const noexcept {
    return mRefCount;
}

uint32_t DependencyGraph::Node::getId() const noexcept {
    return mId;
}

bool DependencyGraph::Node::isCulled() const noexcept {
    return mRefCount == 0;
}

uint32_t DependencyGraph::Node::decRef() noexcept {
    assert(mRefCount >= 1);
    return --mRefCount;
}

} // namespace filament::fg2
