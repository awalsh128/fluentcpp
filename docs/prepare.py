#!/usr/bin/env python3
"""Prepares inputs for the Doxygen website build.

  mainpage <README.md> <out.md>   Turns the GitHub README into a Doxygen main page.
  header   <header.html>          Adds the Doxygen Awesome theme scripts to a generated header.
"""
import re
import sys

REPO_BLOB = "https://github.com/awalsh128/fluentcpp/blob/master/"

SCRIPTS = """\
<script type="text/javascript" src="$relpath^doxygen-awesome-darkmode-toggle.js"></script>
<script type="text/javascript" src="$relpath^doxygen-awesome-fragment-copy-button.js"></script>
<script type="text/javascript" src="$relpath^doxygen-awesome-paragraph-link.js"></script>
<script type="text/javascript" src="$relpath^doxygen-awesome-interactive-toc.js"></script>
<script type="text/javascript">
  DoxygenAwesomeDarkModeToggle.init();
  DoxygenAwesomeFragmentCopyButton.init();
  DoxygenAwesomeParagraphLink.init();
  DoxygenAwesomeInteractiveToc.init();
</script>
"""


def mainpage(src, dst):
    text = open(src, encoding="utf-8").read()
    # GitHub-only HTML: centered logo, title, tagline and badge blocks.
    text = re.sub(r"<p align=\"center\">.*?</p>\s*", "", text, flags=re.S)
    text = re.sub(r"<h1 align=\"center\">.*?</h1>\s*", "", text, flags=re.S)
    # Emoji shortcodes such as ":link:" are not rendered by Doxygen.
    text = re.sub(r"^(\s*[-*] )?:[a-z_]+: ", r"\1", text, flags=re.M)
    # GitHub alert syntax becomes a plain bold-prefixed quote.
    text = re.sub(r"<!-- prettier-ignore -->\n", "", text)
    text = re.sub(r"> \[!(\w+)\]\n> ", lambda m: "> **%s:** " % m.group(1).title(), text)
    # Headings written as HTML stay out of Doxygen's sidebar tree, which would otherwise list
    # every README section above Classes and Files.
    lines, fenced = [], False
    for line in text.split("\n"):
        if line.startswith("```"):
            fenced = not fenced
        m = None if fenced else re.match(r"^(#{2,4}) (.+)$", line)
        lines.append("<h%d>%s</h%d>" % (len(m.group(1)), m.group(2), len(m.group(1))) if m else line)
    text = "\n".join(lines)
    # Repo-relative links only resolve on GitHub; point them at the repository.
    def fix(m):
        target = m.group(2)
        if re.match(r"^(https?:|mailto:|#)", target):
            return m.group(0)
        return "[%s](%s%s)" % (m.group(1), REPO_BLOB, target)
    text = re.sub(r"\[([^\]]+)\]\(([^)\s]+)\)", fix, text)
    tagline = ("Statically typed, fluent query functions for C++20 containers that are easy to "
               "discover through auto-complete.")
    badges = (
        "[![Build status](https://github.com/awalsh128/fluentcpp/actions/workflows/ci.yml/badge.svg)]"
        "(https://github.com/awalsh128/fluentcpp/actions/workflows/ci.yml) "
        "[![Apache 2.0 license](https://img.shields.io/badge/license-Apache%202.0-blue.svg)]"
        "(https://github.com/awalsh128/fluentcpp/blob/master/LICENSE) "
        "[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)]"
        "(https://en.cppreference.com/w/cpp/20)"
    )
    btn = "display:inline-block;padding:.5em 1.1em;margin:0 .5em .5em 0;border-radius:6px;font-weight:600;text-decoration:none;"
    links = (
        '<p><a href="annotated.html" style="%sbackground:var(--primary-color-dark);color:#fff;">API reference</a>'
        '<a href="functions.html" style="%sborder:1px solid var(--primary-color);">Class members</a>'
        '<a href="files.html" style="%sborder:1px solid var(--primary-color);">Files</a></p>'
    ) % (btn, btn, btn)
    out = "# Fluent C++ {#mainpage}\n\n%s\n\n%s\n\n%s\n\n%s" % (tagline, badges, links, text.lstrip())
    open(dst, "w", encoding="utf-8").write(out)


def header(path):
    text = open(path, encoding="utf-8").read()
    assert "</head>" in text, "generated header has no </head>"
    open(path, "w", encoding="utf-8").write(text.replace("</head>", SCRIPTS + "</head>", 1))


if __name__ == "__main__":
    if sys.argv[1:2] == ["mainpage"] and len(sys.argv) == 4:
        mainpage(sys.argv[2], sys.argv[3])
    elif sys.argv[1:2] == ["header"] and len(sys.argv) == 3:
        header(sys.argv[2])
    else:
        sys.exit(__doc__)
