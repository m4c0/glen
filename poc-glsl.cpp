#pragma leco tool
// TODO: remove this after wrapping enough utilities
extern "C" {
#include "tree-sitter/lib/include/tree_sitter/api.h"
}

import glen;
import jojo;
import print;
import sv;

int main() try {
  constexpr const auto src = R"(#version 450
#pragma something weird

layout(location = 0) in vec2 pos;

void main() {
  gl_Position = vec4(pos, 0, 1);
}
  )"_sv;

  glen::parser p { tree_sitter_glsl };
  auto t = p.parse(src);

  putln(ts_node_string(t.root_node()));

  t.for_each_match(R"(
    (preproc_call directive: (_) @d argument: (_) @a)
  )"_sv, [src=src.begin()](auto & m) {
    auto s = ts_node_start_byte(m.captures[0].node);
    auto e = ts_node_end_byte(m.captures[0].node);
    putfn("  pp %.*s", e - s, src + s);

    s = ts_node_start_byte(m.captures[1].node);
    e = ts_node_end_byte(m.captures[1].node);
    putfn("     %.*s", e - s, src + s);
  });
} catch (glen::query_error e) {
  errln("query error ", static_cast<unsigned>(e));
  return 1;
}

