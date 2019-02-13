#include <iostream>
#include <fstream>

#define GRAPH_NAME "state"

int main() {
    std::ofstream of("state.dot");

    if (!of.good()) {
        std::cout << "Failed to open ouput file" << std::endl;
        exit(1);
    }

    std::string test("A1 -> A2");

    of << "digraph " << GRAPH_NAME << " {" << std::endl;

    of << test << ";" << std::endl;

    of << "}" << std::endl;
}
