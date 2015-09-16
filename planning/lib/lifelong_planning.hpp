#pragma once

#include <limits>
#include <algorithm>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <string>
#include "priority_queue.hpp"

using std::max;
using std::make_pair;
using std::min;
using std::abs;
using std::hypot;
using std::vector;
using std::map;
using std::function;
using std::unordered_map;
using std::string;

namespace search
{
    //
    //  Lifelong planning 
    //
    namespace lp
    {
        constexpr auto infinity() 
        { 
            return std::numeric_limits<int>::max(); 
        }
        constexpr auto cost()
        {
            return 1;
        }

        struct Coordinate
        {
            int x, y;
            
            friend auto operator== (Coordinate l, Coordinate r)
            {
                return l.x == r.x && l.y == r.y;
            }
            friend auto operator!= (Coordinate l, Coordinate r)
            {
                return !(l == r);
            }

            auto neighbours() const
            {
                struct Directions : public map< char, function< Coordinate(Coordinate) >>
                {
                    Directions()
                    {
                        (*this)['1'] = [](Coordinate c) { return Coordinate{ c.x - 1, c.y - 1 }; };
                        (*this)['2'] = [](Coordinate c) { return Coordinate{ c.x - 0, c.y - 1 }; };
                        (*this)['3'] = [](Coordinate c) { return Coordinate{ c.x + 1, c.y - 1 }; };
                        (*this)['4'] = [](Coordinate c) { return Coordinate{ c.x - 1, c.y - 0 }; };
                        (*this)['5'] = [](Coordinate c) { return Coordinate{ c.x + 1, c.y + 0 }; };
                        (*this)['6'] = [](Coordinate c) { return Coordinate{ c.x - 1, c.y + 1 }; };
                        (*this)['7'] = [](Coordinate c) { return Coordinate{ c.x - 0, c.y + 1 }; };
                        (*this)['8'] = [](Coordinate c) { return Coordinate{ c.x + 1, c.y + 1 }; };
                    }
                } static const directions;

                vector<Coordinate> result;
                for (auto n = '1'; n != '9'; ++n)
                    result.push_back(directions.at(n)(*this));
                return result;
            }
        };

        struct LpState
        {
            Coordinate coordinate;
            int g, r;

            friend auto operator==(LpState const& l, LpState const& r)
            {
                return l.coordinate == r.coordinate && l.g == r.g && l.r == r.r;
            }
        };

        class Matrix
        {
        public:
            Matrix(unsigned height, unsigned width)
                : _data{ height, vector<LpState>(width) }
            {
                for (auto y = 0; y != height; ++y)
                    for (auto x = 0; x != width; ++x)
                        at({ x, y }).coordinate = { x, y };
            }

            auto at(Coordinate c) -> LpState&
            {
                return{ _data[c.y][c.x] };
            }

            auto at(Coordinate c) const -> LpState const&
            {
                return{ _data[c.y][c.x] };
            }

        private:
            vector<vector<LpState>> _data;
        };

        struct HeuristcFuncs : public unordered_map < string, function<int(Coordinate, Coordinate)> >
        {
            HeuristcFuncs()
            {
                (*this)["manhattan"] =
                    [](Coordinate curr, Coordinate goal)
                {
                    return max(abs(goal.x - curr.x), abs(goal.y - curr.y));
                };

                (*this)["euclidean"] =
                    [](Coordinate curr, Coordinate goal)
                {
                    return static_cast<int>(round(hypot(abs(goal.x - curr.x), abs(goal.y - curr.y))));
                };
            }
        };

        //
        //  Lifelong A*
        //
        struct LpAstarCore
        {
            struct Key
            {
                const int first, second;

                Key(int fst, int snd)
                    : first{ fst }, second{ snd }
                {   }

                Key(LpState s, function<int(Coordinate, Coordinate)> h, Coordinate g)
                    : Key{ min(s.g, s.r + h(s.coordinate, g)), min(s.g, s.r) }
                {   }

                friend auto operator== (Key l, Key r)
                {
                    return l.first == r.first && l.second == r.second;
                }
                friend auto operator < (Key l, Key r)
                {
                    return (l.first < r.first) || (l.first == r.first && l.second < r.second);
                }
            };

            //
            //  Constructor
            //
            LpAstarCore(unsigned height, unsigned width, Coordinate goal, string heuristic):
                heuristics{},
                matrix{ height, width },
                goal{ goal }, 
                h{ heuristics.at(heuristic) },
                q{ [&](LpState const& lft, LpState const& rht) { return Key{ lft, h, goal } < Key{ rht, h, goal }; } }
            {   }

            HeuristcFuncs const heuristics;
            Matrix matrix;
            Coordinate const goal;
            function<int(Coordinate, Coordinate)> const h;
            PriorityQueue < LpState, function<bool(LpState, LpState)>> q;
        };
    }
}