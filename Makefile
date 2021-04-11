CPPFLAGS = -D_DEBUG -g -Wall -std=c++20
KATEX = https://cdn.jsdelivr.net/npm/katex@0.12.0/dist/
FILES = $(wildcard 0*.md) framework.md
HTML = $(FILES:.md=.html)
PDF = $(FILES:.md=.pdf)
CSS = epub.css

FLAGS += -f markdown+tex_math_single_backslash+auto_identifiers+definition_lists
HTML_FLAGS = $(FLAGS) -t html5 -s --katex=$(KATEX) --css=$(CSS)
TEX_FLAGS = $(FLAGS) --pdf-engine=xelatex

%.html: %.md $(CSS)
	pandoc $(HTML_FLAGS) $< -o $@

$(PDF): %.pdf: %.md
	pandoc $(TEX_FLAGS) $< -o $@

um.epub: title.md $(FILES)
	pandoc --table-of-contents --katex --css epub.css -o $@ title.md $(FILES)

docs: $(HTML)
	cp $(HTML) docs
