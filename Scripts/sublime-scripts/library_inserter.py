import os
import sublime
import sublime_plugin

LIBRARY_PATH = os.path.expanduser("~/Competitive Programming/Library")

# Common competitive programming aliases for library files
ALIASES = {
    "segment": ["segtree"],
    "fenwick": ["bit"],
    "sparsetable": ["sparse"],
    "modular": ["modint", "mint"],
    "sieve": ["spf"],
    "maxflow": ["dinic"],
    "convexhulltrick": ["cht"],
    "dsu": ["dsurollback", "rollback"],
    "apsp": ["floyd", "floydwarshall"],
    "z": ["zalgorithm"],
    "suffixautomaton": ["sam"],
    "bitmask": ["tsp"],
    "fft": ["poly", "polynomial"],
}

def clean_code(raw_code, is_template=False):
    if is_template:
        return raw_code.strip()
    lines = raw_code.splitlines()
    clean_lines = []
    for line in lines:
        stripped = line.strip()
        if stripped in ("#include <bits/stdc++.h>", "using namespace std;"):
            continue
        clean_lines.append(line)
    while clean_lines and not clean_lines[0].strip():
        clean_lines.pop(0)
    return "\n".join(clean_lines)

def get_library_items():
    items = []
    if not os.path.isdir(LIBRARY_PATH):
        return items

    for root, dirs, files in os.walk(LIBRARY_PATH):
        for f in sorted(files):
            if f.endswith(".cpp"):
                full_path = os.path.join(root, f)
                rel_path = os.path.relpath(full_path, LIBRARY_PATH)
                category = os.path.dirname(rel_path) or "General"
                name = os.path.splitext(f)[0]
                items.append({
                    "name": name,
                    "category": category,
                    "path": full_path,
                    "rel_path": rel_path
                })
    return items

class InsertLibraryAlgorithmCommand(sublime_plugin.TextCommand):
    """
    Opens a fuzzy quick panel showing all library algorithms.
    Selecting an item inserts its code at the cursor.
    """
    def run(self, edit):
        items = get_library_items()
        if not items:
            sublime.status_message("Library is empty or not found")
            return

        display_list = []
        for it in items:
            display_list.append([
                it["name"],
                f"[{it['category']}] {it['rel_path']}"
            ])

        def on_done(index):
            if index == -1:
                return
            chosen = items[index]
            try:
                with open(chosen["path"], "r", encoding="utf-8") as f:
                    code = clean_code(f.read(), is_template=(chosen["category"] == "Templates"))
                self.view.run_command("library_paste_code", {"code": code})
                sublime.status_message(f"Inserted: {chosen['name']}")
            except Exception as e:
                sublime.error_message(f"Failed to read file: {e}")

        self.view.window().show_quick_panel(display_list, on_done)

class LibraryPasteCodeCommand(sublime_plugin.TextCommand):
    def run(self, edit, code=""):
        for sel in self.view.sel():
            if sel.empty():
                self.view.insert(edit, sel.begin(), code)
            else:
                self.view.replace(edit, sel, code)

class LibraryCompletionListener(sublime_plugin.EventListener):
    """
    Dynamically suggests library algorithms as auto-complete snippets.
    Typing 'dsu', 'segtree', 'dijkstra', etc. + Tab inserts the code.
    """
    def on_query_completions(self, view, prefix, locations):
        if not locations:
            return []
        
        # Only trigger in C++ files
        if not view.match_selector(locations[0], "source.c++"):
            return []

        items = get_library_items()
        completions = []
        prefix_lower = prefix.lower()

        for it in items:
            triggers = [it["name"]]
            if it["name"] in ALIASES:
                triggers.extend(ALIASES[it["name"]])

            for trig in triggers:
                if not prefix_lower or trig.lower().startswith(prefix_lower):
                    try:
                        with open(it["path"], "r", encoding="utf-8") as f:
                            code = clean_code(f.read(), is_template=(it["category"] == "Templates"))
                    except Exception:
                        continue

                    # Sublime Text 4 CompletionItem
                    if hasattr(sublime, "CompletionItem"):
                        completions.append(
                            sublime.CompletionItem(
                                trigger=trig,
                                annotation=f"Library/{it['category']}",
                                details=f"Insert {it['name']} from Library",
                                completion=code,
                                completion_format=sublime.COMPLETION_FORMAT_TEXT,
                                kind=sublime.KIND_SNIPPET
                            )
                        )
                    else:
                        completions.append((f"{trig}\tLibrary/{it['category']}", code))

        return completions
