CPPFLAGS = -g -Wall -std=c++17
FILES = $(wildcard 0*.md)
HTML = $(FILES:.md=.html)

FLAGS += -f markdown+tex_math_single_backslash+auto_identifiers+definition_lists

%.html: %.md $(CSS)
	pandoc $(FLAGS) $< -o $@

um.epub: title.md $(FILES)
	pandoc --table-of-contents --katex --css epub.css -o $@ title.md $(FILES)
