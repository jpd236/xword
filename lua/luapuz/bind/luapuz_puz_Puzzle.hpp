// ---------------------------------------------------------------------------
// luapuz_puz_Puzzle.hpp was generated by puzbind.lua
//
// Any changes made to this file will be lost when the file is regenerated.
// ---------------------------------------------------------------------------

#ifndef luapuz_puz_Puzzle_hpp
#define luapuz_puz_Puzzle_hpp

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "../luapuz_functions.hpp"
#include "../luapuz_tracking.hpp"

// ---------------------------------------------------------------------------
// class Puzzle
// ---------------------------------------------------------------------------

#include "puz/Puzzle.hpp"

LUAPUZ_API extern const char * Puzzle_meta;

// Puzzle userdata
// Userdata member should_gc is used to suppress garbage collection of the
// actual data.
// Calling luapuz_newPuzzle() with default parameters enables
// garbage collection.
// Calling luapuz_pushPuzzle/Ref() with default parameters
// suppresses garbage collection.
struct LUAPUZ_API Puzzle_ud
{
    puz::Puzzle * puzzle;
    bool should_gc;
};

// Get the userdata
inline Puzzle_ud * luapuz_checkPuzzle_ud(lua_State * L, int index)
{
    return (Puzzle_ud *)luaL_checkudata(L, index, Puzzle_meta);
}

// Get the actual data
inline puz::Puzzle * luapuz_checkPuzzle(lua_State * L, int index)
{
    Puzzle_ud * ud = luapuz_checkPuzzle_ud(L, index);
    if (! ud->puzzle)
        luaL_typerror(L, index, Puzzle_meta);
    return ud->puzzle;
}


// Check if this is the correct data type
inline bool luapuz_isPuzzle(lua_State *L, int index)
{
    return luapuz_isudata(L, index, Puzzle_meta);
}

// Create a new userdata with actual data and push it on the stack.
// The userdata will be tracked in the tracked objects table.
LUAPUZ_API void luapuz_newPuzzle(lua_State * L, puz::Puzzle * puzzle, bool should_gc = true);

// Push the actual data.
// If we have already tracked this userdata, push that userdata.
inline void luapuz_pushPuzzle(lua_State * L, puz::Puzzle * puzzle, bool should_gc = false)
{
    if (! puzzle)
        lua_pushnil(L);
    else if (! luapuz_push_tracked_object(L, puzzle))
        luapuz_newPuzzle(L, puzzle, should_gc);
}


void luapuz_openPuzzlelib (lua_State *L);
#endif // luapuz_puz_Puzzle_hpp
