" detect *.cos files

augroup filetype
	au! BufRead,BufNewFile *.cos	set filetype=caos
augroup END

