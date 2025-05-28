set autowrite
set backspace=
set clipboard=unnamedplus
set conceallevel=3
set cursorcolumn
set cursorline
set expandtab
set ignorecase
set list
set listchars+=precedes:«,extends:»,trail:❖
set mouse=
set number
set pumblend=10
set pumheight=10
set scrolloff=10
set shiftround
set shiftwidth=4
" set noshowmode
set sidescrolloff=8
set signcolumn=number
set smartcase
set smartindent
set spelllang=en
set splitbelow
set splitkeep=screen
set splitright
set tabstop=4
set termguicolors
set textwidth=90
set timeoutlen=300
set undofile
set undolevels=10000
set updatetime=200
set nowrap


tnoremap <Esc><Esc> <C-\><C-n>
tnoremap <C-a><Esc> <C-\><C-n>
tnoremap <Esc><cmd> <C-\><C-o>
tnoremap <C-a><cmd> <C-\><C-o>

nnoremap gc <cmd>execute "edit " . stdpath("config") . "/init.vim"<CR>a<ESC>
" nnoremap <Space><Space> <cmd>terminal<CR>a
nnoremap <Space><Space>
    \ <CMD>tabedit<CR><CMD>Telescope find_files hidden=true<CR>

"" set shell=powershell
"" set shellcmdflag=-command
"" set shellquote=\"
"" set shellxquote=

" nnoremap <C-h> <C-w>h
" nnoremap <C-j> <C-w>j
" nnoremap <C-k> <C-w>k
" nnoremap <C-l> <C-w>l

nnoremap _ <cmd>vertical resize -2<CR>
nnoremap = <cmd>resize +2<CR>
nnoremap - <cmd>resize -2<CR>
nnoremap + <cmd>vertical resize +2<CR>

" nnoremap <C-h> <cmd>vertical resize -2<CR>
" nnoremap <C-j> <cmd>resize -2<CR>
" nnoremap <C-k> <cmd>resize +2<CR>
" nnoremap <C-l> <cmd>vertical resize +2<CR>

" vnoremap < <gv
" vnoremap > >gv

let mapleader = ','

nnoremap <Leader>- <cmd>split<CR>
nnoremap <Leader>\| <cmd>vsplit<CR>
nnoremap <Leader>\ <cmd>vsplit<CR>

autocmd TermOpen * setlocal nonumber
autocmd CompleteDone * pclose

" autocmd User TelescopeFindPre * echo "PRE"

" filetype=TelescopePrompt

call plug#begin()
    " Plug 'glepnir/galaxyline.nvim' , { 'branch': 'main' }
    Plug 'catppuccin/nvim', { 'as': 'catppuccin' }
    Plug 'junegunn/fzf', { 'do': { -> fzf#install() } }
    Plug 'nvim-lua/plenary.nvim'
    Plug 'nvim-lualine/lualine.nvim'
    Plug 'nvim-telescope/telescope.nvim', { 'tag': '0.1.2' }
    Plug 'nvim-telescope/telescope-symbols.nvim'
    Plug 'nvim-tree/nvim-web-devicons'
    Plug 'nvim-treesitter/nvim-treesitter', {'do': ':TSUpdate'}
    Plug 'xiyaowong/transparent.nvim'

    Plug 'airblade/vim-gitgutter'
    Plug 'f-person/git-blame.nvim'

    Plug 'folke/twilight.nvim'
    Plug 'folke/zen-mode.nvim'

    Plug 'christoomey/vim-tmux-navigator'
    Plug 'RyanMillerC/better-vim-tmux-resizer'
call plug#end()

let g:tmux_navigator_no_mappings = 1
let g:tmux_navigator_disable_when_zoomed = 1
let g:tmux_navigator_preserve_zoom = 1

nnoremap <silent> <C-a>h <cmd>TmuxNavigateLeft<CR>
nnoremap <silent> <C-a>j <cmd>TmuxNavigateDown<CR>
nnoremap <silent> <C-a>k <cmd>TmuxNavigateUp<CR>
nnoremap <silent> <C-a>l <cmd>TmuxNavigateRight<CR>

let g:tmux_resizer_no_mappings = 1

nnoremap <silent> <C-a>H :TmuxResizeLeft<CR>
nnoremap <silent> <C-a>J :TmuxResizeDown<CR>
nnoremap <silent> <C-a>K :TmuxResizeUp<CR>
nnoremap <silent> <C-a>L :TmuxResizeRight<CR>

let g:gitgutter_highlight_lines = 1
let g:gitgutter_highlight_linenrs = 1
let g:gitgutter_signs = 0

let g:gitblame_enabled = 1
let g:gitblame_message_template = '         <sha> - <author> - <summary>'
let g:gitblame_message_when_not_committed = '         «unstaged»'

" colorscheme desert
colorscheme catppuccin-mocha

highlight OverLengthCaution ctermbg=yellow ctermfg=white guibg=#592929
highlight OverLengthDanger ctermbg=red ctermfg=white guibg=#592929

" match OverLengthCaution /\%81v.\{,6\}/
match OverLengthCaution /\%21v.\{,6\}/
match OverLengthDangrr /\%87v.\+/

set colorcolumn=+1


if !exists("g:init_vim")
    let g:init_vim=stdpath("config") . "/init.vim"
    function Reload()
        execute "source " .. fnameescape(g:init_vim)
    endfunction
endif


lua << END
require('lualine').setup()
require('transparent').setup()
END

nnoremap <Leader><Leader> <cmd>Telescope find_files<CR>
nnoremap <Leader><Space> <cmd>Telescope<CR>
nnoremap <Leader>b <cmd>Telescope buffers<CR>
nnoremap <Leader>c <cmd>Telescope treesitter<CR>
nnoremap <Leader>g <cmd>Telescope live_grep<CR>
nnoremap <Leader>h <cmd>Telescope help_tags<CR>
nnoremap <Leader>t <cmd>Telescope tags<CR>
nnoremap <Leader>/ <cmd>Telescope current_buffer_fuzzy_find<CR>

nnoremap gh        <Plug>(GitGutterNextHunk)
nnoremap gH        <Plug>(GitGutterPrevHunk)
nnoremap <Leader>s <Plug>(GitGutterStageHunk)

nnoremap <Leader>z <cmd>ZenMode<CR>
nnoremap <Leader>l <cmd>Twilight<CR>
