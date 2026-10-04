#include "doctest.h"
#include "puz/Puzzle.hpp"
#include "puz/Square.hpp"

using namespace puz;

TEST_CASE("iPuz parsing: Directional Rebus ({ Across: CAT, Down: DOG })")
{
    std::string ipuz_data = R"({
        "version": "http://ipuz.org/v2",
        "kind": [ "http://ipuz.org/crossword#1" ],
        "dimensions": { "width": 3, "height": 3 },
        "puzzle": [
            [ 1, 2, "#" ],
            [ 3, 0, 4 ],
            [ "#", 5, 0 ]
        ],
        "solution": [
            [ { "Across": "CAT", "Down": "DOG" }, "A", "#" ],
            [ "B", "O", "T" ],
            [ "#", "L", "O" ]
        ],
        "clues": {
            "Across": [ [ 1, "A1" ], [ 3, "A3" ], [ 5, "A5" ] ],
            "Down": [ [ 1, "D1" ], [ 2, "D2" ], [ 4, "D4" ] ]
        }
    })";

    Puzzle puz;
    puz.LoadIpuzString(ipuz_data);
    const Square & sq = puz.GetGrid().At(0, 0);

    CHECK(sq.HasMultipleSolutions());
    CHECK(sq.HasOnlyDirectionalSolutions());
    CHECK_FALSE(sq.IsSolutionBlank());
    CHECK(sq.GetSolution() == puzT("CAT/DOG"));
    CHECK(sq.GetPlainSolution() == 'C');

    const std::vector<Square::SolutionEntry> & entries = sq.GetSolutions();
    REQUIRE(entries.size() == 2);
    CHECK(entries[0].value == puzT("CAT"));
    CHECK(entries[0].direction == puzT("Across"));
    CHECK(entries[1].value == puzT("DOG"));
    CHECK(entries[1].direction == puzT("Down"));
}

TEST_CASE("iPuz parsing: Schrödinger Array ([ CAT, DOG ])")
{
    std::string ipuz_data = R"({
        "version": "http://ipuz.org/v2",
        "kind": [ "http://ipuz.org/crossword#1" ],
        "dimensions": { "width": 3, "height": 3 },
        "puzzle": [
            [ 1, 2, "#" ],
            [ 3, 0, 4 ],
            [ "#", 5, 0 ]
        ],
        "solution": [
            [ [ "CAT", "DOG" ], "A", "#" ],
            [ "B", "O", "T" ],
            [ "#", "L", "O" ]
        ],
        "clues": {
            "Across": [ [ 1, "A1" ], [ 3, "A3" ], [ 5, "A5" ] ],
            "Down": [ [ 1, "D1" ], [ 2, "D2" ], [ 4, "D4" ] ]
        }
    })";

    Puzzle puz;
    puz.LoadIpuzString(ipuz_data);
    const Square & sq = puz.GetGrid().At(0, 0);

    CHECK(sq.HasMultipleSolutions());
    CHECK_FALSE(sq.HasOnlyDirectionalSolutions());
    CHECK_FALSE(sq.IsSolutionBlank());
    CHECK(sq.GetSolution() == puzT("CAT"));
    CHECK(sq.GetPlainSolution() == 'C');

    const std::vector<Square::SolutionEntry> & entries = sq.GetSolutions();
    REQUIRE(entries.size() == 2);
    CHECK(entries[0].value == puzT("CAT"));
    CHECK(entries[0].direction.empty());
    CHECK(entries[1].value == puzT("DOG"));
    CHECK(entries[1].direction.empty());
}

