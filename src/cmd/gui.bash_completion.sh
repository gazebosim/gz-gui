#!/usr/bin/env bash

# bash tab-completion

# This is a per-library function definition, used in conjunction with the
# top-level entry point in ign-tools.

function _gz_gui
{
  if [[ ${COMP_WORDS[COMP_CWORD]} == -* ]]; then
    # Specify options (-*) word list for this subcommand
    COMPREPLY=($(compgen -W "
      -l --list
      -s --standalone
      -c --config
      -v --verbose
      -h --help
      --force-version
      --versions
      " -- "${COMP_WORDS[COMP_CWORD]}" ))
    return
  else
    # Just use bash default auto-complete, because we never have two
    # subcommands in the same line. If that is ever needed, change here to
    # detect subsequent subcommands
    COMPREPLY=($(compgen -o default -- "${COMP_WORDS[COMP_CWORD]}"))
    return
  fi
}
