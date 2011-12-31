require 'luacurl'

function download.get_default_puzzles()
local sources = {
    {
        name = "NY Times Premium",
        url = "?",
        directoryname = "NY_Times",
        filename = "nyt%Y%m%d.puz",
        days = { true, true, true, true, true, true, true },
        func = "return download.download(puzzle.url, puzzle.filename, puzzle.curlopts)",
        fields = { "User Name", "Password", }
    },

    {
        name = "NY Times (XWord Info)",
        url = "http://www.xwordinfo.com/XPF/?date=%m/%d/%Y",
        directoryname = "NY_Times",
        filename = "nyt%Y%m%d.xml",
        days = { true, true, true, true, true, true, true },
        curlopts =
        {
            [curl.OPT_REFERER] = 'http://www.xwordinfo.com/',
        },
        func = "return download.download(puzzle.url, puzzle.filename, puzzle.curlopts)"
    },

    {
        name = "CrosSynergy",
        url = "http://www.washingtonpost.com/r/WashingtonPost/Content/Puzzles/Daily/cs%y%m%d.jpz",
        filename = "cs%Y%m%d.jpz",
        days = { true, true, true, true, true, true, true },
    },

    {
        name = "Newsday",
        url = "http://picayune.uclick.com/comics/crnet/data/crnet%y%m%d-data.xml",
        filename = "nd%Y%m%d.xml",
        days = { true, true, true, true, true, true, true },
    },

    {
        name = "LA Times",
        url = "http://www.cruciverb.com/puzzles/lat/lat%y%m%d.puz",
        filename = "lat%Y%m%d.puz",
        curlopts =
        {
            [curl.OPT_REFERER] = 'http://www.cruciverb.com/',
        },
        days = { true, true, true, true, true, true, true },
    },

    {
        name = "USA Today",
        url = "http://picayune.uclick.com/comics/usaon/data/usaon%y%m%d-data.xml",
        filename = "usa%Y%m%d.xml",
        days = { true, true, true, true, true, false, false },
    },
    {
        name = "Ink Well",
        url = "http://herbach.dnsalias.com/Tausig/vv%y%m%d.puz",
        filename = "tausig%Y%m%d.puz",
        days = { false, false, false, false, true, false, false },
    },

    {
        name = "The Onion AV Club",
        url = "http://herbach.dnsalias.com/Tausig/av%y%m%d.puz",
        filename = "av%Y%m%d.puz",
        days = { false, false, true, false, false, false, false },
    },

    {
        name = "Jonesin'",
        url = "http://herbach.dnsalias.com/Jonesin/jz%y%m%d.puz",
        filename = "jones%Y%m%d.puz",
        days = { false, false, false, true, false, false, false },
    },

    {
        name = "Wall Street Journal",
        url = "http://mazerlm.home.comcast.net/wsj%y%m%d.puz",
        filename = "wsj%Y%m%d.puz",
        days = { false, false, false, false, true, false, false },
    },

    {
        name = "Boston Globe",
        url = "http://home.comcast.net/~nshack/Puzzles/bg%y%m%d.puz",
        filename = "bg%Y%m%d.puz",
        days = { false, false, false, false, false, false, true },
    },

    {
        name = "Philadelphia Inquirer",
        url = "http://mazerlm.home.comcast.net/pi%y%m%d.puz",
        filename = "pi%Y%m%d.puz",
        days = { false, false, false, false, false, false, true },
    },

    {
        name = "The Chronicle of Higher Education",
        url = "http://chronicle.com/items/biz/puzzles/%Y%m%d.puz",
        filename = "che%Y%m%d.puz",
        days = { false, false, false, false, true, false, false },
    },

    {
        name = "Universal",
        url = "http://picayune.uclick.com/comics/fcx/data/fcx%y%m%d-data.xml",
        filename = "univ%Y%m%d.xml",
        days = { true, true, true, true, true, true, true },
    },

    {
        name = "Matt Gaffney's Weekly Crossword Contest",
        filename = "mgwcc%Y%m%d.puz",
        days = { false, false, false, false, true, false, false },
        -- Custom download function
        func = [[
    -- Download the page with puzzles for the month
    local archive = download.download(puzzle.date:fmt(
        "http://crosswordcontest.blogspot.com/%Y_%m_01_archive.html"))

    -- Search for the puzzle number for the current date
    local day = puzzle.date:fmt('%d')
    if day:sub(1,1) == '0' then day = day:sub(2) end
    local number = archive:match("MGWCC[^<]-(%d+)[^<]* " .. puzzle.date:fmt("%b") .. "[^<]* " .. day .. "[^,<]*, " .. puzzle.date:fmt("%Y"))

    -- Download the puzzle
    if number then
        -- Find the applet url
        local id, name = archive:match('"http://icrossword.com/embed/%?id=([^"]*)(mgwcc[^"]*' .. number .. '[^"]*)"')
        if id and name then
            return download.download(
                string.format("http://icrossword.com/publish/server/puzzle/index.php/%s?id=%s%s",
                              name, id, name),
                puzzle.filename)
        end
    end
    return nil, "Could not figure out puzzle number"
]]
    },

    {
        name = "Matt Gaffney's Daily Crossword",
        filename = "mgdc%Y%m%d.puz",
        days = { true, true, true, true, true, false, false },
        -- Custom download function
        func = [[
    -- Download the page with puzzles for the month
    local archive = download.download(puzzle.date:fmt(
        "http://mattgaffneydaily.blogspot.com/%Y_%m_01_archive.html"))

    -- Search for the puzzle number for the current date
    local day = puzzle.date:fmt('%d')
    if day:sub(1,1) == '0' then day = day:sub(2) end

    local number = archive:match("MGDC[^<]-(%d+)[^<]* " .. puzzle.date:fmt("%b") .. "[^<]* " .. day .. "[^,<%d]*, " .. puzzle.date:fmt("%Y"))

    -- Download the puzzle
    if number then
        -- Find the applet url
        local id, name = archive:match('"http://icrossword.com/embed/%?id=([^"]*)(mgdc[^"]*' .. number .. '[^"]*)"')
        if id and name then
            return download.download(
                string.format("http://icrossword.com/publish/server/puzzle/index.php/%s?id=%s%s",
                              name, id, name),
                puzzle.filename)
        end
    end
    return nil, "Could not figure out puzzle number"
]]
    },

    {
        name = "Brendan Emmett Quigley",
        url = "http://www.brendanemmettquigley.com/%Y/%m/%d",
        filename = "beq%Y%m%d.jpz",
        days = { true, false, false, true, false, false, false },
        -- Custom download function
        func = [[
    -- Download the page with the puzzle
    local page = download.download(puzzle.url)

    -- Search for a download link
    local url = page:match('<a href="(http://www.brendanemmettquigley.com/[^"]-.jpz)">')

    -- Download the puzzle
    if url then
        return download.download(url, puzzle.filename)
    end
    return nil, "Could not find a download link"
]]
    },
}

