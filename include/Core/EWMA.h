#pragma once

class EWMA {
    double alpha;   // smoothing factor
    double value;   // current EWMA
    bool initialized;

public:
    EWMA(double alpha) : alpha(alpha), value(0.0), initialized(false) {}

    void record(bool hit) {
        double x = hit ? 1.0 : 0.0;
        if (!initialized) {
            value = x;  // initialize to first observation
            initialized = true;
        } else {
            value = alpha * x + (1.0 - alpha) * value;
        }
    }

    double getRate() const {
        return value;  // between 0 and 1
    }
};