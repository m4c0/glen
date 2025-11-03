#pragma leco tool
// TODO: remove this after wrapping enough utilities
extern "C" {
#include "tree-sitter/lib/include/tree_sitter/api.h"
}
import glen;
import jute;
import print;

static void nav(TSTreeCursor & c) {
  auto node = ts_tree_cursor_current_node(&c);
  auto type = jute::view::unsafe(ts_node_type(node));
  if (type == "preproc_call") {
    auto dir = ts_node_child_by_field_name(node, "directive", 9);
    putln("pp call ", ts_node_child_count(dir));
    return;
  }

  putln("enter ", ts_node_string(node));
  if (ts_tree_cursor_goto_first_child(&c)) {
    nav(c);
    while (ts_tree_cursor_goto_next_sibling(&c)) {
      nav(c);
    }
    ts_tree_cursor_goto_parent(&c);
  }
  putln("leave");
}

int main() {
  glen::parser p { tree_sitter_cpp };
  auto t = p.parse(jute::view {
    // Note: tree-sitter does not distinguish between #ifdef and #ifndef
    "#ifdef DEBUG\n"
    // Note: we can, on a best case, get "#  pragma" as the "call type"
    "#  pragma leco tool\n"
    "#else\n"
    "#  error\n"
    "#endif\n"
  });
  TSTreeCursor c = ts_tree_cursor_new(t.root_node());
  nav(c);
  ts_tree_cursor_delete(&c);
}