TEST_CASE("iPuz parsing: Object with explicit 'value' fallback")
{
    std::string ipuz_data = R"({
        "version": "http://ipuz.org/v2",
        "kind": [ "http://ipuz.org/crossword#1" ],
        "dimensions": { "width": 3, "height": 3 },
        "puzzle": [
            [ 1, 2, "#" ],
            [ 3, 0, 4 ],
            [ "#", 5, 0 ]
        ],
        "solution": [
            [ { "value": "FALLBACK", "Across": "CAT", "Down": "DOG" }, "A", "#" ],
            [ "B", "O", "T" ],
            [ "#", "L", "O" ]
        ],
        "clues": {
            "Across": [ [ 1, "A1" ], [ 3, "A3" ], [ 5, "A5" ] ],
            "Down": [ [ 1, "D1" ], [ 2, "D2" ], [ 4, "D4" ] ]
        }
    })";

    Puzzle puz;
    puz.LoadIpuzString(ipuz_data);
    const Square & sq = puz.GetGrid().At(0, 0);

    CHECK(sq.HasMultipleSolutions());
    CHECK_FALSE(sq.HasOnlyDirectionalSolutions());
    CHECK_FALSE(sq.IsSolutionBlank());
    CHECK(sq.GetSolution() == puzT("FALLBACK"));
    CHECK(sq.GetPlainSolution() == 'F');

    const std::vector<Square::SolutionEntry> & entries = sq.GetSolutions();
    REQUIRE(entries.size() == 3);
    CHECK(entries[0].value == puzT("FALLBACK"));
    CHECK(entries[0].direction.empty());
    CHECK(entries[1].value == puzT("CAT"));
    CHECK(entries[1].direction == puzT("Across"));
    CHECK(entries[2].value == puzT("DOG"));
    CHECK(entries[2].direction == puzT("Down"));
}

TEST_CASE("iPuz parsing: Directional with multiple candidates per direction")
{
    std::string ipuz_data = R"({
        "version": "http://ipuz.org/v2",
        "kind": [ "http://ipuz.org/crossword#1" ],
        "dimensions": { "width": 3, "height": 3 },
        "puzzle": [
            [ 1, 2, "#" ],
            [ 3, 0, 4 ],
            [ "#", 5, 0 ]
        ],
        "solution": [
            [ { "Across": [ "CAT", "KITTY" ], "Down": "DOG" }, "A", "#" ],
            [ "B", "O", "T" ],
            [ "#", "L", "O" ]
        ],
        "clues": {
            "Across": [ [ 1, "A1" ], [ 3, "A3" ], [ 5, "A5" ] ],
            "Down": [ [ 1, "D1" ], [ 2, "D2" ], [ 4, "D4" ] ]
        }
    })";

    Puzzle puz;
    puz.LoadIpuzString(ipuz_data);
    const Square & sq = puz.GetGrid().At(0, 0);

    CHECK(sq.HasMultipleSolutions());
    CHECK(sq.HasOnlyDirectionalSolutions());
    CHECK_FALSE(sq.IsSolutionBlank());
    CHECK(sq.GetSolution() == puzT("CAT/DOG"));
    CHECK(sq.GetPlainSolution() == 'C');

    const std::vector<Square::SolutionEntry> & entries = sq.GetSolutions();
    REQUIRE(entries.size() == 3);
    CHECK(entries[0].value == puzT("CAT"));
    CHECK(entries[0].direction == puzT("Across"));
    CHECK(entries[1].value == puzT("KITTY"));
    CHECK(entries[1].direction == puzT("Across"));
    CHECK(entries[2].value == puzT("DOG"));
    CHECK(entries[2].direction == puzT("Down"));
}

TEST_CASE("iPuz parsing: Identical directional answers ({ Across: A, Down: A })")
{
    std::string ipuz_data = R"({
        "version": "http://ipuz.org/v2",
        "kind": [ "http://ipuz.org/crossword#1" ],
        "dimensions": { "width": 3, "height": 3 },
        "puzzle": [
            [ 1, 2, "#" ],
            [ 3, 0, 4 ],
            [ "#", 5, 0 ]
        ],
        "solution": [
            [ { "Across": "A", "Down": "A" }, "A", "#" ],
            [ "B", "O", "T" ],
            [ "#", "L", "O" ]
        ],
        "clues": {
            "Across": [ [ 1, "A1" ], [ 3, "A3" ], [ 5, "A5" ] ],
            "Down": [ [ 1, "D1" ], [ 2, "D2" ], [ 4, "D4" ] ]
        }
    })";

    Puzzle puz;
    puz.LoadIpuzString(ipuz_data);
    const Square & sq = puz.GetGrid().At(0, 0);

    CHECK(sq.HasMultipleSolutions());
    CHECK(sq.HasOnlyDirectionalSolutions());
    CHECK_FALSE(sq.IsSolutionBlank());
    CHECK(sq.GetSolution() == puzT("A/A"));
    CHECK(sq.GetPlainSolution() == 'A');

    const std::vector<Square::SolutionEntry> & entries = sq.GetSolutions();
    REQUIRE(entries.size() == 2);
    CHECK(entries[0].value == puzT("A"));
    CHECK(entries[0].direction == puzT("Across"));
    CHECK(entries[1].value == puzT("A"));
    CHECK(entries[1].direction == puzT("Down"));
}

