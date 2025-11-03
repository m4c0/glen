#pragma leco tool
// TODO: remove this after wrapping enough utilities
extern "C" {
#include "tree-sitter/lib/include/tree_sitter/api.h"
}

import glen;
import jojo;
import jute;
import print;

using namespace jute::literals;

int main() try {
  auto src = jojo::read_cstr("glen.cppm");

  glen::parser p { tree_sitter_cpp };
  auto t = p.parse(src);

  //putln(ts_node_string(t.root_node()));

  t.for_each_match(R"(
    (module_declaration name: (module_name (identifier) @mod) partition: (module_partition (module_name (identifier) @part)) )
  )"_s, [src=src.begin()](auto & m) {
    auto s = ts_node_start_byte(m.captures[0].node);
    auto e = ts_node_end_byte(m.captures[0].node);
    putfn("  module %.*s", e - s, src + s);

    s = ts_node_start_byte(m.captures[1].node);
    e = ts_node_end_byte(m.captures[1].node);
    putfn("         %.*s", e - s, src + s);
  });
  t.for_each_capture(R"(
    (import_declaration (module_name (identifier) @imp))
  )"_s, [src=src.begin()](auto & n) {
    auto s = ts_node_start_byte(n);
    auto e = ts_node_end_byte(n);
    putfn("  imported %.*s", e - s, src + s);
  });

  t.for_each_capture(R"(
    (preproc_include (string_literal (string_content)) @inc)
  )"_s, [src=src.begin()](auto & n) {
    auto s = ts_node_start_byte(n);
    auto e = ts_node_end_byte(n);
    putfn("  included %.*s", e - s, src + s);
  });

  t.for_each_match(R"(
    (preproc_call directive: (_) @d argument: (_) @a)
  )"_s, [src=src.begin()](auto & m) {
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