-- Make this into a class
local puzzles = {}
puzzles._order = {}

local mt = {}
mt.__index = mt

function mt:iter()
    local i = 0
    local n = #self._order
    return function ()
        i = i + 1
        if i <= n then
            local key = self._order[i]
            return key, self[key]
        end
    end
end

function mt:get(i)
    local p = self[i]
    if p then return p end
    local key = self._order[i]
    if key then return self[key] end
end

function mt:insert(puzzle, idx)
    -- Generate an id
    puzzle.id = puzzle.id or puzzle.name
    local i = 2
    while self[puzzle.id] ~= nil do
        puzzle.id = puzzle.name .. tostring(i)
        i = i + 1
    end
    -- Insert the puzzle
    self[puzzle.id] = puzzle
    if idx then
        table.insert(self._order, idx, puzzle.id)
    else
        table.insert(self._order, puzzle.id)
    end
end

function mt:remove(id)
    if type(id) == 'table' then
        id = id.id
    end
    for i, puzid in ipairs(self._order) do
        if puzid == id then
            table.remove(self._order, i)
        end
    end
    self[id] = nil
end

setmetatable(puzzles, mt)

for _, puzzle in ipairs(sources) do
    puzzles:insert(puzzle)
end
for k,_ in pairs(sources) do sources[k] = nil end

return puzzles

end -- function get_default_puzzles

download.puzzles = download.get_default_puzzles()

-- Update the download sources
require 'download.config'