#include "doctest.h"
#include "puz/Puzzle.hpp"
#include "puz/Square.hpp"

using namespace puz;

TEST_CASE("Square: directional solutions")
{
    Puzzle puz;
    puz.GetGrid().SetSize(2, 2);
    Square & sq = puz.GetGrid().At(0, 0);

    std::vector<Square::SolutionEntry> entries;
    entries.push_back(Square::SolutionEntry(puzT("CAT"), puzT("Across")));
    entries.push_back(Square::SolutionEntry(puzT("DOG"), puzT("Down")));

    sq.SetSolutions(entries);

    CHECK(sq.HasMultipleSolutions());
    CHECK(sq.HasOnlyDirectionalSolutions());
    CHECK(sq.GetSolution() == puzT("CAT/DOG"));
    CHECK(sq.GetPlainSolution() == 'C');

    SUBCASE("Strict check accepts permutations and trimmed tokens")
    {
        sq.SetText(puzT("CAT/DOG"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("DOG/CAT"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("  CAT  /  DOG  "));
        CHECK(sq.Check(false, true));
    }

    SUBCASE("Strict check rejects incomplete, invalid, or extra entries")
    {
        sq.SetText(puzT("CAT"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("DOG"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("C"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("D"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("C/D"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("CAT/COW"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("COW/DOG"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("CAT/DOG/FOX"));
        CHECK_FALSE(sq.Check(false, true));
    }

    SUBCASE("Non-strict check accepts individual candidates, initials, and permutations")
    {
        sq.SetText(puzT("C"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("D"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("CAT"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("DOG"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("CAT/DOG"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("C/D"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("D/C"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("X"));
        CHECK_FALSE(sq.Check(false, false));

        sq.SetText(puzT("COW"));
        CHECK_FALSE(sq.Check(false, false));

        sq.SetText(puzT("C/X"));
        CHECK_FALSE(sq.Check(false, false));

        sq.SetText(puzT("C/C"));
        CHECK_FALSE(sq.Check(false, false));
    }
}

TEST_CASE("Square: Schrödinger solutions (rebus words)")
{
    Puzzle puz;
    puz.GetGrid().SetSize(2, 2);
    Square & sq = puz.GetGrid().At(0, 0);

    std::vector<Square::SolutionEntry> entries;
    entries.push_back(Square::SolutionEntry(puzT("CAT")));
    entries.push_back(Square::SolutionEntry(puzT("DOG")));

    sq.SetSolutions(entries);

    CHECK(sq.HasMultipleSolutions());
    CHECK_FALSE(sq.HasOnlyDirectionalSolutions());
    CHECK(sq.GetSolution() == puzT("CAT"));
    CHECK(sq.GetPlainSolution() == 'C');

    SUBCASE("Strict check accepts individual full words and slash combinations")
    {
        sq.SetText(puzT("CAT"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("DOG"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("CAT/DOG"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("DOG/CAT"));
        CHECK(sq.Check(false, true));
    }

    SUBCASE("Strict check rejects single initials and invalid answers")
    {
        sq.SetText(puzT("C"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("D"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("C/D"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("OTHER"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("CAT/OTHER"));
        CHECK_FALSE(sq.Check(false, true));
    }

    SUBCASE("Non-strict check accepts initials and slash pairs")
    {
        sq.SetText(puzT("C"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("D"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("CAT"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("DOG"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("C/D"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("D/C"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("X"));
        CHECK_FALSE(sq.Check(false, false));

        sq.SetText(puzT("COW"));
        CHECK_FALSE(sq.Check(false, false));

        sq.SetText(puzT("C/X"));
        CHECK_FALSE(sq.Check(false, false));

        sq.SetText(puzT("C/C"));
        CHECK_FALSE(sq.Check(false, false));

        sq.SetText(puzT("C/D/A"));
        CHECK_FALSE(sq.Check(false, false));
    }
}

TEST_CASE("Square: Schrödinger solutions (single letters)")
{
    Puzzle puz;
    puz.GetGrid().SetSize(2, 2);
    Square & sq = puz.GetGrid().At(0, 0);

    std::vector<Square::SolutionEntry> entries;
    entries.push_back(Square::SolutionEntry(puzT("C")));
    entries.push_back(Square::SolutionEntry(puzT("B")));

    sq.SetSolutions(entries);

    CHECK(sq.HasMultipleSolutions());
    CHECK_FALSE(sq.HasOnlyDirectionalSolutions());
    CHECK(sq.GetSolution() == puzT("C"));
    CHECK(sq.GetPlainSolution() == 'C');

    SUBCASE("Strict check accepts either letter or both slash-separated")
    {
        sq.SetText(puzT("C"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("B"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("C/B"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("B/C"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("  C  /  B  "));
        CHECK(sq.Check(false, true));
    }

    SUBCASE("Strict check rejects wrong letters or duplicate tokens")
    {
        sq.SetText(puzT("X"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("C/X"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("C/C"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("C/B/A"));
        CHECK_FALSE(sq.Check(false, true));
    }
}

TEST_CASE("Square: directional with multiple candidates per direction")
{
    Puzzle puz;
    puz.GetGrid().SetSize(2, 2);
    Square & sq = puz.GetGrid().At(0, 0);

    std::vector<Square::SolutionEntry> entries;
    entries.push_back(Square::SolutionEntry(puzT("CAT"), puzT("Across")));
    entries.push_back(Square::SolutionEntry(puzT("KITTY"), puzT("Across")));
    entries.push_back(Square::SolutionEntry(puzT("DOG"), puzT("Down")));

    sq.SetSolutions(entries);

    CHECK(sq.HasMultipleSolutions());
    CHECK(sq.HasOnlyDirectionalSolutions());
    CHECK(sq.GetSolution() == puzT("CAT/DOG"));
    CHECK(sq.GetPlainSolution() == 'C');

    SUBCASE("Strict check accepts valid multi-candidate combinations")
    {
        sq.SetText(puzT("CAT/DOG"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("KITTY/DOG"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("DOG/KITTY"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("DOG/CAT"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("KITTY"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("CAT"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("DOG"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("CAT/KITTY"));
        CHECK_FALSE(sq.Check(false, true));
    }

    SUBCASE("Non-strict check accepts individual candidates and initials")
    {
        sq.SetText(puzT("CAT"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("KITTY"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("DOG"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("C"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("K"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("D"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("C/D"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("K/D"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("D/C"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("D/K"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("X"));
        CHECK_FALSE(sq.Check(false, false));

        sq.SetText(puzT("COW"));
        CHECK_FALSE(sq.Check(false, false));

        sq.SetText(puzT("C/X"));
        CHECK_FALSE(sq.Check(false, false));

        sq.SetText(puzT("K/X"));
        CHECK_FALSE(sq.Check(false, false));

        sq.SetText(puzT("C/C"));
        CHECK_FALSE(sq.Check(false, false));

        // Both C and K are Across; neither is Down
        sq.SetText(puzT("C/K"));
        CHECK_FALSE(sq.Check(false, false));
    }
}

TEST_CASE("Square: identical directional answers")
{
    Puzzle puz;
    puz.GetGrid().SetSize(2, 2);
    Square & sq = puz.GetGrid().At(0, 0);

    std::vector<Square::SolutionEntry> entries;
    entries.push_back(Square::SolutionEntry(puzT("A"), puzT("Across")));
    entries.push_back(Square::SolutionEntry(puzT("A"), puzT("Down")));

    sq.SetSolutions(entries);

    CHECK(sq.HasMultipleSolutions());
    CHECK(sq.HasOnlyDirectionalSolutions());
    CHECK(sq.GetSolution() == puzT("A/A"));
    CHECK(sq.GetPlainSolution() == 'A');

    // Strict checking
    sq.SetText(puzT("A/A"));
    CHECK(sq.Check(false, true));

    sq.SetText(puzT("a/a"));
    CHECK(sq.Check(false, true));

    sq.SetText(puzT("A"));
    CHECK_FALSE(sq.Check(false, true));

    // Non-strict checking
    sq.SetText(puzT("A"));
    CHECK(sq.Check(false, false));

    sq.SetText(puzT("a"));
    CHECK(sq.Check(false, false));

    sq.SetText(puzT("B"));
    CHECK_FALSE(sq.Check(false, false));
}

TEST_CASE("Square: 3 directions")
{
    Puzzle puz;
    puz.GetGrid().SetSize(2, 2);
    Square & sq = puz.GetGrid().At(0, 0);

    std::vector<Square::SolutionEntry> entries;
    entries.push_back(Square::SolutionEntry(puzT("CAT"), puzT("Across")));
    entries.push_back(Square::SolutionEntry(puzT("FOX"), puzT("Diagonal")));
    entries.push_back(Square::SolutionEntry(puzT("DOG"), puzT("Down")));

    sq.SetSolutions(entries);

    CHECK(sq.HasMultipleSolutions());
    CHECK(sq.HasOnlyDirectionalSolutions());
    CHECK(sq.GetSolution() == puzT("CAT/FOX/DOG"));
    CHECK(sq.GetPlainSolution() == 'C');

    // Strict: any 3-permutation passes
    sq.SetText(puzT("FOX/CAT/DOG"));
    CHECK(sq.Check(false, true));

    sq.SetText(puzT("DOG/FOX/CAT"));
    CHECK(sq.Check(false, true));

    sq.SetText(puzT("CAT/DOG"));
    CHECK_FALSE(sq.Check(false, true));

    sq.SetText(puzT("CAT/DOG/FOX/COW"));
    CHECK_FALSE(sq.Check(false, true));

    // Non-strict
    sq.SetText(puzT("F"));
    CHECK(sq.Check(false, false));

    sq.SetText(puzT("FOX"));
    CHECK(sq.Check(false, false));
}

TEST_CASE("Square: canonical override and fallback value")
{
    Puzzle puz;
    puz.GetGrid().SetSize(2, 2);
    Square & sq = puz.GetGrid().At(0, 0);

    SUBCASE("Fallback entry with directional alternates")
    {
        std::vector<Square::SolutionEntry> entries;
        entries.push_back(Square::SolutionEntry(puzT("FALLBACK")));
        entries.push_back(Square::SolutionEntry(puzT("CAT"), puzT("Across")));
        entries.push_back(Square::SolutionEntry(puzT("DOG"), puzT("Down")));

        sq.SetSolutions(entries, puzT("FALLBACK"));

        CHECK(sq.HasMultipleSolutions());
        CHECK_FALSE(sq.HasOnlyDirectionalSolutions());
        CHECK(sq.GetSolution() == puzT("FALLBACK"));
        CHECK(sq.GetPlainSolution() == 'F');

        sq.SetText(puzT("FALLBACK"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("CAT"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("DOG"));
        CHECK(sq.Check(false, true));

        sq.SetText(puzT("OTHER"));
        CHECK_FALSE(sq.Check(false, true));

        sq.SetText(puzT("F"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("C"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("D"));
        CHECK(sq.Check(false, false));

        sq.SetText(puzT("X"));
        CHECK_FALSE(sq.Check(false, false));
    }

    SUBCASE("Explicit canonical override on directional solutions")
    {
        std::vector<Square::SolutionEntry> entries;
        entries.push_back(Square::SolutionEntry(puzT("CAT"), puzT("Across")));
        entries.push_back(Square::SolutionEntry(puzT("DOG"), puzT("Down")));

        sq.SetSolutions(entries, puzT("SPECIAL"));

        CHECK(sq.HasMultipleSolutions());
        CHECK(sq.HasOnlyDirectionalSolutions());
        CHECK(sq.GetSolution() == puzT("SPECIAL"));
        CHECK(sq.GetPlainSolution() == 'S');
    }
}

TEST_CASE("Square: blank canonical solution with alternative entries")
{
    Puzzle puz;
    puz.GetGrid().SetSize(2, 2);
    Square & sq = puz.GetGrid().At(0, 0);

    std::vector<Square::SolutionEntry> entries;
    entries.push_back(Square::SolutionEntry(puzT("BLANK")));
    entries.push_back(Square::SolutionEntry(puzT("EMPTY")));

    sq.SetSolutions(entries, puzT(""));

    CHECK(sq.IsSolutionBlank());
    CHECK(sq.GetSolution() == Square::Blank);
    CHECK(sq.GetPlainSolution() == ' ');
    CHECK(sq.HasMultipleSolutions());

    sq.SetText(puzT(""));
    CHECK(sq.Check());

    sq.SetText(puzT("BLANK"));
    CHECK(sq.Check());

    sq.SetText(puzT("EMPTY"));
    CHECK(sq.Check());

    sq.SetText(puzT("WRONG"));
    CHECK_FALSE(sq.Check());
}

TEST_CASE("Square: SetSolution and ClearSolutions reset state")
{
    Puzzle puz;
    puz.GetGrid().SetSize(2, 2);
    Square & sq = puz.GetGrid().At(0, 0);

    std::vector<Square::SolutionEntry> entries;
    entries.push_back(Square::SolutionEntry(puzT("CAT"), puzT("Across")));
    entries.push_back(Square::SolutionEntry(puzT("DOG"), puzT("Down")));

    sq.SetSolutions(entries);
    CHECK(sq.HasMultipleSolutions());

    sq.SetSolution(puzT("SOLO"));
    CHECK_FALSE(sq.HasMultipleSolutions());
    CHECK_FALSE(sq.HasOnlyDirectionalSolutions());
    CHECK(sq.GetSolutions().empty());
    CHECK(sq.GetSolution() == puzT("SOLO"));

    sq.SetText(puzT("SOLO"));
    CHECK(sq.Check());

    sq.SetText(puzT("CAT"));
    CHECK_FALSE(sq.Check());

    sq.ClearSolutions();
    CHECK_FALSE(sq.HasMultipleSolutions());
    CHECK(sq.IsSolutionBlank());
}

TEST_CASE("Square: copying and assignment")
{
    Puzzle puz1;
    puz1.GetGrid().SetSize(2, 2);
    Square & sq1 = puz1.GetGrid().At(0, 0);

    std::vector<Square::SolutionEntry> entries;
    entries.push_back(Square::SolutionEntry(puzT("CAT"), puzT("Across")));
    entries.push_back(Square::SolutionEntry(puzT("DOG"), puzT("Down")));
    sq1.SetSolutions(entries);

    SUBCASE("Copy constructor via Puzzle")
    {
        Puzzle puz2 = puz1;
        Square & sq2 = puz2.GetGrid().At(0, 0);
        CHECK(sq2.HasMultipleSolutions());
        CHECK(sq2.HasOnlyDirectionalSolutions());
        CHECK(sq2.GetSolution() == puzT("CAT/DOG"));

        sq2.SetText(puzT("DOG/CAT"));
        CHECK(sq2.Check(false, true));
    }

    SUBCASE("Assignment operator")
    {
        Square & sq3 = puz1.GetGrid().At(1, 1);
        sq3 = sq1;
        CHECK(sq3.HasMultipleSolutions());
        CHECK(sq3.HasOnlyDirectionalSolutions());
        CHECK(sq3.GetSolution() == puzT("CAT/DOG"));

        sq3.SetText(puzT("CAT/DOG"));
        CHECK(sq3.Check(false, true));
    }
}
