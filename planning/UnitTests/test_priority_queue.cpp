#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace search;
using std::max;
using std::abs;

namespace UnitTests
{
    struct F
    {
        auto operator()(Node const& node) const-> std::size_t
        {
            return cost(node) + manhattan(node);
        }

        auto cost(Node const& node) const -> std::size_t
        {
            return node.path().size();
        }

        auto manhattan(Node const& node) const -> std::size_t
        {
            auto dy = abs(node.goal().y - node.coordinate().y);
            auto dx = abs(node.goal().x - node.coordinate().x);
            return max(dy, dx);
        }
    };

    struct Less
    {
        F f;
        auto operator()(Node const& lhs, Node const& rhs) const -> bool
        {
            return f(lhs) < f(rhs);
        }
    };

    TEST_CLASS(test_priority_queue)
    {
    public:
        TEST_METHOD(pq_parent)
        {
            auto seq = { 5, 6, 9, 3, 2, 7 };
            auto p = parent(seq.begin(), seq.begin());
            Assert::IsTrue(seq.begin() == parent(seq.begin(), seq.begin() + 1));
            Assert::IsTrue(seq.begin() == parent(seq.begin(), seq.begin() + 2));
        }

        TEST_METHOD(pq_left_child)
        {
            auto seq = { 5, 6, 9, 3, 2, 7 };
            auto left = left_child(seq.begin(), seq.end(), seq.begin());
            Assert::IsTrue(left == seq.begin() + 1);
        }

        TEST_METHOD(pq_right_child)
        {
            auto seq = { 5, 6, 9, 3, 2, 7 };
            auto right = right_child(seq.begin(), seq.end(), seq.begin());
            Assert::IsTrue(right == seq.begin() + 2);
        }

        TEST_METHOD(pq_heapify)
        {
            auto seq = std::vector < int >{ 5, 6, 9, 3, 2, 7 };
            heapify(seq.begin(), seq.end(), seq.begin(), std::greater<int>());
            std::vector<int> expect = { 9, 6, 7, 3, 2, 5 };
            for (auto i = 0u; i != seq.size(); ++i)
                Assert::AreEqual(expect[i], seq[i]);
        }

        TEST_METHOD(pq_build_heap)
        {
            auto seq = std::vector < int >{ 4, 1, 3, 2, 16, 9, 10, 14, 8, 7 };
            build_heap(seq.begin(), seq.end(), std::greater<int>());
            std::vector<int> expect = { 16, 14, 10, 8, 7, 9, 3, 2, 4, 1 };
            for (auto i = 0u; i != seq.size(); ++i)
                Assert::AreEqual(expect[i], seq[i]);
        }

        TEST_METHOD(pq_ctors)
        {
            //test with int
            {
                using Less = decltype(std::less<int>{});
                auto pq = PriorityQueue<int, Less>{ { 4, 6, 7, 1 }, Less{} };
                Assert::AreEqual(4u, pq.size());
                Assert::AreEqual(1, pq.top());  pq.pop();
                Assert::AreEqual(4, pq.top());  pq.pop();
                Assert::AreEqual(6, pq.top());  pq.pop();
                Assert::AreEqual(7, pq.top());  pq.pop();
                Assert::AreEqual(0u,  pq.size());
            }

            //test with node
            {
                //  [start] -> (goal)
                // [1,1] 1,2  1,3       1   2   3
                //  2,1 {2,2} 2,3       4       5 
                //  3,1  3,2 (3,3)      6   7   8
                auto start = Coordinate{ 1, 1 };
                auto goal = Coordinate{ 3, 3 };
                auto path = std::string{ "8" };
                auto nodes = std::vector<Node>{};
                for (auto i = '1'; i != '9'; ++i)
                    nodes.emplace_back(path + i, start, goal);

                F f;
                auto fvalues = std::vector<std::size_t>{ 4, 4, 4, 4, 3, 4, 3, 2 };
                for (auto i = 0u; i != fvalues.size(); ++i)
                    Assert::AreEqual(fvalues[i], f(nodes[i]));

                auto pq = PriorityQueue<Node, Less>{ nodes.cbegin(), nodes.cend(), Less{} };
                Assert::AreEqual(std::string{ "88" }, pq.top().path()); pq.pop();
                Assert::AreEqual(std::string{ "85" }, pq.top().path()); pq.pop();
                Assert::AreEqual(std::string{ "87" }, pq.top().path()); pq.pop();
                Assert::AreEqual(std::string{ "86" }, pq.top().path()); pq.pop();
                Assert::AreEqual(std::string{ "81" }, pq.top().path()); pq.pop();
                Assert::AreEqual(std::string{ "82" }, pq.top().path()); pq.pop();
                Assert::AreEqual(std::string{ "83" }, pq.top().path()); pq.pop();
                Assert::AreEqual(std::string{ "84" }, pq.top().path()); pq.pop();
            }
        }

        TEST_METHOD(pq_push)
        {
            auto start = Coordinate{ 1, 1 };
            auto goal = Coordinate{ 3, 3 };
            auto path = std::string{ "8" };
            auto nodes = std::vector<Node>{};
            for (auto i = '1'; i != '9'; ++i)
                nodes.emplace_back(path + i, start, goal);

            auto pq = PriorityQueue<Node, Less>{ Less{} };
            for (auto const& n : nodes)
                pq.push(n);

            F f;
            auto fvalues = std::vector<std::size_t>{ 4, 4, 4, 4, 3, 4, 3, 2 };
            for (auto i = 0u; i != fvalues.size(); ++i)
                Assert::AreEqual(fvalues[i], f(nodes[i]));

            Assert::AreEqual(std::string{ "88" }, pq.top().path()); pq.pop();
            Assert::AreEqual(std::string{ "85" }, pq.top().path()); pq.pop();
            Assert::AreEqual(std::string{ "87" }, pq.top().path()); pq.pop();
            Assert::AreEqual(std::string{ "86" }, pq.top().path()); pq.pop();
            Assert::AreEqual(std::string{ "82" }, pq.top().path()); pq.pop();
            Assert::AreEqual(std::string{ "81" }, pq.top().path()); pq.pop();
            Assert::AreEqual(std::string{ "83" }, pq.top().path()); pq.pop();
            Assert::AreEqual(std::string{ "84" }, pq.top().path()); pq.pop();
        }
    };
}