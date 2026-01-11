#pragma once

struct Square {
    int r;
    int c;
};

struct Move {
    Square current;
    Square destination;
};