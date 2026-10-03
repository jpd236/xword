// This file is part of XWord
// Copyright (C) 2011 Mike Richards ( mrichards42@gmx.com )
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include <cwctype>
#include <algorithm>
#include "Square.hpp"
#include "Grid.hpp"
#include "puzstring.hpp"

#include <cstring>

//#define PUZ_CHECK_STRINGS

namespace puz {

//------------------------------------------------------------------------------
// Constructors and operator=()
//------------------------------------------------------------------------------

// Default constructor
// Square is white and blank.
Square::Square()
    : m_col(-1),
      m_row(-1),
      m_flag(FLAG_CLEAR),
      m_number(),
      m_red(255),
      m_green(255),
      m_blue(255),
      m_bars()
{
    SetText(puzT(""));
    SetSolution(puzT(""));

    // These will be filled in by the grid
    m_next.clear();
}

// Copy constructor
// Square is copied because it is in a std::vector and needs to
// be relocated (Grid::SetSize() was caled).  Spatial information
// must be reset.  Linked-list information will be filled in
// by the grid.
Square::Square(const Square & other)
    : m_col(other.m_col),
      m_row(other.m_row),
      m_asciiSolution(other.m_asciiSolution),
      m_solution(other.m_solution),
      m_text(other.m_text),
      m_flag(other.m_flag),
      m_number(other.m_number),
      m_red(other.m_red),
      m_green(other.m_green),
      m_blue(other.m_blue),
      m_solutions(other.m_solutions)
{
    m_next = SquareDirectionMap(m_next);
    std::memcpy(m_bars, other.m_bars, 4 * sizeof(bool));
}

// Since all constructors are private, only a grid can create squares.
// Thus, this function will only be called as follows:
//     grid1.At(col, row) = grid2.At(col, row);
// In order to preserve iteration, we need to prevent grid information
// from being copied.
Square & Square::operator=(const Square & other)
{
    m_asciiSolution = other.m_asciiSolution;
    m_solution = other.m_solution;
    m_solutions = other.m_solutions;
    m_text = other.m_text;
    m_flag = other.m_flag;

    m_red = other.m_red;
    m_green = other.m_green;
    m_blue = other.m_blue;

    m_number = other.m_number;

    // Don't copy grid information

    return *this;
}


// Character tables and the definition of Square::Blank and Square::Black
#include "char_tables.hpp"

//------------------------------------------------------------------------------
// Character table functions (static)
//------------------------------------------------------------------------------


char Square::ToPlain(int ch)
{
    if (ch > 0 && ch < (sizeof(ascii) / sizeof(char)))
        return ascii[ch];
    return 0;
}

char Square::ToPlain(const string_t & str)
{
    if (str.empty())
        return ToPlain(Blank);
    char ch = ToPlain(to_unicode(str).at(0));
    if (ch == 0)
        return ToPlain(Blank);
    return ch;
}

char_t Square::ToGrid(int ch)
{
    if (ch == Black[0] || ch == puzT('['))
        return 0;

#if PUZ_UNICODE
    return iswprint(ch) ? towupper(ch) : 0;
#else
    return isprint(ch) ? toupper(ch) : 0;
#endif // PUZ_UNICODE
}

string_t Square::ToGrid(const string_t & str)
{
#ifdef PUZ_CHECK_STRINGS
    if (str.length() > REBUS_ENTRY_LENGTH)
        throw LongString();
#endif 
    string_t ret;
    string_t::const_iterator it;
    for (it = str.begin(); it != str.end(); ++it)
    {
        char_t ch = ToGrid(*it);
        if (ch != 0)
            ret.append(1, ch);
#ifdef PUZ_CHECK_STRINGS
        else
            throw InvalidString();
#endif
    }
    return ret;
}

bool Square::IsValidChar(int ch)
{
    return ToGrid(ch) != 0;
}


bool Square::IsValidString(const string_t & str)
{
    if (str == Black)
        return true;

#if PUZ_UNICODE
    std::wstring::const_iterator begin = str.begin();
    std::wstring::const_iterator end   = str.end();
#else
    std::wstring unicode = to_unicode(str);
    std::wstring::const_iterator begin = unicode.begin();
    std::wstring::const_iterator end   = unicode.end();
#endif // PUZ_UNICODE

    std::wstring::const_iterator it;
    for (it = begin; it != end; ++it)
        if (! IsValidChar(*it))
            return false;
    return true;
}

//------------------------------------------------------------------------------
// Word start and end
//------------------------------------------------------------------------------

// Whether there is a bar in the given direction from the current square.
// This is either a bar in this square, or the next square (in the opposite direction).
// Preconditions: Not at the end of the grid (i.e. a next square exists in this direction).
bool HasBarInDirection(const Square* square, GridDirection dir)
{
    int thisSquareBarDirection, nextSquareBarDirection;
    switch (dir) {
    case GridDirection::RIGHT:
        thisSquareBarDirection = puz::BAR_RIGHT;
        nextSquareBarDirection = puz::BAR_LEFT;
        break;
    case GridDirection::LEFT:
        thisSquareBarDirection = puz::BAR_LEFT;
        nextSquareBarDirection = puz::BAR_RIGHT;
        break;
    case GridDirection::DOWN:
        thisSquareBarDirection = puz::BAR_BOTTOM;
        nextSquareBarDirection = puz::BAR_TOP;
        break;
    case GridDirection::UP:
        thisSquareBarDirection = puz::BAR_TOP;
        nextSquareBarDirection = puz::BAR_BOTTOM;
        break;
    default:
        return false;
    }
    return square->m_bars[thisSquareBarDirection] || square->Next(dir)->m_bars[nextSquareBarDirection];
}

const Square * Square::FindWordBoundary(const Square * square, GridDirection dir)
{
    // Only white squares have a word
    // direction must be a normal direction.
    if (! square->IsWhite() || (dir % 45) != 0)
        return NULL;

    // Iterate until the grid edge, a black/missing square, or a bar.
    for (;;)
    {
        // Not a white square (one past the end)
        if (! square->IsWhite())
            return square->Prev(dir);
        // Edge of the grid
        if (square->IsLast(dir))
            return square;
        // Bar
        if (HasBarInDirection(square, dir))
            return square;
        square = square->Next(dir);
    }
}

const Square * Square::FindSolutionWordBoundary(const Square * square, GridDirection dir)
{
    // Only white squares have a word
    // direction must be a normal direction.
    if (! square->IsSolutionWhite() || (dir % 45) != 0)
        return NULL;

    // Iterate until the grid edge, a black/missing square, or a bar.
    for (;;)
    {
        // Not a white square (one past the end)
        if (! square->IsSolutionWhite())
            return square->Prev(dir);
        // Edge of the grid
        if (square->IsLast(dir))
            return square;
        // Bar
        if (HasBarInDirection(square, dir))
            return square;
        square = square->Next(dir);
    }
}

const Square * Square::GetWordStart(GridDirection dir) const 
{
    return FindWordBoundary(this, (GridDirection)InvertDirection(dir));
}

const Square * Square::GetWordEnd(GridDirection dir) const
{
    return FindWordBoundary(this, dir);
}

const Square * Square::GetSolutionWordStart(GridDirection dir) const 
{
    return FindSolutionWordBoundary(this, (GridDirection)InvertDirection(dir));
}

const Square * Square::GetSolutionWordEnd(GridDirection dir) const
{
    return FindSolutionWordBoundary(this, dir);
}

Square *
Square::FindWordBoundary(Square * square, GridDirection dir)
{
    return const_cast<Square *>(FindWordBoundary(const_cast<const Square *>(square), dir));
}

Square *
Square::FindSolutionWordBoundary(Square * square, GridDirection dir)
{
    return const_cast<Square *>(FindSolutionWordBoundary(const_cast<const Square *>(square), dir));
}

Square * Square::GetWordStart(GridDirection dir)
{
    return FindWordBoundary(this, (GridDirection)InvertDirection(dir));
}

Square * Square::GetWordEnd(GridDirection dir)
{
    return FindWordBoundary(this, dir);
}

Square * Square::GetSolutionWordStart(GridDirection dir)
{
    return FindSolutionWordBoundary(this, (GridDirection)InvertDirection(dir));
}

Square * Square::GetSolutionWordEnd(GridDirection dir)
{
    return FindSolutionWordBoundary(this, dir);
}


bool Square::HasWord(GridDirection dir) const
{
    const Square * start = GetWordStart(dir);
    const Square * end = GetWordEnd(dir);
    return start && end && start != end;
}


//------------------------------------------------------------------------------
// Clue
//------------------------------------------------------------------------------
bool Square::WantsClue(GridDirection dir) const
{
    return GetWordStart(dir) == this &&
           GetWordEnd  (dir) != this;
}

bool Square::SolutionWantsClue(GridDirection dir) const
{
    return GetSolutionWordStart(dir) == this &&
           GetSolutionWordEnd(dir) != this; // Word end
}



//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------
void Square::SetColor(unsigned char red, unsigned char green, unsigned char blue)
{
    m_red = red;
    m_green = green;
    m_blue = blue;
    if (red == 255 && green == 255 && blue == 255)
        RemoveFlag(FLAG_COLOR);
    else
        AddFlag(FLAG_COLOR);
}


static unsigned char ParseHex(char_t hi, char_t lo)
{
    if (! isxdigit(hi) || ! isxdigit(lo))
        throw Exception("Invalid color string");
    unsigned char b1 = isdigit(hi) ? hi - 48 : toupper(hi) - 65 + 10;
    unsigned char b2 = isdigit(lo) ? lo - 48 : toupper(lo) - 65 + 10;
    return (b1 << 4) + b2;
}

void Square::SetColor(const string_t & hexcolor)
{
    size_t len = hexcolor.size();
    if (len > 0 && hexcolor[0] == puzT('#'))
    {
        SetColor(hexcolor.substr(1));
        return;
    }
    try {
        if (len == 3)
            SetColor(ParseHex(hexcolor[0], hexcolor[0]),
                     ParseHex(hexcolor[1], hexcolor[1]),
                     ParseHex(hexcolor[2], hexcolor[2]));
        else if (len == 6)
            SetColor(ParseHex(hexcolor[0], hexcolor[1]),
                     ParseHex(hexcolor[2], hexcolor[3]),
                     ParseHex(hexcolor[4], hexcolor[5]));
    }
    catch (Exception &) {
        // Don't set the color if we can't parse the hex value
    }
}

static const char_t hexDigits [] = puzT("0123456789abcdef");

string_t Square::GetHtmlColor() const
{
    char_t ret[] = {
        puzT('#'),
        hexDigits[(m_red & 0xf0) >> 4],
        hexDigits[m_red & 0x0f],
        hexDigits[(m_green & 0xf0) >> 4],
        hexDigits[m_green & 0x0f],
        hexDigits[(m_blue & 0xf0) >> 4],
        hexDigits[m_blue & 0x0f],
    };
    return string_t(ret, 7);
}

void Square::SetHighlight(bool doit)
{
    doit ? SetColor(192, 192, 192) : RemoveColor();
}

bool Square::HasHighlight() const
{
    return m_red == 192 && m_blue == 192 && m_green == 192;
}

void Square::RemoveColor()
{
    SetColor(255,255,255);
    RemoveFlag(FLAG_COLOR);
}



//------------------------------------------------------------------------------
// Square text and solution
//------------------------------------------------------------------------------

void Square::SetText(const string_t & text, bool propagate)
{
    if (text.empty())
        m_text = Blank;
    else if (text == Black || IsSymbol(text))
        m_text = text;
    else {
        m_text = ToGrid(text);
        if (m_text.empty())
            m_text = Blank;
    }
    if (propagate && !m_partner.empty()) {
        for (std::vector<Square*>::iterator it = m_partner.begin(); it != m_partner.end(); ++it)
            (*it)->SetText(text, false);
    }
}

void Square::SetSolution(const string_t & solution)
{
    if (solution == Black)
        SetText(Black);
    SetSolutionRebus(solution);
    m_asciiSolution = ToPlain(solution);
    m_solutions.clear();
}

void Square::SetSolution(const string_t & solution, char plain)
{
    SetSolutionRebus(solution);
    SetPlainSolution(plain);
    m_solutions.clear();
}

void Square::SetPlainSolution(char solution)
{
    solution = ToPlain(solution);
#ifdef PUZ_CHECK_STRINGS
    if (solution == 0)
        throw InvalidString();
#endif
    // This could break word start and end

    m_asciiSolution = solution;
}

void Square::SetSolutionRebus(const string_t & rebus)
{
    if (rebus.empty())
        m_solution = Blank;
    else if (rebus == Blank ||
             rebus == Black ||
             IsSymbol(rebus))
        m_solution = rebus;
    else {
        m_solution = ToGrid(rebus);
        if (m_solution.empty())
            m_solution = Blank;
    }
}

void Square::SetSolutions(const std::vector<SolutionEntry> & solutions, const string_t & canonical)
{
    m_solutions = solutions;
    SetSolutionRebus(canonical);
    m_asciiSolution = ToPlain(canonical);
}

string_t Square::DeriveCanonicalSolution(const std::vector<SolutionEntry> & solutions)
{
    if (solutions.empty())
        return Blank;

    if (HasOnlyDirectionalSolutions(solutions))
    {
        std::vector<string_t> dirAnswers;
        std::vector<string_t> seenDirections;
        for (size_t i = 0; i < solutions.size(); ++i)
        {
            if (std::find(seenDirections.begin(), seenDirections.end(), solutions[i].direction) == seenDirections.end())
            {
                seenDirections.push_back(solutions[i].direction);
                dirAnswers.push_back(solutions[i].value);
            }
        }

        string_t canonicalSolution;
        for (size_t i = 0; i < dirAnswers.size(); ++i)
        {
            if (i > 0)
                canonicalSolution += puzT("/");
            canonicalSolution += dirAnswers[i];
        }
        return canonicalSolution;
    }
    else
    {
        return solutions[0].value;
    }
}

void Square::SetSolutions(const std::vector<SolutionEntry> & solutions)
{
    SetSolutions(solutions, DeriveCanonicalSolution(solutions));
}

void Square::ClearSolutions()
{
    m_solutions.clear();
    SetSolutionRebus(Blank);
    m_asciiSolution = ToPlain(Blank);
}

bool Square::HasOnlyDirectionalSolutions(const std::vector<SolutionEntry> & solutions)
{
    if (solutions.empty())
        return false;
    for (size_t i = 0; i < solutions.size(); ++i)
    {
        if (solutions[i].direction.empty())
            return false;
    }
    return true;
}

bool Square::HasOnlyDirectionalSolutions() const
{
    return HasOnlyDirectionalSolutions(m_solutions);
}

void Square::SetSolutionSymbol(unsigned char symbol)
{
    m_solution = puzT("[ ]");
    m_solution.at(1) = static_cast<char_t>(symbol);
}

bool Square::HasTextRebus() const
{
    const size_t len = m_text.length();
    return len > 1 || (len > 0 && m_text.at(0) != GetPlainText());
}


bool Square::HasSolutionRebus() const
{
    const size_t len = m_solution.length();
    return len > 1 || (m_solution.at(0) != GetPlainSolution());
}


bool Square::HasTextSymbol() const
{
    return IsSymbol(m_text);
}


bool Square::HasSolutionSymbol() const
{
    return IsSymbol(m_solution);
}

char_t Square::GetTextSymbol() const
{
    if (! HasTextSymbol())
        throw NoSymbol();
    return m_text.at(1);
}


char_t Square::GetSolutionSymbol() const
{
    if (! HasSolutionSymbol())
        throw NoSymbol();
    return m_solution.at(1);
}





static std::vector<string_t> SplitAndTrim(const string_t & str, char_t delim)
{
    std::vector<string_t> tokens;
    size_t start = 0;
    while (start <= str.length())
    {
        size_t pos = str.find(delim, start);
        if (pos == string_t::npos)
        {
            tokens.push_back(TrimWhitespace(str.substr(start)));
            break;
        }
        tokens.push_back(TrimWhitespace(str.substr(start, pos - start)));
        start = pos + 1;
    }
    return tokens;
}

static bool TokenMatchesDirection(const string_t & token, const string_t & direction,
                                  const std::vector<Square::SolutionEntry> & solutions,
                                  bool strictRebus)
{
    for (size_t i = 0; i < solutions.size(); ++i)
    {
        if (solutions[i].direction == direction)
        {
            if (solutions[i].value == token)
                return true;
            if (! strictRebus && token.length() == 1 &&
                Square::ToPlain(token) == Square::ToPlain(solutions[i].value))
                return true;
        }
    }
    return false;
}

static bool MatchDirectionalTokens(const std::vector<string_t> & tokens,
                                   const std::vector<Square::SolutionEntry> & solutions,
                                   bool strictRebus)
{
    std::vector<string_t> directions;
    for (size_t i = 0; i < solutions.size(); ++i)
    {
        if (std::find(directions.begin(), directions.end(), solutions[i].direction) == directions.end())
            directions.push_back(solutions[i].direction);
    }

    if (tokens.size() != directions.size())
        return false;

    std::vector<size_t> p(directions.size());
    for (size_t i = 0; i < p.size(); ++i)
        p[i] = i;

    do {
        bool allMatch = true;
        for (size_t i = 0; i < p.size(); ++i)
        {
            if (! TokenMatchesDirection(tokens[i], directions[p[i]], solutions, strictRebus))
            {
                allMatch = false;
                break;
            }
        }
        if (allMatch)
            return true;
    } while (std::next_permutation(p.begin(), p.end()));

    return false;
}

static bool MatchTokensToSolutions(const std::vector<string_t> & tokens,
                                   const std::vector<Square::SolutionEntry> & solutions,
                                   bool strictRebus)
{
    if (tokens.size() > solutions.size() || tokens.size() <= 1)
        return false;

    std::vector<size_t> p(solutions.size());
    for (size_t i = 0; i < p.size(); ++i)
        p[i] = i;

    do {
        bool allMatch = true;
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            const Square::SolutionEntry & sol = solutions[p[i]];
            bool match = (tokens[i] == sol.value);
            if (! match && ! strictRebus && tokens[i].length() == 1)
            {
                match = (Square::ToPlain(tokens[i]) == Square::ToPlain(sol.value));
            }
            if (! match)
            {
                allMatch = false;
                break;
            }
        }
        if (allMatch)
            return true;
    } while (std::next_permutation(p.begin(), p.end()));

