" Vim syntax file
" Language: Neon

if exists("b:current_syntax")
    finish
endif

" It seems that later entries in this file are matched first,
" which is why neonKeyword comes after neonIdentifier.
syntax match neonIdentifier '\<[A-Za-z][A-Za-z0-9_]*\>'
syntax match neonKeyword '\<[A-Z]\+\>'
syntax keyword neonKeyword _
syntax match neonOperator '[()\[\]{}+*/^&=,.<>:-]'
syntax match neonError '=='
syntax keyword neonBoolean FALSE TRUE
syntax match neonNumber '\<\d[0-9_]*\(\.[0-9_]\+\)\?\([Ee][+-]\?[0-9_]\+\)\?\>'
syntax match neonNumber '\<0b[01_]\+\>'
syntax match neonNumber '\<0o[0-7_]\+\>'
syntax match neonNumber '\<0x[0-9A-Fa-f_]\+\>'
syntax match neonString '"\(\\.\|[^"]\)*"'
syntax match neonString '@"\(\\.\|[^"]\)*"'
syntax match neonString '@\(\w*\)@".\{-}"@\1@'
syntax region neonComment oneline start="--" end="$" contains=neonTodo
syntax region neonComment start="/\*" end="\*/" contains=neonTodo
syntax keyword neonTodo contained TODO FIXME XXX

highlight def link neonKeyword      Keyword
highlight def link neonIdentifier   Identifier
highlight def link neonOperator     Operator
highlight def link neonError        Error
highlight def link neonBoolean      Boolean
highlight def link neonNumber       Number
highlight def link neonString       String
highlight def link neonComment      Comment
highlight def link neonTodo         Todo

let b:current_syntax = "neon"
