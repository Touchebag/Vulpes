#pragma once

class PhysicsVariables {
  public:
    struct Variables {
        int max;
        int available;
    };

    // This will take a jump and return true if the number of
    // available jumps > 0
    bool popJumps();
    void resetJumps();
    void setMaxJumps(int jumps);

    bool popDashes();
    void resetDashes();
    void setMaxDashes(int dashes);

  private:
    Variables aerial_jumps_;
    Variables dashes_;
};