    return false;
}

bool Square::Check(bool checkBlank, bool strictRebus)  const
{
    // Black squares should be checked as well (for diagramless)
    if (IsBlank() && ! IsSolutionBlank())
        return ! checkBlank;

    if (m_solutions.empty())
    {
        if (IsSolutionBlank())
            return ! checkBlank;

        if (strictRebus || (HasTextRebus() && HasSolutionRebus()))
            return m_solution == m_text;
        else
            return GetPlainText() == GetPlainSolution();
    }

    std::vector<string_t> tokens = SplitAndTrim(m_text, puzT('/'));

    if (HasOnlyDirectionalSolutions())
    {
        // Case 1: Directional solutions
        if (tokens.size() > 1)
            return MatchDirectionalTokens(tokens, m_solutions, strictRebus);

        if (! strictRebus)
        {
            // If user entered a rebus, allow matching any single direction's full answer
            if (HasTextRebus())
            {
                for (size_t i = 0; i < m_solutions.size(); ++i)
                {
                    if (m_text == m_solutions[i].value)
                        return true;
                }
            }
            else
            {
                // Single letter entry: check against plain solution of any valid candidate
                const char plainText = GetPlainText();
                for (size_t i = 0; i < m_solutions.size(); ++i)
                {
                    if (plainText == ToPlain(m_solutions[i].value))
                        return true;
                }
            }
        }
        return false;
    }
    else
    {
        // Case 2: Non-directional / directionless present
        if (tokens.size() > 1)
            return MatchTokensToSolutions(tokens, m_solutions, strictRebus);

        if (m_text == m_solution)
            return true;

        for (size_t i = 0; i < m_solutions.size(); ++i)
        {
            if (m_text == m_solutions[i].value)
                return true;
        }

        if (! strictRebus && ! HasTextRebus())
        {
            if (GetPlainText() == GetPlainSolution())
                return true;

            const char plainText = GetPlainText();
            for (size_t i = 0; i < m_solutions.size(); ++i)
            {
                if (plainText == ToPlain(m_solutions[i].value))
                    return true;
            }
        }
        return false;
    }
}

bool Square::IsSymbol(const string_t & str)
{
    return str.length() == 3
        && str[0] == puzT('[')
        && str[2] == puzT(']');
}

bool Square::IsBetween(const Square * start, const Square * end) const
{
    return m_col >= start->m_col && m_col <= end->m_col
        && m_row >= start->m_row && m_row <= end->m_row;
}

} // namespace puz
