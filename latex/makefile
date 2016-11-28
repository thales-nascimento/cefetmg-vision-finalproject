texname = example

article:
	pdflatex $(texname)
	bibtex $(texname)
	pdflatex $(texname)
	pdflatex $(texname)

clean:
	rm $(texname).pdf $(texname).bbl $(texname).aux $(texname).blg $(texname).log $(texname).out
