# vim: set ft=sh:

if [[ $BASH_VERSION ]]; then
  # Aliases should act like simple text-replacement macros;
  # similar to how they behave (by default) in Zsh
  shopt -s expand_aliases
fi

alias al-mkme='if [[ -n ${FUNCNAME[0]} ]]; then local __me__="${FUNCNAME[0]}"; elif [[ -n ${funcstack[0]} ]]; then local __me__="${funcstack[0]}"; elif [[ -z ${funcstack[0]} ]]; then local __me__="${funcstack[1]}"; else local __me__=""; fi'
