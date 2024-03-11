g++ -o a -std=c++17 \
    *.cpp \
    -O3 \
    -D_THREAD_SAFE -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2