# fizanc
A really simple (literally just rewritten from human langugage to c) implementation of evacuation simulation models described in [this article.](https://github.com/remilia-smierdzi/fizanc/blob/master/mrowinski-fens2010.pdf)

### Building
This program depends on ncurses, so install that beforehand if compiling from source

Arch based: `pacman -S ncurses`  
Debian based: `apt install libncurses5-dev`  
Mac: install [homebrew](brew.sh) and then `brew install ncurses`

I couldn't understand how to compile c programs on windows, use wsl maybe if you need to run locally.
Then run `make`.

If you don't care about all that and just want to quickly run it, here: https://replit.com/@frytkisasmaczne/fizanc-1#README.md

## Usage
run `./ffp` or `./ffrm` to get respective help messages.

`./ffp`:
```
Floor field with pressure model  
Usage
  %s [options] <file>
Options:
  <file>        file containing the simulation template
  -s            enable skip mode - don't draw
  -b <0.0-1.0>  beta factor - antisociality, floating point number from 0 to 1 (default .5)
  
Template file format
can only contain characters:
  #       for wall
  p       for person
  e       for exit
  space   for air
The room containing the exits has to be completely closed, with sharp corners, example:
  ####
  #p ##
  #p e#
  #####```
