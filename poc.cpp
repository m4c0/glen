#pragma leco test

#pragma leco add_impl ts

extern "C" {
#include "tree-sitter/lib/include/tree_sitter/api.h"
const TSLanguage * tree_sitter_cpp();
const TSLanguage * tree_sitter_java();
}

import hay;
import jojo;
import jute;
import print;

namespace glen::lang {
  using t = const TSLanguage * (*)();
  constexpr const auto cpp  = tree_sitter_cpp;
  constexpr const auto java = tree_sitter_java;
};
namespace glen {
  class tree {
    hay<TSTree *, nullptr, ts_tree_delete> m_t;
  public:
    tree(TSTree * t) : m_t { t } {}

    auto root_node() { return ts_tree_root_node(m_t); }
  };

  class parser {
    hay<TSParser *, ts_parser_new, ts_parser_delete> m_p {};

  public:
    parser(lang::t lang) {
      ts_parser_set_language(m_p, lang());
    }

    auto parse(jute::view src) {
      return tree { ts_parser_parse_string(m_p, nullptr, src.begin(), src.size()) };
    }
  };
}

int main() {
  glen::parser p { glen::lang::cpp };
  auto t = p.parse(jojo::read_cstr("poc.cpp"));
  putln(ts_node_string(t.root_node()));
}
