" Vim syntax file
" Language: Custom financial ledger
" Maintainer: Omar Padron
" Latest Revision: 13 October 2014

if exists("b:current_syntax")
    finish
endif

let b:current_syntax = "fin"

syn match finComment          '^#.*'

" syn match finAccountEntry     '^[a-zA-Z][a-zA-Z0-9 ]\{,6\}-\s*\(/[a-zA-Z0-9]\+\)*' contains=finAccountEntryRhs
" syn match finAccountEntryRhs  '-\(/[a-zA-Z0-9]\+\)*'                         contains=finAccount
" syn match finAccount          '\(/[a-zA-Z0-9]\+\)*'

hi def link finComment                       Comment

" hi def link finAccountEntry                  Type
" hi def link finAccountEntryRhs               Constant
" hi def link finAccount                       Constant

syn match finTest             '^[a-zA-Z][a-zA-Z0-9 ]\{,6\}-\s*\(/[a-zA-Z0-9]\+\)*'

hi def link finTest                          Statement

