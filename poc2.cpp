#pragma leco tool
// TODO: remove this after wrapping enough utilities
extern "C" {
#include "tree-sitter/lib/include/tree_sitter/api.h"
}
import glen;
import jute;
import print;

static void nav(TSTreeCursor & c) {
  putln("enter ", ts_node_string(ts_tree_cursor_current_node(&c)));
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
    "#ifdef DEBUG\n"
    "#  pragma leco tool\n"
    "#else\n"
    "#  error\n"
    "#endif\n"
  });
  TSTreeCursor c = ts_tree_cursor_new(t.root_node());
  nav(c);
  ts_tree_cursor_delete(&c);
}
