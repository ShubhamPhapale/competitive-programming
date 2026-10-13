import sublime
import sublime_plugin

ITEMS = [
    ("vec", "std::vector<${1:int}> ${2:a}", sublime.KIND_TYPE, "std::", "vector<T>"),
    ("pair", "std::pair<${1:int}, ${2:int}> ${3:p}", sublime.KIND_TYPE, "std::", "pair<T1, T2>"),
    ("pii", "std::pair<int, int> ${1:p}", sublime.KIND_TYPE, "std::", "pair<int, int>"),
    ("string", "std::string ${1:s}", sublime.KIND_TYPE, "std::", "std::string"),
    ("set", "std::set<${1:int}> ${2:st}", sublime.KIND_TYPE, "std::", "set<T>"),
    ("multiset", "std::multiset<${1:int}> ${2:ms}", sublime.KIND_TYPE, "std::", "multiset<T>"),
    ("unordered_set", "std::unordered_set<${1:int}> ${2:st}", sublime.KIND_TYPE, "std::", "Hash set"),
    ("unordered_map", "std::unordered_map<${1:int}, ${2:int}> ${3:mp}", sublime.KIND_TYPE, "std::", "Hash map"),
    ("pq", "std::priority_queue<${1:int}> ${2:pq}", sublime.KIND_TYPE, "std::", "Max heap"),
    ("pqg", "std::priority_queue<${1:int}, std::vector<${1:int}>, std::greater<${1:int}>> ${2:pq}", sublime.KIND_TYPE, "std::", "Min heap (greater)"),
    ("queue", "std::queue<${1:int}> ${2:q}", sublime.KIND_TYPE, "std::", "queue<T>"),
    ("stack", "std::stack<${1:int}> ${2:st}", sublime.KIND_TYPE, "std::", "stack<T>"),
    ("deque", "std::deque<${1:int}> ${2:dq}", sublime.KIND_TYPE, "std::", "Double-ended queue"),
    ("array", "std::array<${1:int}, ${2:N}> ${3:a}", sublime.KIND_TYPE, "std::", "Fixed size array"),
    ("tuple", "std::tuple<${1:int}, ${2:int}> ${3:tp}", sublime.KIND_TYPE, "std::", "tuple<Args...>"),
    ("cin", "std::cin >> ${1:x};", sublime.KIND_VARIABLE, "std::", "Input stream"),
    ("cout", "std::cout << ${1:x} << '\\n';", sublime.KIND_VARIABLE, "std::", "Output stream"),
    ("cerr", "std::cerr << ${1:x} << '\\n';", sublime.KIND_VARIABLE, "std::", "Error stream"),
    ("all", "${1:a}.begin(), ${1:a}.end()", sublime.KIND_KEYWORD, "range", "begin() to end()"),
    ("rall", "${1:a}.rbegin(), ${1:a}.rend()", sublime.KIND_KEYWORD, "range", "rbegin() to rend()"),
    ("sz", "int(${1:a}.size())", sublime.KIND_FUNCTION, "int", "Cast size to int"),
    ("yes", "std::cout << \"YES\\n\";", sublime.KIND_SNIPPET, "io", "Print YES"),
    ("no", "std::cout << \"NO\\n\";", sublime.KIND_SNIPPET, "io", "Print NO"),
    ("pb", "push_back(${1:x})", sublime.KIND_FUNCTION, "vec", "push_back"),
]


class StdCompleterListener(sublime_plugin.EventListener):
    def on_query_completions(self, view, prefix, locations):
        if not locations:
            return []

        if not view.match_selector(locations[0], "source.c++"):
            return []

        pt = locations[0]
        prefix_len = len(prefix)
        start_pt = max(0, pt - prefix_len - 5)
        before_text = view.substr(sublime.Region(start_pt, pt - prefix_len))
        has_std_prefix = before_text.endswith("std::")

        prefix_lower = prefix.lower()
        completions = []

        for trig, snip, kind, ann, details in ITEMS:
            if has_std_prefix:
                if not (snip.startswith("std::") or ann == "std::"):
                    continue
                if prefix_lower and not trig.lower().startswith(prefix_lower):
                    continue
                clean_snip = snip[5:] if snip.startswith("std::") else snip
                completions.append(
                    sublime.CompletionItem(
                        trigger=trig,
                        annotation=ann,
                        details=details,
                        completion=clean_snip,
                        completion_format=sublime.COMPLETION_FORMAT_SNIPPET,
                        kind=kind
                    )
                )
            else:
                if not prefix_lower or trig.lower().startswith(prefix_lower):
                    completions.append(
                        sublime.CompletionItem(
                            trigger=trig,
                            annotation=ann,
                            details=details,
                            completion=snip,
                            completion_format=sublime.COMPLETION_FORMAT_SNIPPET,
                            kind=kind
                        )
                    )

        return completions
