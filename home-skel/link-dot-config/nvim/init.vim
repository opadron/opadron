colorscheme ron

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

nnoremap <C-h> <C-w>h
nnoremap <C-j> <C-w>j
nnoremap <C-k> <C-w>k
nnoremap <C-l> <C-w>l

inoremap <C-h> <C-\><C-N><C-w>h
inoremap <C-j> <C-\><C-N><C-w>j
inoremap <C-k> <C-\><C-N><C-w>k
inoremap <C-l> <C-\><C-N><C-w>l

inoremap <C-h> <C-\><C-N><C-w>h
inoremap <C-j> <C-\><C-N><C-w>j
inoremap <C-k> <C-\><C-N><C-w>k
inoremap <C-l> <C-\><C-N><C-w>l

nnoremap _ <cmd>vertical resize -2<CR>
nnoremap = <cmd>resize +2<CR>
nnoremap - <cmd>resize -2<CR>
nnoremap + <cmd>vertical resize +2<CR>

" vnoremap < <gv
" vnoremap > >gv

let mapleader = ','

nnoremap <Leader>- <cmd>split<CR>
nnoremap <Leader>\| <cmd>vsplit<CR>

autocmd TermOpen * setlocal nonumber

" autocmd User TelescopeFindPre * echo "PRE"

" filetype=TelescopePrompt

call plug#begin()
    Plug 'catppuccin/nvim', { 'as': 'catppuccin' }
    " Plug 'glepnir/galaxyline.nvim' , { 'branch': 'main' }
    Plug 'nvim-lualine/lualine.nvim'
    Plug 'nvim-tree/nvim-web-devicons'
    Plug 'nvim-lua/plenary.nvim'
    Plug 'nvim-telescope/telescope.nvim', { 'tag': '0.1.2' }
    Plug 'xiyaowong/transparent.nvim'

    Plug 'nvim-treesitter/nvim-treesitter', {'do': ':TSUpdate'}
call plug#end()

" colorscheme desert
colorscheme catppuccin-mocha

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
