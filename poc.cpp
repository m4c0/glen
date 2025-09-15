#pragma leco test

import glen;
import jojo;
import jute;
import print;

using namespace jute::literals;

int main() try {
  auto src = jojo::read_cstr("glen.cppm");

  glen::parser p { glen::lang::cpp };
  auto t = p.parse(src);

  //putln(ts_node_string(t.root_node()));

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

  t.for_each_capture(R"(
    (preproc_call) @pp
  )"_s, [&t,src=src.begin()](auto & n) {
    t.query(R"(
      (_ directive: (_) @d
         argument: (_) @a)
    )"_s).for_each_match(n, [src](auto & m) {
      auto s = ts_node_start_byte(m.captures[0].node);
      auto e = ts_node_end_byte(m.captures[0].node);
      putfn("  pp %.*s", e - s, src + s);

      s = ts_node_start_byte(m.captures[1].node);
      e = ts_node_end_byte(m.captures[1].node);
      putfn("     %.*s", e - s, src + s);
    });
  });
} catch (glen::query_error e) {
  errln("query error ", static_cast<unsigned>(e));
  return 1;
}
