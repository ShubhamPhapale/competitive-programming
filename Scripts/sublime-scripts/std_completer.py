import sublime
import sublime_plugin

# Standard library definitions for CP
# (trigger, completion_snippet, kind, annotation, details)
ITEMS = [
    # --- Containers & Types ---
    ("vector", "std::vector<${1:int}> ${2:a}", sublime.KIND_TYPE, "std::", "Dynamic array"),
    ("vec", "std::vector<${1:int}> ${2:a}", sublime.KIND_TYPE, "std::", "vector<T>"),
    ("vvi", "std::vector<std::vector<int>> ${1:a}", sublime.KIND_TYPE, "std::", "2D vector<int>"),
    ("vvl", "std::vector<std::vector<long long>> ${1:a}", sublime.KIND_TYPE, "std::", "2D vector<long long>"),
    ("pair", "std::pair<${1:int}, ${2:int}> ${3:p}", sublime.KIND_TYPE, "std::", "std::pair<T1, T2>"),
    ("pii", "std::pair<int, int> ${1:p}", sublime.KIND_TYPE, "std::", "std::pair<int, int>"),
    ("pll", "std::pair<long long, long long> ${1:p}", sublime.KIND_TYPE, "std::", "std::pair<ll, ll>"),
    ("string", "std::string ${1:s}", sublime.KIND_TYPE, "std::", "std::string"),
    ("str", "std::string ${1:s}", sublime.KIND_TYPE, "std::", "std::string"),
    ("set", "std::set<${1:int}> ${2:st}", sublime.KIND_TYPE, "std::", "std::set<T>"),
    ("multiset", "std::multiset<${1:int}> ${2:ms}", sublime.KIND_TYPE, "std::", "std::multiset<T>"),
    ("map", "std::map<${1:int}, ${2:int}> ${3:mp}", sublime.KIND_TYPE, "std::", "std::map<K, V>"),
    ("unordered_set", "std::unordered_set<${1:int}> ${2:st}", sublime.KIND_TYPE, "std::", "Hash set"),
    ("unordered_map", "std::unordered_map<${1:int}, ${2:int}> ${3:mp}", sublime.KIND_TYPE, "std::", "Hash map"),
    ("priority_queue", "std::priority_queue<${1:int}> ${2:pq}", sublime.KIND_TYPE, "std::", "Max heap"),
    ("pq", "std::priority_queue<${1:int}> ${2:pq}", sublime.KIND_TYPE, "std::", "Max heap"),
    ("pqg", "std::priority_queue<${1:int}, std::vector<${1:int}>, std::greater<${1:int}>> ${2:pq}", sublime.KIND_TYPE, "std::", "Min heap (greater)"),
    ("queue", "std::queue<${1:int}> ${2:q}", sublime.KIND_TYPE, "std::", "std::queue<T>"),
    ("stack", "std::stack<${1:int}> ${2:st}", sublime.KIND_TYPE, "std::", "std::stack<T>"),
    ("deque", "std::deque<${1:int}> ${2:dq}", sublime.KIND_TYPE, "std::", "Double-ended queue"),
    ("array", "std::array<${1:int}, ${2:N}> ${3:a}", sublime.KIND_TYPE, "std::", "Fixed size array"),
    ("tuple", "std::tuple<${1:int}, ${2:int}> ${3:tp}", sublime.KIND_TYPE, "std::", "std::tuple<Args...>"),

    # --- I/O Streams ---
    ("cin", "std::cin >> ${1:x};", sublime.KIND_VARIABLE, "std::", "Input stream"),
    ("cout", "std::cout << ${1:x} << '\\n';", sublime.KIND_VARIABLE, "std::", "Output stream"),
    ("cerr", "std::cerr << ${1:x} << '\\n';", sublime.KIND_VARIABLE, "std::", "Error stream"),
    ("endl", "std::endl", sublime.KIND_VARIABLE, "std::", "Flush newline"),

    # --- Algorithms ---
    ("sort", "std::sort(${1:a}.begin(), ${1:a}.end());", sublime.KIND_FUNCTION, "std::", "Ascending sort"),
    ("rsort", "std::sort(${1:a}.rbegin(), ${1:a}.rend());", sublime.KIND_FUNCTION, "std::", "Descending sort"),
    ("reverse", "std::reverse(${1:a}.begin(), ${1:a}.end());", sublime.KIND_FUNCTION, "std::", "std::reverse"),
    ("min", "std::min(${1:a}, ${2:b})", sublime.KIND_FUNCTION, "std::", "std::min"),
    ("max", "std::max(${1:a}, ${2:b})", sublime.KIND_FUNCTION, "std::", "std::max"),
    ("min_element", "std::min_element(${1:a}.begin(), ${1:a}.end())", sublime.KIND_FUNCTION, "std::", "Find min element iterator"),
    ("max_element", "std::max_element(${1:a}.begin(), ${1:a}.end())", sublime.KIND_FUNCTION, "std::", "Find max element iterator"),
    ("swap", "std::swap(${1:a}, ${2:b});", sublime.KIND_FUNCTION, "std::", "std::swap"),
    ("clamp", "std::clamp(${1:x}, ${2:low}, ${3:high})", sublime.KIND_FUNCTION, "std::", "Clamp value between low and high"),
    ("gcd", "std::gcd(${1:a}, ${2:b})", sublime.KIND_FUNCTION, "std::", "Greatest common divisor"),
    ("lcm", "std::lcm(${1:a}, ${2:b})", sublime.KIND_FUNCTION, "std::", "Least common multiple"),
    ("iota", "std::iota(${1:a}.begin(), ${1:a}.end(), ${2:0});", sublime.KIND_FUNCTION, "std::", "Fill with sequential values"),
    ("fill", "std::fill(${1:a}.begin(), ${1:a}.end(), ${2:0});", sublime.KIND_FUNCTION, "std::", "Fill range with value"),
    ("accumulate", "std::accumulate(${1:a}.begin(), ${1:a}.end(), ${2:0LL})", sublime.KIND_FUNCTION, "std::", "Range sum"),
    ("count", "std::count(${1:a}.begin(), ${1:a}.end(), ${2:val})", sublime.KIND_FUNCTION, "std::", "Count occurrences"),
    ("find", "std::find(${1:a}.begin(), ${1:a}.end(), ${2:val})", sublime.KIND_FUNCTION, "std::", "Find element iterator"),
    ("unique", "${1:a}.erase(std::unique(${1:a}.begin(), ${1:a}.end()), ${1:a}.end());", sublime.KIND_FUNCTION, "std::", "Remove consecutive duplicates"),
    ("lower_bound", "std::lower_bound(${1:a}.begin(), ${1:a}.end(), ${2:val})", sublime.KIND_FUNCTION, "std::", "Binary search lower bound"),
    ("upper_bound", "std::upper_bound(${1:a}.begin(), ${1:a}.end(), ${2:val})", sublime.KIND_FUNCTION, "std::", "Binary search upper bound"),
    ("binary_search", "std::binary_search(${1:a}.begin(), ${1:a}.end(), ${2:val})", sublime.KIND_FUNCTION, "std::", "Binary search boolean test"),
    ("next_permutation", "std::next_permutation(${1:a}.begin(), ${1:a}.end())", sublime.KIND_FUNCTION, "std::", "Next lexicographical permutation"),
    ("prev_permutation", "std::prev_permutation(${1:a}.begin(), ${1:a}.end())", sublime.KIND_FUNCTION, "std::", "Prev lexicographical permutation"),

    # --- Numeric & Math ---
    ("abs", "std::abs(${1:x})", sublime.KIND_FUNCTION, "std::", "std::abs"),
    ("sqrt", "std::sqrt(${1:x})", sublime.KIND_FUNCTION, "std::", "std::sqrt"),
    ("log2", "std::log2(${1:x})", sublime.KIND_FUNCTION, "std::", "Base-2 logarithm"),
    ("ceil", "std::ceil(${1:x})", sublime.KIND_FUNCTION, "std::", "Ceiling"),
    ("floor", "std::floor(${1:x})", sublime.KIND_FUNCTION, "std::", "Floor"),
    ("pow", "std::pow(${1:base}, ${2:exp})", sublime.KIND_FUNCTION, "std::", "Power"),
    ("fixed", "std::fixed", sublime.KIND_VARIABLE, "std::", "Fixed floating-point formatting"),
    ("setprecision", "std::setprecision(${1:12})", sublime.KIND_FUNCTION, "std::", "Decimal precision"),
]


class StdCompleterListener(sublime_plugin.EventListener):
    """
    Provides instant, zero-latency auto-completion for standard library items
    prefixed with std:: in C++ source files.
    """
    def on_query_completions(self, view, prefix, locations):
        if not locations:
            return []

        # Only trigger in C++ source
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
                if prefix_lower and not trig.lower().startswith(prefix_lower):
                    continue
                # Strip leading std:: from the completion snippet because std:: is already on the line
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
