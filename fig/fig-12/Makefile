all:
	#!/bin/bash; \
	for file in $$(ls *.py | grep -v "zplot.py"); do \
		python "$$file" eps; \
	done; \
	for file in *.eps; do \
		ps2pdf "$$file"; \
	done; \
	for file in *.pdf; do \
		pdfcrop "$$file" "$$file"; \
	done
	rm -rf *.eps
fig:
	mv -f *.pdf ../../../
clean: 
	rm -rf *.eps *.pdf


# fig:
# # mv -f *.pdf ../../../fig/
# 	mv -f *.pdf ../../../
# clean: 
# 	rm -rf *.eps *.pdf

# PY=python
# EPS2PDF=ps2pdf
# CROP=pdfcrop

# SRC := $(wildcard *.py)
# SRC := $(filter-out zplot.py, $(SRC))
# EPS = $(patsubst %.py,%.eps,$(SRC))m
# PDF = $(patsubst %.py,%.pdf,$(SRC))

# all: $(EPS) $(PDF)

# %.eps: %.py 
# 	$(PY) $< eps

# %.pdf: %.eps
# 	$(EPS2PDF) $<
# 	$(CROP) $@ $@
# 	rm -rf $<

# fig:
# # mv -f *.pdf ../../../fig/
# 	mv -f *.pdf ../../../
# clean: 
# 	rm -rf *.eps *.pdf

