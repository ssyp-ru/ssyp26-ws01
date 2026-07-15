# Как подключить к nvim

## 1. Build

```sh
make build      # produces ./nani.so
```

## 2. Add to your nvim config

Add something like this to your Neovim Lua config (adjust the path):

```lua
local nani_dir = "/absolute/path/to/nani/treesitter"

-- Recognize *.nani files
vim.filetype.add({ extension = { nani = "nani" } })

local so_path = nani_dir .. "/nani.so"
if vim.uv.fs_stat(so_path) then
  -- Register the parser, loaded directly from this folder.
  vim.treesitter.language.add("nani", { path = so_path })

  -- Load the highlight query directly from this folder (it is not
  -- installed into a runtimepath queries/nani/ directory).
  local f = io.open(nani_dir .. "/queries/highlights.scm", "r")
  if f then
    vim.treesitter.query.set("nani", "highlights", f:read("*a"))
    f:close()
  end
end

-- Start treesitter highlighting for .nani buffers.
vim.api.nvim_create_autocmd("FileType", {
  pattern = "nani",
  callback = function(args)
    pcall(vim.treesitter.start, args.buf, "nani")
  end,
})
```

If you already have a generic autocmd elsewhere that does
`pcall(vim.treesitter.start, args.buf)` for every filetype, you don't
need the last autocmd block above — just the `filetype.add`,
`language.add`, and `query.set` calls are enough.

## 3. Reload

Restart Neovim (or `:source` the file) and open a `.nani` file.
`:InspectTree` shows the live parse tree; `:Inspect` under the cursor
shows which highlight capture (if any) applies.
