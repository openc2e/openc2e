CAOS syntax highlighting and injection for Vim
----------------------------------------------

First of all make sure you have the latest version of the
text editor Vim installed.  Check this by typing vim --version.
I get the following:

VIM - Vi IMproved 5.6 (2000 Jan 16, compiled Mar  7 2000 12:17:13)

Now add lines like these to the start of your ~/.vimrc:

let myfiletypefile = "~/c2e/tools/vimcaos/caosfiletypes.vim"
let mysyntaxfile = "~/c2e/tools/vimcaos/caossyntaxfile.vim"

If you don't already have a ~/.vimrc, then copy the one that comes with vim.
On my system it is at /usr/share/vim/vim56/vimrc_example.vim 
(type "locate vimrc" to find it on yours).  This is recommended, as 
it turns on lots of cool vim features.

According to the documentation, if you now edit a file with a .cos
extension it will be syntax highlighted as a caos file.  However,
this didn't work for me with Mandrake 7.

If it doesn't work
------------------

The problem seemed to be that the .vimrc file is read _after_
the common scripts which search for myfiletypefile and mysyntaxfile.
To force them to look again, add the following lines to your .vimrc
file, just after the let statements above:

filetype off
filetype on
syntax off
syntax on

This is obviously a hack, so anyone who can find out the real cause,
please let me know!

Injection
---------

The CAOS syntax file also maps function keys to let you inject and 
remove CAOS files into and from a running game. To do this, first
make sure the game says "Bound to port 20001" on startup.  This only
works under the Linux version of the game, under Windows you probably
want the CAOS tool (see http://cdn.creaturelabs.com/cdn/).

Next, you need to have the program "netcat" installed.  Try typing
"nc" at the command line to see if you already have it.  If not
you can get it from your OS distribution vendor or from
http://www.l0pht.com/~weld/netcat/.  Netcat echos file to internet
ports - in this case CAOS files to the game's external interface.

The keys this activates are:

<F11> - Inject current .cos file, including scripts
<F12> - Uninject current .cos file, running everything after rscr
<F10> - One off COAS command.  You can use the cursor keys to
retrieve earlier commands.  For example, type "outv totl 0 0 0" to
find out how many agents are in the world.

Rebuilding with new syntax
--------------------------

With new versions of the engine new commands are added.  You can
update the syntax highlighting to support them by doing the 
following:

Using <F10> run the command:
	file oope 1 "caos_help.html" 0 dbg: html 1
This will make a file caos_help.html in the Journal directory.

In the vimcaos directory, type the following:
	./convert.pl </wherever/Journal/caos_help.html >caos.auto.vim

Then restart vim.

Patches
-------

Send any additions or bug fixes to francis.irving@creaturelabs.com.