TEST_CASE("iPuz parsing: 3 directions ({ Across: CAT, Down: DOG, Diagonal: FOX })")
{
    std::string ipuz_data = R"({
        "version": "http://ipuz.org/v2",
        "kind": [ "http://ipuz.org/crossword#1" ],
        "dimensions": { "width": 3, "height": 3 },
        "puzzle": [
            [ 1, 2, "#" ],
            [ 3, 0, 4 ],
            [ "#", 5, 0 ]
        ],
        "solution": [
            [ { "Across": "CAT", "Down": "DOG", "Diagonal": "FOX" }, "A", "#" ],
            [ "B", "O", "T" ],
            [ "#", "L", "O" ]
        ],
        "clues": {
            "Across": [ [ 1, "A1" ], [ 3, "A3" ], [ 5, "A5" ] ],
            "Down": [ [ 1, "D1" ], [ 2, "D2" ], [ 4, "D4" ] ]
        }
    })";

    Puzzle puz;
    puz.LoadIpuzString(ipuz_data);
    const Square & sq = puz.GetGrid().At(0, 0);

    CHECK(sq.HasMultipleSolutions());
    CHECK(sq.HasOnlyDirectionalSolutions());
    CHECK_FALSE(sq.IsSolutionBlank());
    CHECK(sq.GetSolution() == puzT("CAT/FOX/DOG"));
    CHECK(sq.GetPlainSolution() == 'C');

    const std::vector<Square::SolutionEntry> & entries = sq.GetSolutions();
    REQUIRE(entries.size() == 3);
    CHECK(entries[0].value == puzT("CAT"));
    CHECK(entries[0].direction == puzT("Across"));
    CHECK(entries[1].value == puzT("FOX"));
    CHECK(entries[1].direction == puzT("Diagonal"));
    CHECK(entries[2].value == puzT("DOG"));
    CHECK(entries[2].direction == puzT("Down"));
}

TEST_CASE("iPuz parsing: Blank canonical solution with alternative entries")
{
    std::string ipuz_data = R"({
        "version": "http://ipuz.org/v2",
        "kind": [ "http://ipuz.org/crossword#1" ],
        "dimensions": { "width": 2, "height": 2 },
        "puzzle": [
            [ 1, 2 ],
            [ 3, 0 ]
        ],
        "solution": [
            [ { "value": "", "Across": "BLANK" }, "A" ],
            [ "B", "C" ]
        ],
        "clues": {
            "Across": [ [ 1, "A1" ], [ 3, "A3" ] ],
            "Down": [ [ 1, "D1" ], [ 2, "D2" ] ]
        }
    })";

    Puzzle puz;
    puz.LoadIpuzString(ipuz_data);
    const Square & sq = puz.GetGrid().At(0, 0);

    CHECK(sq.IsSolutionBlank());
    CHECK(sq.GetSolution() == Square::Blank);
    CHECK(sq.GetPlainSolution() == ' ');
    CHECK(sq.HasMultipleSolutions());

    const std::vector<Square::SolutionEntry> & entries = sq.GetSolutions();
    REQUIRE(entries.size() == 2);
    CHECK(entries[0].value == puzT(""));
    CHECK(entries[0].direction.empty());
    CHECK(entries[1].value == puzT("BLANK"));
    CHECK(entries[1].direction == puzT("Across"));
}
