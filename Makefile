FILES = $(shell ls 0*.md)

um.epub: title.md $(FILES)
	pandoc --katex -o $@ title.md $(FILES)
