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
  class cursor {
    hay<TSQueryCursor *, ts_query_cursor_new, ts_query_cursor_delete> m_c {};

  public:
    cursor(TSQuery * q, TSNode n) {
      ts_query_cursor_exec(m_c, q, n);
    }

    constexpr operator TSQueryCursor *() const { return m_c; }
  };

  class query {
    hay<TSQuery *, nullptr, ts_query_delete> m_q;

    static auto create(const TSLanguage * lang, jute::view src) {
      unsigned err_ofs {};
      TSQueryError err {};
      auto q = ts_query_new(lang, src.begin(), src.size(), &err_ofs, &err);
      if (err) throw err;
      return q;
    }
  public:
    query(const TSLanguage * lang, jute::view src) : m_q { create(lang, src) } {}

    constexpr operator TSQuery *() const { return m_q; }

    auto exec(TSNode n) const { return cursor { m_q, n }; }

    void for_each_capture(TSNode n, auto && fn) const {
      auto c = exec(n);
      
      TSQueryMatch match {};
      unsigned idx {};
      while (ts_query_cursor_next_capture(c, &match, &idx)) {
        fn(match.captures[idx].node);
      }
    }

    void for_each_match(TSNode n, auto && fn) const {
      auto c = exec(n);
      
      TSQueryMatch match {};
      while (ts_query_cursor_next_match(c, &match)) {
        fn(static_cast<const TSQueryMatch &>(match));
      }
    }
  };

  class tree {
    hay<TSTree *, nullptr, ts_tree_delete> m_t;
  public:
    tree(TSTree * t) : m_t { t } {}

    constexpr operator TSTree *() const { return m_t; }

    auto root_node() const { return ts_tree_root_node(m_t); }
    auto language() const { return ts_tree_language(m_t); }

    auto query(jute::view src) const {
      return glen::query { language(), src };
    }
    void for_each_capture(jute::view src, auto && fn) const {
      query(src).for_each_capture(root_node(), fn);
    }
    void for_each_match(jute::view src, auto && fn) const {
      query(src).for_each_match(root_node(), fn);
    }
  };

  class parser {
    hay<TSParser *, ts_parser_new, ts_parser_delete> m_p {};

  public:
    parser(lang::t lang) {
      ts_parser_set_language(m_p, lang());
    }

    constexpr operator TSParser *() const { return m_p; }

    auto parse(jute::view src) {
      return tree { ts_parser_parse_string(m_p, nullptr, src.begin(), src.size()) };
    }
  };
}

int main() try {
  auto src = jojo::read_cstr("poc.cpp");

  glen::parser p { glen::lang::cpp };
  auto t = p.parse(src);

  //putln(ts_node_string(t.root_node()));

  t.for_each_capture(R"(
    (import_declaration (module_name (identifier) @imp))
  )", [src=src.begin()](auto & n) {
    auto s = ts_node_start_byte(n);
    auto e = ts_node_end_byte(n);
    putfn("  imported %.*s", e - s, src + s);
  });

  t.for_each_capture(R"(
    (preproc_include (string_literal (string_content)) @inc)
  )", [src=src.begin()](auto & n) {
    auto s = ts_node_start_byte(n);
    auto e = ts_node_end_byte(n);
    putfn("  included %.*s", e - s, src + s);
  });

  t.for_each_capture(R"(
    (preproc_call) @pp
  )", [&t,src=src.begin()](auto & n) {
    t.query(R"(
      (_ directive: (_) @d
         argument: (_) @a)
    )").for_each_match(n, [src](auto & m) {
      auto s = ts_node_start_byte(m.captures[0].node);
      auto e = ts_node_end_byte(m.captures[0].node);
      putfn("  pp %.*s", e - s, src + s);

      s = ts_node_start_byte(m.captures[1].node);
      e = ts_node_end_byte(m.captures[1].node);
      putfn("     %.*s", e - s, src + s);

    });
  });
} catch (TSQueryError e) {
  errln("query error ", static_cast<unsigned>(e));
  return 1;
}
