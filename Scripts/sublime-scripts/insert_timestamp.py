import sublime
import sublime_plugin
from datetime import datetime

class InsertTimestampCommand(sublime_plugin.TextCommand):
    def run(self, edit):
        timestamp = datetime.now().strftime("%d.%m.%Y %H:%M:%S")
        for region in self.view.sel():
            if region.empty():
                self.view.insert(edit, region.begin(), timestamp)
            else:
                self.view.replace(edit, region, timestamp)
