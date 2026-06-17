#include <print>
#include "splay_tree.h"

int main()
{
    splay_tree<int> tree{1, 2, 3, 4, 5};
    std::println("{}", tree.level_order());
}