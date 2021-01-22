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

#include "fg2/FrameGraph.h"
#include "fg2/details/PassNode.h"

namespace filament::fg2 {

FrameGraph::FrameGraph(ResourceAllocatorInterface& resourceAllocator)
        : mResourceAllocator(resourceAllocator),
          mArena("FrameGraph Arena", 131072)
{
}

FrameGraph::~FrameGraph() = default;


FrameGraph& FrameGraph::compile() noexcept {
    return *this;
}

void FrameGraph::execute(backend::DriverApi& driver) noexcept {
}

void FrameGraph::present(FrameGraphHandle input) {
}

FrameGraphId<Texture> FrameGraph::import(char const* name, Texture::Descriptor const& desc,
        backend::Handle<backend::HwRenderTarget> target) {
    return FrameGraphId<Texture>();
}

PassNode& FrameGraph::createPass(char const* name, PassExecutor* base) noexcept {
    auto& passNodes = mPassNodes;
    const uint32_t id = (uint32_t)passNodes.size();
    return passNodes.emplace_back(*this, name, id, base);
}

} // namespace filament::fg2
