out/%: %.sv
	mkdir -p out && iverilog -g2012  -o $@ $<

clean:
	rm -rf out

help:
	@echo use make out/some-binary

.PHONY: clean help
.DEFAULT_GOAL := help
