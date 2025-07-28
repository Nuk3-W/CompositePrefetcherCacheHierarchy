#pragma once

#include <variant>
#include <cstdint>

using Address = uint32_t;

struct Hit {};
struct Miss {};
struct Evict { Address addr; };
struct Prefetch { Address addr; };

using AccessResult = std::variant<Hit, Miss, Evict, Prefetch>;