KICAD_CLI ?= /Applications/KiCad/KiCad.app/Contents/MacOS/kicad-cli
SCHEMA_DIR := docs/schema
SCHEMA_SVG := $(SCHEMA_DIR)/schema.svg

all: schema_svg

schema_svg: $(SCHEMA_SVG)

$(SCHEMA_SVG): $(SCHEMA_DIR)/schema.kicad_sch
	$(KICAD_CLI) sch export svg \
		--black-and-white --exclude-drawing-sheet --no-background-color \
		--output $(SCHEMA_DIR) $<

clean:
	rm -f $(SCHEMA_SVG)

.PHONY: all schema_svg clean
