FILES = $(wildcard 0*.md)

um.epub: title.md $(FILES)
	pandoc --table-of-contents --katex --css epub.css -o $@ title.md $(FILES)
